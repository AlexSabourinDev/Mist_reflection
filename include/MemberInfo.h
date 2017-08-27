#pragma once

#include "../../Mist_common/include/common/UtilityMacros.h"
#include "MetaData.h"
#include "Delegate.h"

MIST_NAMESPACE

// Member info holds a reference to a member in a class allowing
// to be queried from any object
class MemberInfo {

public:

	// -Public API-

	template< typename MemberType, typename ClassType >
	MemberType* Get(ClassType* object);

	inline MetaData* GetMetaData();

	// -Types-

	template< typename ClassType, typename MemberType >
	using MemberPointer = MemberType ClassType::*;
	
	// -Structors-

	template< typename ClassType, typename MemberType >
	explicit MemberInfo(MemberPointer<ClassType, MemberType> memberPointer);

	MemberInfo(const MemberInfo&) = delete;
	MemberInfo& operator=(const MemberInfo&) = delete;

	inline MemberInfo(MemberInfo&& move);
	inline MemberInfo& operator=(MemberInfo&& move);

private:

	MetaData m_MetaData;
	Delegate m_MemberRetrieval;
};


// -Implementation-

template< typename MemberType, typename ClassType >
MemberType* MemberInfo::Get(ClassType* object) {

	MIST_ASSERT(object != nullptr);

	return m_MemberRetrieval.Invoke<MemberType*, ClassType*>(object);
}

inline MetaData* MemberInfo::GetMetaData() {

	return &m_MetaData;
}

template< typename ClassType, typename MemberType >
MemberInfo::MemberInfo(MemberInfo::MemberPointer<ClassType, MemberType> memberPointer) 
	: m_MemberRetrieval([=](ClassType* object)->MemberType*{
	
		return &(object->*memberPointer);
	}) {
}

MemberInfo::MemberInfo(MemberInfo&& move) : m_MetaData(std::move(move.m_MetaData)), m_MemberRetrieval(std::move(move.m_MemberRetrieval)) {}

MemberInfo& MemberInfo::operator=(MemberInfo&& move) {

	m_MetaData = std::move(move.m_MetaData);
	m_MemberRetrieval = std::move(move.m_MemberRetrieval);
	return *this;
}

MIST_NAMESPACE_END
