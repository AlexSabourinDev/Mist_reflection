#pragma once

#include "PreMain.h"
#include <utility>

MIST_NAMESPACE

// PreMainCall is used to invoke a method before main is
// invoked by creating a global object and using the constructor
// to call the method
class PreMainCall {

public:
	template< typename CallType >
	PreMainCall(CallType call);
};


// -Implementation-

template< typename CallType >
PreMainCall::PreMainCall(CallType call) {
	
	call();
}

MIST_NAMESPACE_END