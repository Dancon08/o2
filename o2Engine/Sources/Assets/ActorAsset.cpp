#include "ActorAsset.h"

#include "Assets/Assets.h"

namespace o2
{
	ActorAsset::ActorAsset():
		Asset(), actor(Actor::CreateMode::NotInScene)
	{
		mMeta = mnew MetaInfo();
		InitializeProperties();

		actor.ExcludeFromScene();
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();
	}

	ActorAsset::ActorAsset(const String& path):
		Asset(), actor(Actor::CreateMode::NotInScene)
	{
		mPath = path;
		mMeta = mnew MetaInfo();
		IdRef() = o2Assets.GetAssetId(path);
		InitializeProperties();

		actor.ExcludeFromScene();
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();

		Load();
	}

	ActorAsset::ActorAsset(UID id):
		Asset(), actor(Actor::CreateMode::NotInScene)
	{
		mMeta = mnew MetaInfo();
		IdRef() = id;
		mPath = o2Assets.GetAssetPath(id);
		InitializeProperties();

		actor.ExcludeFromScene();
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();

		Load();
	}

	ActorAsset::ActorAsset(const ActorAsset& asset):
		Asset(asset), actor(Actor::CreateMode::NotInScene)
	{
		mMeta = mnew MetaInfo();
		mPath = asset.mPath;
		IdRef() = asset.GetAssetId();

		actor = asset.actor;
		actor.ExcludeFromScene();
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();

		InitializeProperties();
	}

	ActorAsset::~ActorAsset()
	{}

	ActorAsset& ActorAsset::operator=(const ActorAsset& asset)
	{
		Asset::operator=(asset);

		actor = asset.actor;
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();

		*mMeta = *(MetaInfo*)(asset.mMeta);

		return *this;
	}

	bool ActorAsset::operator==(const ActorAsset& other) const
	{
		return mMeta->IsEqual(other.mMeta);
	}

	bool ActorAsset::operator!=(const ActorAsset& other) const
	{
		return !mMeta->IsEqual(other.mMeta);
	}

	ActorAsset::MetaInfo* ActorAsset::GetMeta() const
	{
		return (MetaInfo*)mMeta;
	}

	const char* ActorAsset::GetFileExtensions() const
	{
		return "prefab prf actor act";
	}

	void ActorAsset::LoadData(const String& path)
	{
		DataNode data;
		data.LoadFromFile(path);
		actor.Deserialize(data);
		actor.ExcludeFromScene();
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();
	}

	void ActorAsset::SaveData(const String& path)
	{
		actor.mIsAsset = true;
		actor.mAssetId = IdRef();

		DataNode data;
		data = actor.Serialize();
		data.SaveToFile(path);
	}

	void ActorAsset::InitializeProperties()
	{
		INITIALIZE_GETTER(ActorAsset, meta, GetMeta);
	}

	Type::Id ActorAsset::MetaInfo::GetAssetType() const
	{
		return TypeOf(ActorAsset).ID();
	}
}
 
CLASS_META(o2::ActorAsset)
{
	BASE_CLASS(o2::Asset);

	PUBLIC_FIELD(actor);
	PUBLIC_FIELD(meta);

	PUBLIC_FUNCTION(MetaInfo*, GetMeta);
	PUBLIC_FUNCTION(const char*, GetFileExtensions);
	PROTECTED_FUNCTION(void, LoadData, const String&);
	PROTECTED_FUNCTION(void, SaveData, const String&);
	PROTECTED_FUNCTION(void, InitializeProperties);
}
END_META;

CLASS_META(o2::ActorAsset::MetaInfo)
{
	BASE_CLASS(o2::Asset::IMetaInfo);


	PUBLIC_FUNCTION(Type::Id, GetAssetType);
}
END_META;
 