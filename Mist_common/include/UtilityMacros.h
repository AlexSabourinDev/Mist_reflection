#pragma once

#include "ConfigMacros.h"


// Define a utility macro to define multiple optional namespaces.
// @Details: This decision was made in order to support nesting namespaces 
//			in later files and modules
#if MIST_USE_NAMESPACES

#define OPTIONAL_NAMESPACE(namespaceName) \
namespace namespaceName {

#define NAMESPACE_END }

#else

#define OPTIONAL_NAMESPACE(namespaceName)
#define NAMESPACE_END

#endif

// Define some shortcut namespace macros for quick namespace addition
#define MIST_NAMESPACE OPTIONAL_NAMESPACE(MIST_NAMESPACE_NAME)
#define MIST_NAMESPACE_END NAMESPACE_END

// Define the assertion method for assertions
#include <cassert>
#define MIST_ASSERT(condition) assert(condition)
#define MIST_ASSERT_MSG(condition, msg) assert(condition && msg)