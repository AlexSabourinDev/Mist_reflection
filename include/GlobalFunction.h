#pragma once

#include "../Mist_common/include/UtilityMacros.h"
#include "Delegate.h"
#include "MetaData.h"

MIST_NAMESPACE

// Global function maintains meta data for a global/static function with reflection data
class GlobalFunction {

public:

	// -Public API-

	template< typename ReturnType, typename... Arguments >
	ReturnType Invoke(Arguments... arguments);

	inline MetaData* GetMetaData();

	// -Structors-

	template< typename FunctionType >
	GlobalFunction(FunctionType function);

private:

	Delegate m_Function;
	MetaData m_MetaData;
};


// -Implementation-

template< typename ReturnType, typename... Arguments >
ReturnType GlobalFunction::Invoke(Arguments... arguments) {

	return m_Function.Invoke<ReturnType, Arguments...>(arguments...);
}

inline MetaData* GlobalFunction::GetMetaData() {

	return &m_MetaData;
}

// -Structors-

template< typename FunctionType >
GlobalFunction::GlobalFunction(FunctionType function) : m_Function(function) {

}

MIST_NAMESPACE_END