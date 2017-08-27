# Reflection 
The Reflection project is a collection of reflection utilities for C++. The goals of the library were to be able to have a simple utility to provide reflection capabilities for C++ based on the rttr library and C# reflection tools. The library is simple, with few, easy to understand and modify files.

### Example

To begin, a reflection object must be created in the project. For this example, we'll be making a global.

```c++
Mist::Reflection g_Reflection;
```

reflection can be simply added to pre-built classes or new classes.
Take the class ExampleFoo

```c++
class ExampleFoo {

public:

	ExampleFoo(size_t* value) : m_Target(value) {}

	void ChangeTarget(size_t value) {
		*m_Target = value;
	}

	size_t* m_Target = nullptr;
};
```

In order to define the reflection for the ExampleFoo, we create a PreMainCall global object. The constructor of this object will assure that the method passed in get's invoked before the program entry. We then add our type information to the global reflection object.
Note: We place it in the anonymous namespace to avoid name collisions.

```c++
namespace {

	Mist::PreMainCall ExampleFooRegister([]() {

		Mist::TypeInfo* type = g_Reflection.AddType<ExampleFoo>("ExampleFoo");

		type->AddConstructor<ExampleFoo, size_t*>();

		Mist::MethodInfo* methodInfo = type->AddMethod("ChangeTarget", &ExampleFoo::ChangeTarget);

		type->AddMember("m_Target", &ExampleFoo::m_Target);
	});
}
```

To read the reflection data, you query the reflection object for the information.
```c++
int main() {

	Mist::TypeInfo* type = g_Reflection.GetType("ExampleFoo");

	size_t result = 0;
	const size_t SOME_VALUE = 10;

	ExampleFoo* exampleFoo = type->Create<ExampleFoo>(&result);
	// The template parameters for the invoke method must match the method signature exactly
	type->GetMethodInfo("ChangeTarget")->Invoke<void>(exampleFoo, SOME_VALUE);

	assert(result == SOME_VALUE);

	return 0;
}
```

Additionaly, you can add MetaData to reflection data by retrieving the MetaData object.
```c++
methodInfo->GetMetaData()->Add("ExampleAttribute", ExampleAttribute());
```

### Dependancies
The reflection project depends on Mist_common and Mist_hashing

# Contact
For questions, comments or general conversation reach me at AlexSabourinDev@gmail.com
