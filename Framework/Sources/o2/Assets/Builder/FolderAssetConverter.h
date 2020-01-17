#pragma once

#include "IAssetConverter.h"

namespace o2
{
	// ----------------
	// Folder converter
	// ----------------
	class FolderAssetConverter: public IAssetConverter
	{
	public:
		// Returns vector of processing assets types
		Vector<const Type*> GetProcessingAssetsTypes() const;

		// Converts folder by path
		void ConvertAsset(const AssetsTree::AssetNode& node);

		// Removes folder by path
		void RemoveAsset(const AssetsTree::AssetNode& node);

		// Moves folder to new path
		void MoveAsset(const AssetsTree::AssetNode& nodeFrom, const AssetsTree::AssetNode& nodeTo);

		// Post processing folders
		Vector<UID> AssetsPostProcess();

		// Resets converter
		void Reset();

		IOBJECT(FolderAssetConverter);

	protected:
		Vector<String> mRemovedFolders; // Removes folders
	};
}

CLASS_BASES_META(o2::FolderAssetConverter)
{
	BASE_CLASS(o2::IAssetConverter);
}
END_META;
CLASS_FIELDS_META(o2::FolderAssetConverter)
{
	PROTECTED_FIELD(mRemovedFolders);
}
END_META;
CLASS_METHODS_META(o2::FolderAssetConverter)
{

	PUBLIC_FUNCTION(Vector<const Type*>, GetProcessingAssetsTypes);
	PUBLIC_FUNCTION(void, ConvertAsset, const AssetsTree::AssetNode&);
	PUBLIC_FUNCTION(void, RemoveAsset, const AssetsTree::AssetNode&);
	PUBLIC_FUNCTION(void, MoveAsset, const AssetsTree::AssetNode&, const AssetsTree::AssetNode&);
	PUBLIC_FUNCTION(Vector<UID>, AssetsPostProcess);
	PUBLIC_FUNCTION(void, Reset);
}
END_META;
