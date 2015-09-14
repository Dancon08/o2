#include "Assets.h"

#include "Assets/Asset.h"
#include "Assets/BinaryAsset.h"
#include "Assets/Builder/AssetsBuilder.h"
#include "Utils/Debug.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Log/LogStream.h"

namespace o2
{
	DECLARE_SINGLETON(Assets);

	Assets::Assets()
	{
		InitializeProperties();

		mLog = mnew LogStream("Assets");
		Debug.GetLog()->BindStream(mLog);

		mAssetsBuilder = mnew AssetsBuilder();

		LoadAssetTypes();

		if (ASSETS_PREBUILDING_ENABLE)
			RebuildAssets();
		else
			LoadAssetsInfos();
	}

	Assets::~Assets()
	{
		mAssetsBuilder.Release();
	}

	String Assets::GetAssetsPath() const
	{
		return ASSETS_PATH;
	}

	String Assets::GetDataPath() const
	{
		return DATA_PATH;
	}

	String Assets::GetAssetPath(UInt id) const
	{
		return GetAssetInfo(id).mPath;
	}

	UInt Assets::GetAssetId(const String& path) const
	{
		return GetAssetInfo(path).mId;
	}

	AssetInfo Assets::GetAssetInfo(UInt id) const
	{
		for (auto info : mAssetsInfos)
		{
			if (info.mId == id)
				return info;

			AssetInfo childSearch = info.FindAssetInfo(id);
			if (childSearch.mId != 0)
				return childSearch;
		}

		return AssetInfo();
	}

	AssetInfo Assets::GetAssetInfo(const String& path) const
	{
		for (auto info : mAssetsInfos)
		{
			if (info.mPath == path)
				return info;

			AssetInfo childSearch = info.FindAssetInfo(path);
			if (childSearch.mId != 0)
				return childSearch;
		}

		return AssetInfo();
	}

	UInt Assets::GetRandomAssetId()
	{
		return Math::Random<UInt>(1, UINT_MAX);
	}

	const Assets::TypesExtsDict Assets::GetAssetsExtensionsTypes() const
	{
		return mAssetsTypes;
	}

	const Type* Assets::GetStdAssetType() const
	{
		return mStdAssetType;
	}

	const Type* Assets::GetAssetTypeByExtension(const String& extension) const
	{
		if (mAssetsTypes.ContainsKey(extension))
			return mAssetsTypes[extension];

		return mStdAssetType;
	}

	Ptr<Asset> Assets::LoadAsset(const AssetInfo& info)
	{
		Type* assetType = Types::GetType(info.mType);
		if (!assetType)
		{
			mLog->Error("Failed to load asset by info: incorrect type (%i)", info.mType);
			return nullptr;
		}

		Ptr<Asset> res = static_cast<Asset*>(assetType->Sample()->Clone());
		res->Load(info);
		return res;
	}

	bool Assets::IsAssetExist(const String& path) const
	{
		return GetAssetInfo(path).mId != 0;
	}

	bool Assets::IsAssetExist(UInt id) const
	{
		return GetAssetInfo(id).mId != 0;
	}

	bool Assets::IsAssetExist(const AssetInfo& info) const
	{
		return GetAssetInfo(info.mId).mId == 0;
	}

	bool Assets::RemoveAsset(Ptr<Asset> asset, bool rebuildAssets /*= true*/)
	{
		return RemoveAsset(asset->GetAssetId());
	}

	bool Assets::RemoveAsset(const String& path, bool rebuildAssets /*= true*/)
	{
		AssetInfo info = GetAssetInfo(path);

		if (info.mId == 0)
		{
			mLog->Error("Can't remove asset by path (%s) - asset isn't exist", (char*)path);
			return false;
		}

		return o2FileSystem.FileDelete(GetAssetsPath() + info.mPath);
	}

	bool Assets::RemoveAsset(UInt id, bool rebuildAssets /*= true*/)
	{
		AssetInfo info = GetAssetInfo(id);

		if (info.mId == 0)
		{
			mLog->Error("Can't remove asset by id (%i) - asset isn't exist", id);
			return false;
		}

		return o2FileSystem.FileDelete(GetAssetsPath() + info.mPath);
	}

	bool Assets::RemoveAsset(const AssetInfo& info, bool rebuildAssets /*= true*/)
	{
		return RemoveAsset(info.mId, rebuildAssets);
	}

	bool Assets::MoveAsset(Ptr<Asset> asset, const String& newPath, bool rebuildAssets /*= true*/)
	{
		return MoveAsset(asset->GetAssetId(), newPath);
	}

	bool Assets::MoveAsset(const String& path, const String& newPath, bool rebuildAssets /*= true*/)
	{
		AssetInfo info = GetAssetInfo(path);

		if (info.mId == 0)
		{
			mLog->Error("Can't remove asset by path (%s) - asset isn't exist", (char*)path);
			return false;
		}

		if (GetAssetId(newPath) != 0)
		{
			mLog->Error("Can't remove asset by path (%s) \nto new path (%s)\n - another asset exist in target path", 
						(char*)path, (char*)newPath);
			return false;
		}

		return o2FileSystem.FileMove(GetAssetsPath() + info.mPath, GetAssetsPath() + newPath);
	}

	bool Assets::MoveAsset(UInt id, const String& newPath, bool rebuildAssets /*= true*/)
	{
		AssetInfo info = GetAssetInfo(id);

		if (info.mId == 0)
		{
			mLog->Error("Can't remove asset by id (%i) - asset isn't exist", id);
			return false;
		}

		if (GetAssetId(newPath) != 0)
		{
			mLog->Error("Can't remove asset by id (%s) \nto new path (%s)\n - another asset exist in target path",
						id, (char*)newPath);
			return false;
		}

		return o2FileSystem.FileMove(GetAssetsPath() + info.mPath, GetAssetsPath() + newPath);
	}

	bool Assets::MoveAsset(const AssetInfo& info, const String& newPath, bool rebuildAssets /*= true*/)
	{
		return MoveAsset(info.mId, newPath, rebuildAssets);
	}

	void Assets::RebuildAssets(bool forcible /*= false*/)
	{
		mAssetsBuilder->BuildAssets(GetAssetsPath(), GetDataPath());
		LoadAssetsInfos();
	}

	void Assets::LoadAssetsInfos()
	{
		String buildedAssetsPath = (String)DATA_PATH + "AssetsInfo.xml";
		DataNode data;
		data.LoadFromFile(buildedAssetsPath);
		mAssetsInfos = data;
	}

	void Assets::LoadAssetTypes()
	{
		mStdAssetType = &BinaryAsset::type;

		auto assetTypes = Asset::type.InheritedTypes();

		for (auto type : assetTypes)
		{
			String extensions = static_cast<const Asset*>(type->Sample())->GetFileExtensions();
			auto extensionsVec = extensions.Split(" ");

			for (auto ext : extensionsVec)
			{
				if (mAssetsTypes.ContainsKey(ext))
				{
					mLog->Warning("Assets extensions duplicating: %s, at %s and %s", 
								  ext, mAssetsTypes[ext]->Name(), type->Name());
					continue;
				}

				mAssetsTypes.Add(ext, type);
			}
		}
	}

	void Assets::InitializeProperties()
	{
		INITIALIZE_GETTER(Assets, AssetsPath, GetAssetsPath);
	}
}