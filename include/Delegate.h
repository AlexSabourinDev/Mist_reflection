#pragma once

#include "../Mist_common/include/common/UtilityMacros.h"
#include "Type.h"
#include <tuple>
#include <memory>
#include <utility>

MIST_NAMESPACE

namespace Detail {
	class Callback;
}

// The delegate class is a generic wrapper around
// a method of any signature. The method is then called using the invoke method.
// The delegate class handles lambdas and global methods in a homogeneous interface.
// @Detail: The signature of the invoke method must match the signature of the inner method.
class Delegate {

public:

	// -Public API-

	template< typename ReturnType = void, typename... Arguments >
	ReturnType Invoke(Arguments... arguments);

	template< typename ReturnType = void, typename... Arguments >
	bool HasSignature() const;

	// -Structors-

	template< typename CallbackType >
	Delegate(CallbackType callback);
	~Delegate() = default;

	Delegate(const Delegate&) = delete;
	Delegate& operator=(const Delegate&) = delete;

	inline Delegate(Delegate&& move);
	inline Delegate& operator=(Delegate&& move);

private:

	std::unique_ptr<Detail::Callback> m_Callback;
};


namespace Detail {

	// @Detail: The series of callback, callbackInterface and callbackdefinition is a three tierd method to
	// provide the correct call to a method that could be a lambda or a global function

	class Callback {

	public:

		// -Structors-

		virtual ~Callback() = default;
	};

	// The interface is the lowest the user will go in order to 
	// call the method
	template< typename ReturnType, typename... ArgumentTypes >
	class CallbackInterface : public Callback {

	public:
		// -Public API-

		virtual ReturnType Invoke(ArgumentTypes... Arguments) = 0;

		// -Structors-
		virtual ~CallbackInterface() override = default;
	};

	template < typename CallbackType, typename ReturnType, typename... ArgumentTypes>
	class CallbackDefinition : public CallbackInterface< ReturnType, ArgumentTypes... > {

	public:
		ReturnType Invoke(ArgumentTypes... arguments) override;

		// -Structors-
		explicit CallbackDefinition(CallbackType callback);

	private:
		CallbackType m_Callback;
	};

	// Construct a callback from the passed in object method
	template< typename CallbackType, typename ReturnType, typename... Arguments >
	Callback* MakeCallback(CallbackType&& callback, ReturnType(CallbackType::*callbackMethod)(Arguments...));

	// Construct a callback from the passed in object method
	template< typename CallbackType, typename ReturnType, typename... Arguments >
	Callback* MakeCallback(CallbackType&& callback, ReturnType(CallbackType::*callbackMethod)(Arguments...) const);

	// Construct a callback from the passed in object
	template< typename CallbackType,
		// @Template Condition: Require that the callback type has a () operator for this call
		typename TemplateCondition = decltype(&CallbackType::operator()) >
		Callback* MakeCallback(CallbackType callback);

	// Construct a callback from the passed in global function
	template< typename ReturnType, typename... Arguments >
	Callback* MakeCallback(ReturnType(*callback)(Arguments...));

	// Determine if a callback has the correct definition
	template< typename ReturnType, typename... Arguments >
	bool HasDefinition(Callback* callback);

	// Cast the callback to the correct callback interface
	template< typename ReturnType, typename... Arguments >
	CallbackInterface<ReturnType, Arguments...>* Cast(Callback* callback);
}


// -Implementation-


// -Delegate-

// Invoke the inner callback
template< typename ReturnType, typename... Arguments >
ReturnType Delegate::Invoke(Arguments... arguments) {

	// The object does not implement the correct method signature, call it with the correct types
	MIST_ASSERT((Detail::HasDefinition<ReturnType, Arguments...>(m_Callback.get())));

	Detail::CallbackInterface<ReturnType, Arguments...>* interface = Detail::Cast<ReturnType, Arguments...>(m_Callback.get());
	return interface->Invoke(arguments...);
}

template< typename ReturnType, typename... Arguments >
bool Delegate::HasSignature() const {

	return Detail::HasDefinition<ReturnType, Arguments...>(m_Callback.get());
}


template< typename CallbackType >
Delegate::Delegate(CallbackType callback) : m_Callback(Detail::MakeCallback(callback)) {
}

Delegate::Delegate(Delegate&& move) {
	m_Callback = std::move(move.m_Callback);
}

Delegate& Delegate::operator=(Delegate&& move) {
	m_Callback = std::move(move.m_Callback);
	return *this;
}


namespace Detail {

	template < typename CallbackType, typename ReturnType, typename... ArgumentTypes>
	ReturnType CallbackDefinition<CallbackType, ReturnType, ArgumentTypes...>::Invoke(ArgumentTypes... arguments) {

		return m_Callback(arguments...);
	}

	template < typename CallbackType, typename ReturnType, typename... ArgumentTypes>
	CallbackDefinition<CallbackType, ReturnType, ArgumentTypes...>::CallbackDefinition(CallbackType callback) 
		: m_Callback(callback) {
	}

	template< typename CallbackType,
		// @Template Condition: Require that the callback type has a () operator for this call
		typename TemplateCondition >
	Callback* MakeCallback(CallbackType callback) {

		return MakeCallback(std::forward<CallbackType>(callback), &CallbackType::operator());
	}
	
	template< typename CallbackType, typename ReturnType, typename... Arguments >
	Callback* MakeCallback(CallbackType&& callback, ReturnType(CallbackType::*)(Arguments...)) {

		return new CallbackDefinition<CallbackType, ReturnType, Arguments...>(std::forward<CallbackType>(callback));
	}

	template< typename CallbackType, typename ReturnType, typename... Arguments >
	Callback* MakeCallback(CallbackType&& callback, ReturnType(CallbackType::*)(Arguments...) const) {

		return new CallbackDefinition<CallbackType, ReturnType, Arguments...>(std::forward<CallbackType>(callback));
	}

	template< typename ReturnType, typename... Arguments >
	Callback* MakeCallback(ReturnType(*callback)(Arguments...)) {
		
		MIST_ASSERT(callback != nullptr);
		return new CallbackDefinition<ReturnType(*)(Arguments...), ReturnType, Arguments...>(callback);
	}

	// Determine if a callback has the correct definition
	template< typename ReturnType, typename... Arguments >
	bool HasDefinition(Callback* callback) {

		MIST_ASSERT(callback != nullptr);
		return dynamic_cast<CallbackInterface<ReturnType, Arguments...>*>(callback) != nullptr;
	}

	// Cast the callback to the correct callback interface
	template< typename ReturnType, typename... Arguments >
	CallbackInterface<ReturnType, Arguments...>* Cast(Callback* callback) {

		MIST_ASSERT(callback != nullptr);
		return dynamic_cast<CallbackInterface<ReturnType, Arguments...>*>(callback);
	}

}

MIST_NAMESPACE_END
