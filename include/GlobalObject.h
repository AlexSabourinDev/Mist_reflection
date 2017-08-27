#pragma once

#include "../Mist_common/include/common/UtilityMacros.h"
#include "Any.h"
#include "MetaData.h"

MIST_NAMESPACE

// Maintains a reference to a global object
class GlobalObject {

public:

	// -Public API-

	template< typename ObjectType >
	ObjectType* Get();

	inline MetaData* GetMetaData();

	// -Structors-

	template< typename ObjectType >
	GlobalObject(ObjectType* globalObject);

private:

	Any m_Global;
	MetaData m_MetaData;
};



// -Implementation-


template< typename ObjectType >
ObjectType* GlobalObject::Get() {

	// Dereference the pointer because we just want the pointer that we set into the any var, not a pointer to pointer
	return *m_Global.Get<ObjectType*>();
}

inline MetaData* GlobalObject::GetMetaData() {

	return &m_MetaData;
}

// -Structors-

template< typename ObjectType >
GlobalObject::GlobalObject(ObjectType* globalObject) : m_Global(globalObject) {

}

MIST_NAMESPACE_END