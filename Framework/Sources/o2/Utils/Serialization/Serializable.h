
#pragma once

#include "o2/Utils/Types/Containers/Map.h"
#include "o2/Utils/Serialization/DataValue.h"
#include "o2/Utils/Basic/IObject.h"
#include "o2/Utils/Reflection/Reflection.h"
#include "o2/Utils/Types/String.h"
#include "o2/Utils/Singleton.h"

namespace o2
{
	// -----------------------------
	// Serializable object interface
	// -----------------------------
	class ISerializable: public IObject
	{
	public:
		// Serializing object into data node
		virtual void Serialize(DataValue& node) const {}

		// Deserializing object from data node
		virtual void Deserialize(const DataValue& node) {};

		// Serializes data to string
		String SerializeToString() const;

		// Deserializes data from string
		void DeserializeFromString(const String& str);

		// Assign operator from data node
		ISerializable& operator=(const DataValue& node) { return *this; };

		// DataDocument converting operator
		operator DataDocument() const { return DataDocument(); }

		// Beginning serialization callback
		virtual void OnSerialize(DataValue& node) const {}

		// Completion deserialization callback
		virtual void OnDeserialized(const DataValue& node) {}

		IOBJECT(ISerializable);

	protected:
		// Serializing object into data node
		void SerializeBasic(const IObject& thisObject, DataValue& node) const;

		// Deserializing object from data node
		void DeserializeBasic(IObject& thisObject, const DataValue& node);
	};

	// ----------------------------
	// Serializable field attribute
	// ----------------------------
	class SerializableAttribute: public IAttribute
	{
		ATTRIBUTE_COMMENT_DEFINITION("SERIALIZABLE");
		ATTRIBUTE_SHORT_DEFINITION("SERIALIZABLE_ATTRIBUTE");
	};

	class SerializationTypeProcessor
	{
		// !!! Remove attributes pointer!!!
	public:
		DataValue& node;

	public:
		SerializationTypeProcessor(DataValue& node):node(node) {}

		template<typename _object_type, typename _base_type>
		void BaseType(_object_type* object, Type* type, const char* name)
		{
			if constexpr (std::is_base_of<ISerializable, _base_type>::value)
				object->_base_type::Serialize(node);
		}

		template<typename _object_type>
		void StartBases(_object_type* object, Type* type) {}

		template<typename _object_type>
		void StartFields(_object_type* object, Type* type) {}

		template<typename _field_type>
		struct FieldInfo
		{
			_field_type* fieldPtr;
			DataValue&   node;
			const char*  name;

			FieldInfo(_field_type* fieldPtr, DataValue& node, const char* name):fieldPtr(fieldPtr), node(node), name(name) {}

			template<typename _attribute_type>
			FieldInfo& AddAttribute(_attribute_type* attribute)
			{
				if constexpr (std::is_same<_attribute_type, SerializableAttribute>::value)
					node.AddMember(name).Set(*fieldPtr);

				return *this;
			}

			FieldInfo& SetDefaultValue(const _field_type& val) { return *this; }
		};

		template<typename _object_type, typename _field_type>
		FieldInfo<_field_type> Field(_object_type* object, Type* type, const char* name, void*(*pointerGetter)(void*), _field_type& field, ProtectSection protection)
		{
			_field_type* fieldPtr = (_field_type*)((*pointerGetter)(object));
			return FieldInfo<_field_type>(fieldPtr, node, name);
		}
	};

	// Serialization implementation macros
#define SERIALIZABLE_MAIN(CLASS)  							                                                    \
    IOBJECT_MAIN(CLASS)																							\
                                                                                                                \
    void Serialize(o2::DataValue& node) const override                                                          \
    {						                                                                                    \
		SerializationTypeProcessor p(node);                                                                     \
		ProcessBaseTypes(const_cast<thisclass*>(this), p);                                                      \
		ProcessFields(const_cast<thisclass*>(this), p);                                                         \
	}												                                                            \
    void Deserialize(const o2::DataValue& node) override                                                        \
    {												                                                            \
        DeserializeBasic(*this, node);                                                                          \
	}												                                                            \
	CLASS& operator=(const o2::DataValue& node) 		                                                        \
	{												                                                            \
		Deserialize(node); return *this; 			                                                            \
	}                                                                                                           \
	operator DataDocument() const                                                                               \
	{                																							\
		DataDocument doc;																						\
		Serialize(doc);																							\
		return doc;                                                                                             \
	}

#define SERIALIZABLE(CLASS)                                                                                     \
    SERIALIZABLE_MAIN(CLASS)                                                                                    \
		                                                                                                        \
    template<typename _type_processor> static void ProcessBaseTypes(CLASS* object, _type_processor& processor); \
    template<typename _type_processor> static void ProcessFields(CLASS* object, _type_processor& processor);    \
    template<typename _type_processor> static void ProcessMethods(CLASS* object, _type_processor& processor);   

#define SERIALIZABLE_MANUAL(CLASS) \
    SERIALIZABLE(CLASS)

#define SERIALIZABLE_ATTRIBUTE() \
    AddAttribute(new SerializableAttribute())
}

CLASS_BASES_META(o2::ISerializable)
{
	BASE_CLASS(o2::IObject);
}
END_META;
CLASS_FIELDS_META(o2::ISerializable)
{
}
END_META;
CLASS_METHODS_META(o2::ISerializable)
{

	PUBLIC_FUNCTION(void, Serialize, DataValue&);
	PUBLIC_FUNCTION(void, Deserialize, const DataValue&);
	PUBLIC_FUNCTION(String, SerializeToString);
	PUBLIC_FUNCTION(void, DeserializeFromString, const String&);
	PUBLIC_FUNCTION(void, OnSerialize, DataValue&);
	PUBLIC_FUNCTION(void, OnDeserialized, const DataValue&);
	PROTECTED_FUNCTION(void, SerializeBasic, const IObject&, DataValue&);
	PROTECTED_FUNCTION(void, DeserializeBasic, IObject&, const DataValue&);
}
END_META;
