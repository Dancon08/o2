#pragma once

#include "Assets/Asset.h"

namespace o2
{
	// ------------
	// Folder asset
	// ------------
	class FolderAsset: public Asset
	{
	public:
		class MetaInfo;

	public:
		Getter<MetaInfo*>     meta;         // Meta information getter
		Getter<AssetInfosVec> insideAssets; // Inside assets infos getter

		// Default constructor
		FolderAsset();

		// Constructor by path - loads asset by path
		FolderAsset(const String& path);

		// Constructor by id - loads asset by id
		FolderAsset(UID id);

		// Copy-constructor
		FolderAsset(const FolderAsset& asset);

		// Destructor
		~FolderAsset();

		// Check equals operator
		FolderAsset& operator=(const FolderAsset& asset);

		// Check equals operator
		bool operator==(const FolderAsset& other) const;

		// Check not equals operator
		bool operator!=(const FolderAsset& other) const;

		// Returns containing assets infos
		AssetInfosVec GetContainingAssetsInfos() const;

		// Returns meta information
		MetaInfo* GetMeta() const;

		SERIALIZABLE(FolderAsset);

	public:
		// ----------------
		// Meta information
		// ----------------
		class MetaInfo: public IMetaInfo
		{
		public:
			// Returns asset type id
			TypeId GetAssetType() const;

			SERIALIZABLE(MetaInfo);
		};

	protected:
		AssetInfosVec mContainingAssetsInfos; // Inside assets infos

	protected:
		// Loads data
		void LoadData(const String& path);

		// Saves asset data
		void SaveData(const String& path);

		// Initializes properties
		void InitializeProperties();
	};
}
