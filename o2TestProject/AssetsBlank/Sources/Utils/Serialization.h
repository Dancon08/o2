#pragma once

#include "Utils/Containers/Dictionary.h"
#include "Utils/Data/DataDoc.h"
#include "Utils/IObject.h"
#include "Utils/Reflection/Types.h"
#include "Utils/String.h"

namespace o2
{
	// -----------------------------
	// Serializable object interface
	// -----------------------------
	class ISerializable: public IObject
	{
	public:
		// Serializing object into data node
		virtual DataNode Serialize() = 0;

		// Deserializing object from data node
		virtual void Deserialize(const DataNode& node) = 0;

		// DataNode converting operator
		virtual operator DataNode() = 0;

		// Assign operator from data node
		virtual ISerializable& operator=(const DataNode& node) = 0;

	protected:
		// Beginning serialization callback
		virtual void OnSerialize() {}

		// Completion deserialization callback
		virtual void OnDeserialized() {}
	};

	// --------------------------------------
	// Serializable field attribute interface
	// --------------------------------------
	class ISerializableAttribute: public IAttribute
	{
	public:
		virtual void Serialize(void* object, DataNode& data) const = 0;
		virtual void Deserialize(void* object, const DataNode& data) const = 0;
	};

	// -----------------------------------
	// Specialized serialization attribute
	// -----------------------------------
	template<typename _type>
	class SerializableAttribute: public ISerializableAttribute
	{
	public:
		void Serialize(void* object, DataNode& data) const;
		void Deserialize(void* object, const DataNode& data) const;
	};

	// Registering field in type with serialization attribute
#define SRLZ_FIELD(NAME) \
	type.RegField(#NAME, (char*)(&sample->NAME) - (char*)sample).AddAttribute<SerializableAttribute<decltype(NAME)>>()

// Serialization implementation macros
#define SERIALIZABLE_IMPL(CLASS)                                                          \
	DataNode Serialize()                                                        		  \
	{                                                              						  \
		OnSerialize();																	  \
		DataNode res;																	  \
		for (auto field : type.Fields())												  \
		{																				  \
			ISerializableAttribute* srlzAttr = field.Attribute<ISerializableAttribute>(); \
			if (srlzAttr)																  \
				srlzAttr->Serialize(this, res);										      \
		}																				  \
																						  \
		return res;																		  \
	}																					  \
	void Deserialize(const DataNode& node)												  \
	{																					  \
		for (auto field : type.Fields())												  \
		{																				  \
			ISerializableAttribute* srlzAttr = field.Attribute<ISerializableAttribute>(); \
			if (srlzAttr)																  \
				srlzAttr->Deserialize(this, node);										  \
		}																				  \
		OnDeserialized();																  \
	}																					  \
	CLASS& operator=(const DataNode& node) 												  \
	{																					  \
		Deserialize(node); return *this; 												  \
	} 																					  \
	operator DataNode() 																  \
	{ 																					  \
		return Serialize(); 															  \
	}       


	template<typename _type>
	void SerializableAttribute<_type>::Deserialize(void* object, const DataNode& data) const
	{
		_type value;
		value = *data.GetNode(mOwnerFieldInfo->Name());
		mOwnerFieldInfo->SetValue<_type>(object, value);
	}

	template<typename _type>
	void SerializableAttribute<_type>::Serialize(void* object, DataNode& data) const
	{
		*data.AddNode(mOwnerFieldInfo->Name()) = mOwnerFieldInfo->GetValue<_type>(object);
	}
}