#include "AssetInfo.h"

namespace o2
{
	AssetInfo::AssetInfo():
		mId(0), mType(0)
	{}

	AssetInfo::AssetInfo(const String& path, UID id, Type::Id type):
		mPath(path), mId(id), mType(type)
	{}

	bool AssetInfo::operator==(const AssetInfo& other) const
	{
		return mId == other.mId;
	}
}
 
CLASS_META(o2::AssetInfo)
{
	BASE_CLASS(o2::ISerializable);

	PUBLIC_FIELD(mType).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mPath).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mId).SERIALIZABLE_ATTRIBUTE();
}
END_META;
 