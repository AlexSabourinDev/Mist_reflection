#include <iostream>
#include "../include/Type.h"
#include "../include/Delegate.h"
#include "../include/MetaData.h"
#include "../include/MethodInfo.h"
#include "../include/MemberInfo.h"
#include "../include/TypeInfo.h"
#include "../include/Reflection.h"

size_t g_SomeGlobal = 10;

void Pause() {
	std::cout << "Press Any Key" << std::endl;
	char c;
	std::cin >> c;
}

size_t ReturnNumber(size_t num) {
	return num;
}

void TestReflection() {

	std::cout << "Reflection Test" << std::endl;

	// -Type-

	Mist::Type type;
	type.DefineTypes<int>();

	MIST_ASSERT(type.Is<int>());
	MIST_ASSERT(type.Is<float>() == false);

	type.DefineTypes<float>();

	MIST_ASSERT(type.Is<int>() == false);
	MIST_ASSERT(type.Is<float>());

	std::unique_ptr<Mist::Detail::Callback> callback(Mist::Detail::MakeCallback(Pause));
	MIST_ASSERT(Mist::Detail::HasDefinition<void>(callback.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(callback.get())) == false);

	// -Callback-

	const size_t CHANGE_TARGET = 1;
	size_t changed;
	std::unique_ptr<Mist::Detail::Callback> lambda(Mist::Detail::MakeCallback([=,&changed]() { changed = CHANGE_TARGET; }));

	MIST_ASSERT(Mist::Detail::HasDefinition<void>(lambda.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(lambda.get())) == false);

	Mist::Detail::CallbackInterface<void>* call = Mist::Detail::Cast<void>(lambda.get());
	call->Invoke();

	MIST_ASSERT(changed == CHANGE_TARGET);

	changed = 0;
	auto l = [&changed](size_t j) { changed = j; };
	std::unique_ptr<Mist::Detail::Callback> savedLambda(Mist::Detail::MakeCallback(l));

	MIST_ASSERT((Mist::Detail::HasDefinition<void, size_t>(savedLambda.get())));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(savedLambda.get())) == false);

	Mist::Detail::CallbackInterface<void, size_t>* c = Mist::Detail::Cast<void, size_t>(savedLambda.get());
	c->Invoke(CHANGE_TARGET);

	MIST_ASSERT(changed == CHANGE_TARGET);
	
	// -Delegate-

	size_t data = 0;
	Mist::Delegate del([&]() { data = CHANGE_TARGET; });
	del.Invoke();

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del2([](size_t j) { return j; });
	data = del2.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del3(&ReturnNumber);
	MIST_ASSERT((del3.HasSignature<size_t, size_t>()) == true);
	data = del3.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);

	// MetaData

	const size_t META_DATA_VALUE = 5;
	
	Mist::MetaData metaData;

	MIST_ASSERT(metaData.Has("Lol") == false);
	MIST_ASSERT((*metaData.Add<size_t>("Foo", META_DATA_VALUE)) == META_DATA_VALUE);
	MIST_ASSERT(metaData.Has("Foo"));
	MIST_ASSERT((*metaData.Get<size_t>("Foo")) == META_DATA_VALUE);

	metaData.Add("Hi", META_DATA_VALUE);
	metaData.Add("This", META_DATA_VALUE);

	for (auto& i : metaData) {
		std::cout << i.first << ":" << *Mist::Cast<size_t>(i.second) << std::endl;
		MIST_ASSERT(*Mist::Cast<size_t>(i.second) == META_DATA_VALUE);
	}

	struct TestClass {
		size_t data = META_DATA_VALUE;
	};

	MIST_ASSERT(metaData.Add(Mist::HashID(1), TestClass())->data == META_DATA_VALUE);
	MIST_ASSERT(metaData.Has(Mist::HashID(1)));
	MIST_ASSERT(metaData.Get<TestClass>(Mist::HashID(1))->data == META_DATA_VALUE);
	
	// -MethodInfo-

	struct TestMethodInfo {

		size_t Value() { std::cout << "TestMethodInfo::Value" << std::endl; return CHANGE_TARGET; }
		size_t Repeat(size_t value) { return value; }

		size_t m_Value = CHANGE_TARGET;
	};

	TestMethodInfo obj;

	Mist::MethodInfo method(&TestMethodInfo::Value);
	MIST_ASSERT(method.Invoke<size_t>(&obj) == CHANGE_TARGET);

	Mist::MethodInfo repeatMethod(&TestMethodInfo::Repeat);
	MIST_ASSERT(repeatMethod.Invoke<size_t>(&obj, CHANGE_TARGET) == CHANGE_TARGET);

	Mist::MetaData* meta = repeatMethod.GetMetaData();
	meta->Add<size_t>("Property", CHANGE_TARGET);

	MIST_ASSERT(*meta->Get<size_t>("Property") == CHANGE_TARGET);

	// -MemberInfo-

	TestMethodInfo memberTest;

	Mist::MemberInfo member(&TestMethodInfo::m_Value);
	MIST_ASSERT(*member.Get<size_t>(&memberTest) == CHANGE_TARGET);

	Mist::MetaData* memberMera = member.GetMetaData();
	memberMera->Add<size_t>("Property", CHANGE_TARGET);

	MIST_ASSERT(*memberMera->Get<size_t>("Property") == CHANGE_TARGET);


	// -TypeInfo-

	Mist::Type testType;
	testType.DefineTypes<TestMethodInfo>();
	Mist::TypeInfo typeInfo(std::move(testType));

	Mist::MethodInfo* repeatMethodInfo = typeInfo.AddMethod("Repeat", &TestMethodInfo::Repeat);
	Mist::MemberInfo* valueMemberInfo = typeInfo.AddMember("m_Value", &TestMethodInfo::m_Value);

	MIST_ASSERT(typeInfo.HasConstructor<TestMethodInfo>() == false);
	typeInfo.AddConstructor<TestMethodInfo>();
	MIST_ASSERT(typeInfo.HasConstructor<TestMethodInfo>());

	MIST_ASSERT(typeInfo.HasMethod("Repeat"));
	repeatMethodInfo = typeInfo.GetMethodInfo("Repeat");

	MIST_ASSERT(typeInfo.HasMethod("Lol") == false);

	MIST_ASSERT(typeInfo.HasMember("m_Value"));
	valueMemberInfo = typeInfo.GetMemberInfo("m_Value");

	MIST_ASSERT(typeInfo.HasMember("Lol") == false);

	TestMethodInfo* result = typeInfo.Create<TestMethodInfo>(5);
	// These constructors aren't available
	MIST_ASSERT(result == nullptr);
	MIST_ASSERT(typeInfo.TryCreate<TestMethodInfo>(&result, 5) == false);

	result = typeInfo.Create<TestMethodInfo>();
	MIST_ASSERT(result != nullptr);

	TestMethodInfo* otherResult;
	MIST_ASSERT(typeInfo.TryCreate<TestMethodInfo>(&otherResult));
	MIST_ASSERT(otherResult != nullptr);
	delete otherResult;

	MIST_ASSERT((repeatMethodInfo->Invoke<size_t, TestMethodInfo, size_t>(result, CHANGE_TARGET)) == CHANGE_TARGET);
	MIST_ASSERT(*valueMemberInfo->Get<size_t>(result) == CHANGE_TARGET);
	delete result;

	Mist::MetaData* typeMeta = typeInfo.GetMetaData();
	MIST_ASSERT(typeMeta->Has("Strength") == false);
	typeMeta->Add("Strength", 1.0f);
	MIST_ASSERT(typeMeta->Has("Strength"));
	float* strength = typeMeta->Get<float>("Strength");
	MIST_ASSERT(*strength == 1.0f);
	*strength = 10.0f;
	MIST_ASSERT(*typeMeta->Get<float>("Strength") == 10.0f);

	struct NonDefault {
		NonDefault() {
			m_Data = CHANGE_TARGET;
		}

		NonDefault(size_t value) {
			m_Data = value;
		}

		size_t m_Data;
	};

	Mist::Type ndType;
	ndType.DefineTypes<NonDefault>();

	Mist::TypeInfo nonDefaultType(std::move(ndType));
	nonDefaultType.AddConstructor<NonDefault>();
	nonDefaultType.AddConstructor<NonDefault, size_t>();

	// Assure that non default constructors work
	NonDefault* d = nonDefaultType.Create<NonDefault>(CHANGE_TARGET);
	MIST_ASSERT(d != nullptr);
	delete d;

	d = nonDefaultType.Create<NonDefault>();
	MIST_ASSERT(d != nullptr);
	delete d;

	Mist::Reflection reflection;
	Mist::TypeInfo* reflectedType = reflection.AddType<NonDefault>("NonDefault");
	MIST_ASSERT(reflectedType != nullptr);
	Mist::TypeInfo* otherReflected = reflection.GetType("NonDefault");

	MIST_ASSERT(reflectedType == otherReflected);

	Mist::GlobalFunction* global = reflection.AddGlobalFunction("ReturnNumber", &ReturnNumber);
	global->GetMetaData()->Add("Meta", (size_t)10);
	MIST_ASSERT(*global->GetMetaData()->Get<size_t>("Meta") == 10);

	Mist::GlobalFunction* otherGlobal = reflection.GetGlobalFunction("ReturnNumber");
	MIST_ASSERT(global == otherGlobal);

	MIST_ASSERT((global->Invoke<size_t, size_t>(CHANGE_TARGET)) == CHANGE_TARGET);

	Mist::GlobalObject* gAny = reflection.AddGlobalObject("Any", &g_SomeGlobal);
	gAny->GetMetaData()->Add("Meta", (size_t)10);
	MIST_ASSERT(*gAny->GetMetaData()->Get<size_t>("Meta") == 10);

	Mist::GlobalObject* otherAny = reflection.GetGlobalObject("Any");

	MIST_ASSERT(gAny == otherAny);
	MIST_ASSERT((*gAny->Get<size_t>()) == 10);

	Mist::Reflection mergedTarget;
	Mist::Merge(std::move(reflection), &mergedTarget);

	global = mergedTarget.GetGlobalFunction("ReturnNumber");
	MIST_ASSERT((global->Invoke<size_t, size_t>(CHANGE_TARGET)) == CHANGE_TARGET);

	std::cout << "Reflection Test Passed!" << std::endl;
}

int main() {

	TestReflection();

	return 0;
}