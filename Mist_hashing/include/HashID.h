#pragma once

#include "../../Mist_common/include/UtilityMacros.h"
#include "Hashing.h"
#include <cstdint>

MIST_NAMESPACE

// Maintains an abstraction between the maintanance of the hash value
// and the hash generation
class HashID {

public:

	// -Public API-

	inline uint32_t GetValue() const;
	
	inline bool operator==(const char* name);

	// -Structors-

	inline constexpr HashID(const char* name);

	// Creates a hash id with a directly initialized value
	inline explicit HashID(uint32_t hashValue);

private:
	uint32_t m_HashValue = 0;

};


// -Implementation-

inline uint32_t HashID::GetValue() const {
	
	return m_HashValue;
}

inline bool HashID::operator==(const char* name) {

	return m_HashValue == djb2::Hash(name);
}

inline constexpr HashID::HashID(const char* name) : m_HashValue(djb2::Hash(name)) {}

inline HashID::HashID(uint32_t hashValue) : m_HashValue(hashValue) {}


MIST_NAMESPACE_END
