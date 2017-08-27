#pragma once

#include "../Mist_common/include/common/UtilityMacros.h"
#include "../utility/HashID.h"
#include "Any.h"
#include <unordered_map>


MIST_NAMESPACE

// The meta data class will hold all of the meta data associated to a particular
// info in the reflection table.
// @Detail: When requesting a type, the type must match the insterted type exactly or else an assert will be hit.
class MetaData {

public:

	// -Public API-
	
	// Add data to the meta data at the location of id
	template< typename Type >
	Type* Add(HashID id, Type data);

	// Retrieve data from the meta data at the location of id
	template< typename Type >
	Type* Get(HashID id);
	
	// Determine if the meta data has data at the location of id
	inline bool Has(HashID id) const;

	// -Types-
	using Iterator = std::unordered_map<uint32_t, Any>::iterator;

	// -Iterators-

	// User lowercase in order to be able to use range based loop
	inline Iterator begin();
	inline Iterator end();

	MetaData() = default;

	MetaData(const MetaData&) = delete;
	MetaData& operator=(const MetaData&) = delete;

	inline MetaData(MetaData&& move);
	inline MetaData& operator=(MetaData&& move);

private:

	std::unordered_map<uint32_t, Any> m_Data;

};


// -Implementation-

template< typename Type >
Type* MetaData::Add(HashID id, Type data) {

	// If this is hit, there is already an item at this location. There has either been a collision
	// with the hashing method or you're repeating a name.
	MIST_ASSERT(Has(id) == false);
	auto insertionResult = m_Data.emplace(id.GetValue(), Any(data));

	// The insertion must have worked, if it didn't check the id and try again
	MIST_ASSERT(insertionResult.second == true);
	MIST_ASSERT(m_Data[id.GetValue()].Is<Type>());

	return Cast<Type>(m_Data[id.GetValue()]);
}

// Retrieve data from the meta data at the location of id
template< typename Type >
Type* MetaData::Get(HashID id) {

	// If this is hit, there is not an item at this location. Call MetaData::Has in order to determine if the
	// id is present.
	MIST_ASSERT(Has(id));

	Any& any = m_Data[id.GetValue()];

	// The retrieved type must match the other
	MIST_ASSERT(any.Is<Type>());
	return Cast<Type>(m_Data[id.GetValue()]);
}

inline bool MetaData::Has(HashID id) const {

	return m_Data.find(id.GetValue()) != m_Data.cend();
}

inline MetaData::Iterator MetaData::begin() {
	
	return m_Data.begin();
}

inline MetaData::Iterator MetaData::end() {

	return m_Data.end();
}

MetaData::MetaData(MetaData&& move) : m_Data(std::move(move.m_Data)) {

}
MetaData& MetaData::operator=(MetaData&& move) {

	m_Data = std::move(move.m_Data);
	return *this;
}

MIST_NAMESPACE_END
