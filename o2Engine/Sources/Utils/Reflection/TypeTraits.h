#pragma once

#include "Utils/IObject.h"
#include "Utils/UID.h"

namespace o2
{
	template<class T> struct IsVectorHelper: std::false_type {};
	template<class T> struct IsVectorHelper<Vector<T>>: std::true_type {};
	template<class T> struct IsVector: IsVectorHelper<typename std::remove_cv<T>::type> {};
	template<class T> struct VectorElementTypeGetter { typedef T type; };
	template<class T> struct VectorElementTypeGetter<Vector<T>> { typedef T type; };

	template<class T> struct IsPropertyHelper: std::false_type {};
	template<class T> struct IsPropertyHelper<Property<T>>: std::true_type {};
	template<class T> struct IsProperty: IsPropertyHelper<typename std::remove_cv<T>::type> {};
	template<class T> struct PropertyTypeGetter { typedef T type; };
	template<class T> struct PropertyTypeGetter<Property<T>> { typedef T type; };

	template<class T, class T2> struct IsDictionaryHelper: std::false_type {};
	template<class T, class T2> struct IsDictionaryHelper<Dictionary<T, T2>, void>: std::true_type {};
	template<class T> struct IsDictionary: IsDictionaryHelper<typename std::remove_cv<T>::type, void> {};

	template<class T, class T2> struct DictionaryKeyTypeGetterHelper { typedef T type; };
	template<class T, class T2> struct DictionaryKeyTypeGetterHelper<Dictionary<T, T2>, void> { typedef T type; };
	template<class T> struct DictionaryKeyTypeGetter: DictionaryKeyTypeGetterHelper<typename std::remove_cv<T>::type, void> {};

	template<class T, class T2> struct DictionaryValueTypeGetterHelper { typedef T2 type; };
	template<class T, class T2> struct DictionaryValueTypeGetterHelper<Dictionary<T, T2>, void> { typedef T2 type; };
	template<class T> struct DictionaryValueTypeGetter: DictionaryValueTypeGetterHelper<typename std::remove_cv<T>::type, void> {};

	template<class T> struct IsStringAccessorHelper: std::false_type {};
	template<class T> struct IsStringAccessorHelper<Accessor<T*, const String&>>: std::true_type {};
	template<class T> struct IsStringAccessor: IsStringAccessorHelper<typename std::remove_cv<T>::type> {};
	template<class T> struct StringAccessorTypeGetter { typedef T type; };
	template<class T> struct StringAccessorTypeGetter<Accessor<T*, const String&>> { typedef T type; };

    class Reflection;
    
    template<typename _type>
    class FundamentalType;
}

#include "Utils/Reflection/Type.h"
#include "Utils/Data/DataNode.h"
#include "Utils/Reflection/Reflection.h"

namespace o2
{
	// type trait
	template<typename T, typename X =
	/* if */   typename std::conditional<std::is_base_of<IObject, T>::value,
	/* then */ T,
	/* else */ std::conditional<(
	           /* if */   std::is_fundamental<T>::value ||
		                  std::is_same<T, Basis>::value ||
		                  std::is_same<T, Color4>::value ||
		                  std::is_same<T, RectI>::value ||
		                  std::is_same<T, RectF>::value ||
		                  std::is_same<T, Vec2I>::value ||
		                  std::is_same<T, Vec2F>::value ||
		                  std::is_same<T, Vertex2>::value ||
		                  std::is_same<T, String>::value ||
		                  std::is_same<T, WString>::value ||
		                  std::is_same<T, UID>::value ||
		                  std::is_same<T, DataNode>::value) && !std::is_const<T>::value,
		       /* then */ FundamentalTypeContainer<T>,
		       /* else */ std::conditional<
		                  /* if */   std::is_enum<T>::value,
		                  /* then */ EnumTypeContainer<T>,
		                  /* else */ Type::Dummy
			              >::typex
	           >::type
	>::type>
	struct GetTypeHelper
	{
		typedef X type;
	};
    
    template<typename T>
    struct RegularTypeGetter
    {
        static const Type& GetType();
    };
    
    template<typename T>
    struct PointerTypeGetter
    {
        static const Type& GetType();
    };
    
    template<typename T>
    struct VectorTypeGetter
    {
        static const Type& GetType();
    };
    
    template<typename T>
    struct DictionaryTypeGetter
    {
        static const Type& GetType();
    };
    
    template<typename T>
    struct AccessorTypeGetter
    {
        static const Type& GetType();
    };


    template<typename _type, typename _getter =
    typename std::conditional<
    /* if */   std::is_pointer<_type>::value,
    /* then */ PointerTypeGetter<_type>,
    /* else */ typename std::conditional<
    /* if */   IsVector<_type>::value,
    /* then */ VectorTypeGetter<_type>,
    /* else */ typename std::conditional<
    /* if */   IsStringAccessor<_type>::value,
    /* then */ AccessorTypeGetter<_type>,
    /* else */ typename std::conditional<
    /* if */   IsDictionary<_type>::value,
    /* then */ DictionaryTypeGetter<_type>,
    /* else */ RegularTypeGetter<_type>
    >::type
    >::type
    >::type
    >::type
    >
	const Type& GetTypeOf();

	template<typename T>
	const Type& RegularTypeGetter<T>::GetType() { return *GetTypeHelper<T>::type::type; }
    
	template<typename T>
	const Type& PointerTypeGetter<T>::GetType() { return *GetTypeOf<typename std::remove_pointer<T>::type>().GetPointerType(); }
    
	template<typename T>
	const Type& VectorTypeGetter<T>::GetType() { return *Reflection::InitializeVectorType<typename VectorElementTypeGetter<T>::type>(); }
    
	template<typename T>
	const Type& DictionaryTypeGetter<T>::GetType() {
        return *Reflection::InitializeDictionaryType<typename DictionaryKeyTypeGetter<T>::type, typename DictionaryValueTypeGetter<T>::type>();
    }

	template<typename T>
	const Type& AccessorTypeGetter<T>::GetType() { return *Reflection::InitializeAccessorType<StringAccessorTypeGetter<T>::type>(); }
    
	// Returns type of template parameter
	template<typename _type, typename _getter>
	const Type& GetTypeOf()
	{
		return _getter::GetType();
	}
}
