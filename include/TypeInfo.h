#pragma once

#include "../Mist_common/include/common/UtilityMacros.h"
#include "../utility/HashID.h"
#include "MemberInfo.h"
#include "MethodInfo.h"
#include "MetaData.h"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <utility>

MIST_NAMESPACE

// TypeInfo is the core reflection class associated to the
// types, this class holds all of the member and method infos
class TypeInfo {

public:
	
	// -Public API-

	// -Constructors-
	
	template< typename Type, typename... Arguments>
	void AddConstructor();

	template< typename ConstructionType, typename... Arguments >
	ConstructionType* Create(Arguments... args);

	template< typename ConstructionType, typename... Arguments >
	bool TryCreate(ConstructionType** instance, Arguments... args);

	template< typename ClassType, typename... Arguments >
	bool HasConstructor();

	// -MemberInfo-

	template< typename ClassType, typename MemberType >
	MemberInfo* AddMember(HashID key, MemberInfo::MemberPointer<ClassType, MemberType> member);

	inline MemberInfo* GetMemberInfo(HashID key);

	inline bool HasMember(HashID key);

	// -MethodInfo-

	template< typename ClassType, typename ReturnType, typename... Arguments >
	MethodInfo* AddMethod(HashID key, MethodInfo::MethodPointer<ClassType, ReturnType, Arguments...> method);

	inline MethodInfo* GetMethodInfo(HashID key);

	inline bool HasMethod(HashID key);


	inline MetaData* GetMetaData();


	inline std::unordered_map<uint32_t, MethodInfo>* GetMethods();
	inline std::unordered_map<uint32_t, MemberInfo>* GetMembers();

	// -Structors-

	// Pass in the defined type because constructors can't have default
	// constructed template constructors
	explicit inline TypeInfo(Type type);

private:

	Type m_Type;

	std::vector<Delegate> m_Constructors;
	std::unordered_map<uint32_t, MethodInfo> m_Methods;
	std::unordered_map<uint32_t, MemberInfo> m_Members;
	
	MetaData m_MetaData;
};


// -Implementation-

template< typename Type, typename... Arguments >
void TypeInfo::AddConstructor() {

	m_Constructors.push_back(Delegate([](Arguments... args)->Type* { return new Type(args...); }));
}

template< typename ConstructionType, typename... Arguments >
ConstructionType* TypeInfo::Create(Arguments... arguments) {

	MIST_ASSERT(m_Constructors.empty() == false);
	MIST_ASSERT(m_Type.Is<ConstructionType>());

	for (auto& constructor : m_Constructors) {
		if (constructor.HasSignature<ConstructionType*, Arguments...>()) {
			return constructor.Invoke<ConstructionType*, Arguments...>(arguments...);
		}
	}
	return nullptr;
}

template< typename ConstructionType, typename... Arguments >
bool TypeInfo::TryCreate(ConstructionType** instance, Arguments... args) {

	*instance = Create<ConstructionType, Arguments...>(args...);
	return *instance != nullptr;
}

template< typename ClassType, typename... Arguments >
bool TypeInfo::HasConstructor() {

	for (const auto& constructor : m_Constructors) {
		if (constructor.HasSignature<ClassType*, Arguments...>()) {
			return true;
		}
	}
	return false;
}

template< typename ClassType, typename MemberType >
MemberInfo* TypeInfo::AddMember(HashID key, MemberInfo::MemberPointer<ClassType, MemberType> member) {
	
	// This member pointer must match our class type
	MIST_ASSERT(m_Type.Is<ClassType>());

	MIST_ASSERT(HasMember(key) == false);
	auto result = m_Members.emplace(key.GetValue(), MemberInfo(member));

	// Assure that the insertion took place
	MIST_ASSERT(result.second);
	return &result.first->second;
}

inline MemberInfo* TypeInfo::GetMemberInfo(HashID key) {

	MIST_ASSERT(HasMember(key));
	return &m_Members.at(key.GetValue());
}

inline bool TypeInfo::HasMember(HashID key) {

	return m_Members.find(key.GetValue()) != m_Members.end();
}

template< typename ClassType, typename ReturnType, typename... Arguments >
MethodInfo* TypeInfo::AddMethod(HashID key, MethodInfo::MethodPointer<ClassType, ReturnType, Arguments...> method) {

	// This type info must match the new methods type
	MIST_ASSERT(m_Type.Is<ClassType>());

	MIST_ASSERT(HasMethod(key) == false);
	auto result = m_Methods.emplace(key.GetValue(), MethodInfo(method));

	// Assure that the insertion took place
	MIST_ASSERT(result.second);
	return &result.first->second;
}

inline MethodInfo* TypeInfo::GetMethodInfo(HashID key) {

	MIST_ASSERT(HasMethod(key));
	return &m_Methods.at(key.GetValue());
}

inline bool TypeInfo::HasMethod(HashID key) {

	return m_Methods.find(key.GetValue()) != m_Methods.end();
}


inline MetaData* TypeInfo::GetMetaData() {

	return &m_MetaData;
}

inline std::unordered_map<uint32_t, MethodInfo>* TypeInfo::GetMethods() {

	return &m_Methods;
}

inline std::unordered_map<uint32_t, MemberInfo>* TypeInfo::GetMembers() {

	return &m_Members;
}

// Pass in the defined type because constructors can't have default
// constructed template constructors
inline TypeInfo::TypeInfo(Type type) : m_Type(std::move(type)) {}


MIST_NAMESPACE_END
