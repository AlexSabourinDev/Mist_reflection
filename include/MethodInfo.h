#pragma once

#include "../Mist_common/include/UtilityMacros.h"
#include "MetaData.h"
#include "Delegate.h"

MIST_NAMESPACE

// The method info class holds relfection data about a method for a class
class MethodInfo {

public:

	// -Public API-

	// Invoke the method on the object of type ClassType
	// The method signature must match the passed in method's exactly
	template< typename ReturnType, typename ClassType, typename... Arguments >
	ReturnType Invoke(ClassType* object, Arguments... arguments);


	inline MetaData* GetMetaData();

	// -Types-

	template< typename ClassType, typename ReturnType, typename... Arguments >
	using MethodPointer = ReturnType(ClassType::*) (Arguments...);

	// -Structors-

	template< typename ClassType, typename ReturnType, typename... Arguments >
	explicit MethodInfo(MethodPointer<ClassType, ReturnType, Arguments...> method);

	MethodInfo(const MethodInfo&) = delete;
	MethodInfo& operator=(const MethodInfo&) = delete;

	inline MethodInfo(MethodInfo&& move);
	inline MethodInfo& operator=(MethodInfo&& move);

private:

	MetaData m_MetaData;
	Delegate m_Method;
};


// -Implementation-

template< typename ReturnType, typename ClassType, typename... Arguments >
ReturnType MethodInfo::Invoke(ClassType* object, Arguments... arguments) {

	MIST_ASSERT(object != nullptr);

	return m_Method.Invoke<ReturnType, ClassType*, Arguments...>(object, arguments...);
}


inline MetaData* MethodInfo::GetMetaData() {

	return &m_MetaData;
}

template< typename ClassType, typename ReturnType, typename... Arguments >
MethodInfo::MethodInfo(MethodInfo::MethodPointer<ClassType, ReturnType, Arguments...> method) 
	: m_Method([=](ClassType* object, Arguments... arguments)->ReturnType {

		return (object->*method)(arguments...);
	}) {
}

inline MethodInfo::MethodInfo(MethodInfo&& move) : m_MetaData(std::move(move.m_MetaData)), m_Method(std::move(move.m_Method)) {}

inline MethodInfo& MethodInfo::operator=(MethodInfo&& move) {

	m_MetaData = std::move(move.m_MetaData);
	m_Method = std::move(move.m_Method);
	return *this;

}

MIST_NAMESPACE_END
