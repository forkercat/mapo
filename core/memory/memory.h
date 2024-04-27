//
// Created by Junhao Wang (@forkercat) on 4/26/24.
//

#pragma once

#include "core/typedefs.h"

#include <new>

// [USAGE] Test* test = MP_NEW(Test, allocator)(0, 1, 2);
#define MP_NEW(Type, Allocator) new (Allocator.Allocate(sizeof(Type))) Type

// No placement-form of the delete operator.
template <typename T, class A>
void DeleteHelper(T* pObject, A& allocator)
{
	// Call the destructor first and then free the associated memory.
	pObject->~T();
	allocator.Free(pObject);
}

// [USAGE] MP_DELETE(test, allocator);
#define MP_DELETE(Ptr, Allocator) DeleteHelper(Ptr, Allocator)

/////////////////////////////////////////////////////////////////////////////////
// Traits-class that decides whether a given type is POD or not.
/////////////////////////////////////////////////////////////////////////////////

// POD (plain old data) like fundamental types, struct, and class that is without a constructor.
// If it is a POD type, we don't need to explicitly call the constructor/destructor.
// Therefore, we don't need to allocate extra bytes to hold the bytes for value N.

// Option 1: You need to define for the POD types.
/*
template <typename T>
struct IsPOD
{
	static const bool Value = false;
};

template <>
struct IsPOD<char>
{
	static const bool Value = true;
};
*/

// Option 2: Instead, let's use std::is_pod().

template <typename T>
struct IsPOD
{
	static const bool Value = std::is_pod<T>::value;
};

// In addition, since function overloads are based on types, not values, we cannot
// add overloads for false and true of what IsPOD<T>::Value yields. We can then
// use type-based dispatching. Basically, making two new types for false and true.
template <bool I>
struct IntToType
{
};

using NonPODType = IntToType<false>;
using PODType = IntToType<true>;

/////////////////////////////////////////////////////////////////////////////////
// For array data
/////////////////////////////////////////////////////////////////////////////////

template <typename T, typename A>
T* NewArray(A& allocator, size_t numBytes, NonPODType)
{
	union
	{
		void* pAsVoid;
		size_t* pAsSizeType;
		T* pAsT;
	};
	pAsVoid = allocator.Allocate(sizeof(T) * numBytes + sizeof(size_t));

	// Stores number of instances in the first size_t bytes.
	*pAsSizeType++ = numBytes;

	// Constructs instances.
	const T* const pLast = pAsT + numBytes;
	while (pAsT < pLast)
	{
		new (pAsT++) T; // use placement new
	}

	return (pAsT - numBytes); // return the first instance, not the base
}

template <typename T, typename A>
T* NewArray(A& allocator, size_t numBytes, PODType)
{
	// No extra bytes to hold N for POD (plain old data without a constructor).
	return static_cast<T*>(allocator.Allocate(sizeof(T) * numBytes));
}

template <typename T>
struct TypeAndCount
{
	// Used to deduce type and count from T[N].
};

template <typename T, size_t N>
struct TypeAndCount<T[N]> // partial template specialization to extract type and count
{
	using ExtractedType = T;
	static const size_t ExtractedCount = N;
};

// [USAGE] Test* test = MP_NEW_ARRAY(Test[3], allocator);
#define MP_NEW_ARRAY(Type, Allocator)                                                          \
	NewArray<TypeAndCount<Type>::ExtractedType>(Allocator, TypeAndCount<Type>::ExtractedCount, \
		IntToType<IsPOD<TypeAndCount<Type>::ExtractedType>::Value>())

template <typename T, typename A>
void DeleteArray(T* pObject, A& allocator, NonPODType)
{
	union
	{
		size_t* pAsSizeType;
		T* pAsT;
	};

	pAsT = pObject;
	// Go back size_t bytes and get the N value.
	const size_t N = pAsSizeType[-1];

	for (size_t i = N; i > 0; --i)
	{
		pAsT[i - 1].~T(); // call destructor in reverse order
	}

	allocator.Free(pAsSizeType - 1);
}

template <typename T, typename A>
void DeleteArray(T* pObject, A& allocator, PODType)
{
	allocator.Free(pObject);
}

// The reason why we need an extra helper is we cannot deduct type from the "Ptr"
// in the macro since it is a value. We need to use a helper function by relying on
// the compiler's template argument deduction to extract the type, then forward to the
// right overload version.
template <typename T, typename A>
void DeleteArray(T* pObject, A& allocator)
{
	DeleteArray(pObject, allocator, IntToType<IsPOD<T>::Value>());
}

// [USAGE] MP_DELETE_ARRAY(test, allocator);
#define MP_DELETE_ARRAY(Ptr, Allocator) DeleteArray(Ptr, Allocator)
