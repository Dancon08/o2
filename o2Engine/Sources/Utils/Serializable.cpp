#include "Serializable.h"

namespace o2
{
	DataNode ISerializable::SerializeBasic(const void* thisObject) const
	{
		DataNode res;
		OnSerialize(res);
		char* thisPtr = (char*)thisObject;
		for (auto field : GetType().Fields())
		{
			auto srlzAttribute = field->Attribute<SerializableAttribute>();
			if (srlzAttribute)
			{
				field->SerializeObject((void*)field->GetValuePtrStrong<char>(thisPtr), *res.AddNode(field->Name()));
			}
		}

		return res;
	}

	void ISerializable::DeserializeBasic(const DataNode& node, const void* thisObject)
	{
		char* thisPtr = (char*)thisObject;
		for (auto field : GetType().Fields())
		{
			auto srlzAttribute = field->Attribute<SerializableAttribute>();
			if (srlzAttribute)
			{
				auto fldNode = node.GetNode(field->Name());
				if (fldNode)
				{
					field->DeserializeObject((void*)field->GetValuePtrStrong<char>(thisPtr), *fldNode);
				}
			}
		}
		OnDeserialized(node);
	}
}
 
CLASS_META(o2::ISerializable)
{
	BASE_CLASS(o2::IObject);


	PUBLIC_FUNCTION(DataNode, Serialize);
	PUBLIC_FUNCTION(void, Deserialize, const DataNode&);
	PROTECTED_FUNCTION(void, OnSerialize, DataNode&);
	PROTECTED_FUNCTION(void, OnDeserialized, const DataNode&);
	PROTECTED_FUNCTION(DataNode, SerializeBasic, const void*);
	PROTECTED_FUNCTION(void, DeserializeBasic, const DataNode&, const void*);
}
END_META;
 