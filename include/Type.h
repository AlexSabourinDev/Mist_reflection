#pragma once

#include "../../Mist_common/include/common/UtilityMacros.h"
#include <memory>
#include <utility>

MIST_NAMESPACE

namespace Detail {
	class TypeContainer;
}

// The type class is a class that holds compile time type information that can be queried
// Using the Is method, the Type class can be held in a container with other Types
class Type {

public:

	// -Public API-

	// Create the type and define it's core types
	// @Detail: Use an explicit method because a default templated constructor
	// cannot be invoked correctly as it believes its the type template list
	template< typename... CoreTypes >
	void DefineTypes();

	// Determine if this type is actually
	// the core types requested
	template< typename... CoreTypes >
	bool Is() const;

	// -Structors-

	Type() = default;
	~Type() = default;

	Type(const Type&) = delete;
	Type& operator=(const Type&) = delete;

	inline Type(Type&& move);
	inline Type& operator=(Type&& move);

private:
	
	std::unique_ptr<Detail::TypeContainer> m_Types;
};

namespace Detail {

	// Define a base class for the type data in order to
	// hold it homogenously
	class TypeContainer {
	public:
		virtual ~TypeContainer() = default;
	};

	// Define the derived class that actually holds the type data
	template< typename... Types >
	class TypeDeclarations : public TypeContainer {};
}

// -Implementation-

template< typename... CoreTypes >
bool Type::Is() const {
	// Please invoke Type::DefineTypes before calling Is
	MIST_ASSERT(m_Types != nullptr);

	Detail::TypeContainer* type = m_Types.get();
	// Try to dynamically cast it, if it doesn't return null, that means
	// it is the definition of those types
	return dynamic_cast<Detail::TypeDeclarations<CoreTypes...>*>(type) != nullptr;
}

template< typename... CoreTypes >
void Type::DefineTypes() {
	m_Types.reset(new Detail::TypeDeclarations<CoreTypes...>());
}

Type::Type(Type&& move) {
	m_Types = std::move(move.m_Types);
}

Type& Type::operator=(Type&& move) {
	m_Types = std::move(move.m_Types);
	return *this;
}

MIST_NAMESPACE_END
