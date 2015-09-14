#pragma once

#include "Utils/CommonTypes.h"
#include "Utils/Containers/Vector.h"
#include "Utils/Reflection/Attribute.h"
#include "Utils/String.h"

namespace o2
{
	// -----------------------
	// Class field information
	// -----------------------
	class FieldInfo
	{
		friend class Type;

		String              mName;       // Name of field
		UInt                mOffset;     // Offset of field in bytes from owner address
		bool                mIsProperty; // Is it property or field
		Vector<IAttribute*> mAttributes; // Attributes array

	public:
		// Default constructor
		FieldInfo();

		// Constructor
		FieldInfo(const String& name, UInt offset, bool isProperty);

		// Equal operator
		bool operator==(const FieldInfo& other) const;

		// Adds attribute by type and parameters
		template<typename _attr_type, typename ... _args>
		FieldInfo& AddAttribute(_args ... args);

		// Returns name of field
		const String& Name() const;

		// Returns is it property or not
		bool IsProperty() const;

		// Returns value of field in specified object
		template<typename _type>
		_type GetValue(void* object) const;

		// Sets value of field in specified object
		template<typename _type>
		void SetValue(void* object, _type value) const;

		// Returns attribute of field, if it is not exist - returns nullptr
		template<typename _attr_type>
		_attr_type* Attribute() const;

		// Returns true if exist attribute with cpecified type
		template<typename _attr_type>
		bool HaveAttribute() const;

		// Returns attributes array
		const Vector<IAttribute*>& Attributes() const;
	};

	template<typename _attr_type>
	bool o2::FieldInfo::HaveAttribute() const
	{
		for (auto attr : mAttributes)
		{
			if (dynamic_cast<_attr_type*>(attr))
				return true;
		}

		return false;
	}

	template<typename _attr_type>
	_attr_type* o2::FieldInfo::Attribute() const
	{
		for (auto attr : mAttributes)
		{
			_attr_type* res = dynamic_cast<_attr_type*>(attr);
			if (res)
				return res;
		}

		return nullptr;
	}

	template<typename _type>
	void o2::FieldInfo::SetValue(void* object, _type value) const
	{
		*(_type*)((char*)object + mOffset) = value;
	}

	template<typename _type>
	_type o2::FieldInfo::GetValue(void* object) const
	{
		return *(_type*)((char*)object + mOffset);
	}

	template<typename _attr_type, typename ... _args>
	FieldInfo& o2::FieldInfo::AddAttribute(_args ... args)
	{
		_attr_type* attribute = new _attr_type(args ...);
		attribute->mOwnerFieldInfo = this;
		mAttributes.Add(attribute);
		return *this;
	}

}