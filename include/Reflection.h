#pragma once

#include <Mist_Common/include/UtilityMacros.h>
#include "TypeInfo.h"
#include "Any.h"
#include "GlobalFunction.h"
#include "GlobalObject.h"
#include "../Mist_hashing/include/HashID.h"
#include <unordered_map>

MIST_NAMESPACE

// Holds all of the reflection data such as:
// TypeInfos, GlobalFunctions, GlobalMethods
class Reflection {

public:

	// -Public API-

	template< typename ClassType >
	TypeInfo* AddType(HashID name);

	inline TypeInfo* GetType(HashID name);


	template< typename ReturnType, typename... Arguments >
	GlobalFunction* AddGlobalFunction(HashID name, ReturnType (*function)(Arguments...));

	inline GlobalFunction* GetGlobalFunction(HashID name);


	template< typename ObjectType >
	GlobalObject* AddGlobalObject(HashID name, ObjectType* object);

	inline GlobalObject* GetGlobalObject(HashID name);

	inline std::unordered_map<uint32_t, TypeInfo>* GetTypes();
	inline std::unordered_map<uint32_t, GlobalFunction>* GetGlobalFunctions();
	inline std::unordered_map<uint32_t, GlobalObject>* GetGlobalObjects();

private:
	std::unordered_map<uint32_t, TypeInfo> m_Types;
	std::unordered_map<uint32_t, GlobalFunction> m_GlobalFunctions;
	std::unordered_map<uint32_t, GlobalObject> m_GlobalObjects;
};

void Merge(Reflection&& left, Reflection* output);


// -Implementation-

template< typename ClassType >
TypeInfo* Reflection::AddType(HashID name) {

	Type classType;
	classType.DefineTypes<ClassType>();

	TypeInfo type(std::move(classType));

	auto result = m_Types.emplace(name.GetValue(), std::move(type));

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline TypeInfo* Reflection::GetType(HashID name) {

	return &m_Types.at(name.GetValue());
}


template< typename ReturnType, typename... Arguments >
GlobalFunction* Reflection::AddGlobalFunction(HashID name, ReturnType(*function)(Arguments...)) {

	auto result = m_GlobalFunctions.emplace(name.GetValue(), function);

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline GlobalFunction* Reflection::GetGlobalFunction(HashID name) {

	return &m_GlobalFunctions.at(name.GetValue());
}


template< typename ObjectType >
GlobalObject* Reflection::AddGlobalObject(HashID name, ObjectType* object) {

	auto result = m_GlobalObjects.emplace(name.GetValue(), GlobalObject(object));

	MIST_ASSERT(result.second);

	return &result.first->second;
}

inline GlobalObject* Reflection::GetGlobalObject(HashID name) {

	return &m_GlobalObjects.at(name.GetValue());
}

inline std::unordered_map<uint32_t, TypeInfo>* Reflection::GetTypes() {

	return &m_Types;
}

inline std::unordered_map<uint32_t, GlobalFunction>* Reflection::GetGlobalFunctions() {

	return &m_GlobalFunctions;
}

inline std::unordered_map<uint32_t, GlobalObject>* Reflection::GetGlobalObjects() {

	return &m_GlobalObjects;
}

MIST_NAMESPACE_END
