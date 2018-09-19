#include "stdafx.h"
#include "AssetsTree.h"

#include "Assets/FolderAsset.h"
#include "Utils/Debug/Debug.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Debug/Log/LogStream.h"

namespace o2
{
	AssetTree::AssetNode::AssetNode():
		meta(nullptr)
	{}

	AssetTree::AssetNode::AssetNode(const String& path, UID id, const Type* type) :
		AssetInfo(path, id, type)
	{}

	AssetTree::AssetNode::~AssetNode()
	{
		delete meta;
	}

	AssetTree::AssetNode* AssetTree::AssetNode::AddChild(AssetNode* node)
	{
		if (node->parent)
			node->parent->RemoveChild(node, false);

		node->parent = this;

		children.Add(node);

		return node;
	}

	bool AssetTree::AssetNode::RemoveChild(AssetNode* node, bool release /*= true*/)
	{
		node->parent = nullptr;

		if (!children.Remove(node))
			return false;

		if (release && node)
			delete node;

		return true;
	}

	void AssetTree::AssetNode::SetParent(AssetNode* parent)
	{
		if (parent)
		{
			parent->AddChild(this);
		}
		else
		{
			if (parent)
				parent->RemoveChild(this, false);

			parent = nullptr;
		}
	}

	void AssetTree::AssetNode::OnDeserialized(const DataNode& node)
	{
		AssetInfo::OnDeserialized(node);

		for (auto child : children)
			child->parent = this;
	}

	AssetTree::AssetTree() :
		mLog(nullptr)
	{}

	AssetTree::~AssetTree()
	{
		Clear();
	}

	void AssetTree::BuildTree(const String& path)
	{
		mPath = path;

		FolderInfo folderInfo = o2FileSystem.GetFolderInfo(path);
		folderInfo.ClampPathNames();
		LoadFolder(folderInfo, nullptr);
	}

	void AssetTree::RebuildTree()
	{
		FolderInfo folderInfo = o2FileSystem.GetFolderInfo(mPath);
		folderInfo.ClampPathNames();
		LoadFolder(folderInfo, nullptr);
	}

	AssetTree::AssetNode* AssetTree::FindAsset(const String& path) const
	{
		return mAllAssets.FindMatch([&](const AssetNode* asset) { return asset->path == path; });
	}

	AssetTree::AssetNode* AssetTree::FindAsset(UID id) const
	{
		return mAllAssets.FindMatch([&](const AssetNode* asset) { return asset->id == id; });
	}

	AssetInfo AssetTree::FindAssetInfo(const String& path) const
	{
		auto asset = FindAsset(path);
		if (asset)
			return (AssetInfo)(*asset);

		return AssetInfo();
	}

	AssetInfo AssetTree::FindAssetInfo(UID id) const
	{
		auto asset = FindAsset(id);
		if (asset)
			return (AssetInfo)(*asset);

		return AssetInfo();
	}

	AssetTree::AssetNode* AssetTree::AddAsset(AssetNode* asset)
	{
		int delPos = asset->path.FindLast("/");
		if (delPos < 0)
		{
			mAllAssets.Add(asset);
		}
		else
		{
			String parentPath = asset->path.SubStr(0, delPos);
			AssetNode* parent = mAllAssets.FindMatch([&](auto info) { return info->path == parentPath; });

			if (!parent)
			{
				if (mLog) mLog->Out("Failed to add builded asset info: " + asset->path);
			}
			else
			{
				parent->AddChild(asset);
			}

			mAllAssets.Add(asset);
		}


		return asset;
	}

	void AssetTree::RemoveAsset(AssetNode* asset, bool release /*= true*/)
	{
		mAllAssets.Remove(asset);

		if (asset->parent)
			asset->parent->RemoveChild(asset, false);
		else
			mRootAssets.Remove(asset);

		if (asset->assetType == &TypeOf(FolderAsset) && release)
		{
			auto& childs = asset->children;
			for (auto ch : childs)
				RemoveAsset(ch, release);
		}

		if (release)
			delete asset;
	}

	void AssetTree::Clear()
	{
		for (auto asset : mRootAssets)
			delete asset;

		mAllAssets.Clear();
		mRootAssets.Clear();
	}

	void AssetTree::LoadFolder(FolderInfo& folder, AssetNode* parentAsset)
	{
		Vector<AssetNode*> missingAssetNodes;
		auto& parentChilds = parentAsset ? parentAsset->children : mRootAssets;
		for (auto assetNode : parentChilds)
		{
			bool exist = false;

			if (assetNode->assetType == &TypeOf(FolderAsset))
				exist = folder.mFolders.ContainsPred([=](const FolderInfo& x) { return x.mPath == assetNode->path; });
			else
				exist = folder.mFiles.ContainsPred([=](const FileInfo& x) { return x.mPath == assetNode->path; });

			if (!exist)
				missingAssetNodes.Add(assetNode);
		}

		for (auto assetNode : missingAssetNodes)
			RemoveAsset(assetNode);

		for (auto fileInfo : folder.mFiles)
		{
			if (parentChilds.ContainsPred([&](AssetNode* x) { return x->path == fileInfo.mPath; }))
				continue;

			String extension = o2FileSystem.GetFileExtension(fileInfo.mPath);

			if (extension != "meta")
			{
				String assetFullPath = mPath + fileInfo.mPath;
				String metaFullPath = assetFullPath + ".meta";

				bool isExistMetaForAsset = o2FileSystem.IsFileExist(metaFullPath);
				if (!isExistMetaForAsset)
					continue;

				LoadAsset(fileInfo.mPath, parentAsset, fileInfo.mEditDate);
			}
		}

		for (auto subFolder : folder.mFolders)
		{
			AssetNode* asset = nullptr;

			if (!parentChilds.ContainsPred([&](AssetNode* x) { return x->path == subFolder.mPath; }))
			{
				String folderFullPath = mPath + subFolder.mPath;
				String metaFullPath = folderFullPath + ".meta";

				bool isExistMetaForFolder = o2FileSystem.IsFileExist(metaFullPath);
				if (!isExistMetaForFolder)
				{
					mLog->Warning("Can't load asset info for " + subFolder.mPath + " - missing meta file");
					continue;
				}

				asset = LoadAsset(subFolder.mPath, parentAsset, TimeStamp());
			}
			else asset = parentChilds.FindMatch([&](AssetNode* x) { return x->path == subFolder.mPath; });

			LoadFolder(subFolder, asset);
		}
	}

	AssetTree::AssetNode* AssetTree::LoadAsset(const String& path, AssetNode* parent, const TimeStamp& time)
	{
		DataNode metaData;
		metaData.LoadFromFile(mPath + path + ".meta");
		o2::Asset::IMetaInfo* meta;
		meta = metaData;

		AssetNode* asset = mnew AssetNode();

		asset->meta = meta;
		asset->path = path;
		asset->time = time;
		asset->id = meta->ID();
		asset->assetType = meta->GetAssetType();

// 		meta->r();
// 		DataNode xx;
// 		xx = meta;
// 		xx.SaveToFile(mPath + path + ".meta");

		asset->SetParent(parent);

		mAllAssets.Add(asset);

		if (!parent)
			mRootAssets.Add(asset);

		return asset;
	}

	void AssetTree::OnDeserialized(const DataNode& node)
	{
		struct helper
		{
			static void Do(AssetsVec& assets, AssetsVec& children)
			{
				assets.Add(children);

				for (auto child : children)
					Do(assets, child->children);
			}
		};

		helper::Do(mAllAssets, mRootAssets);
	}

}

DECLARE_CLASS(o2::AssetTree);

DECLARE_CLASS(o2::AssetTree::AssetNode);
