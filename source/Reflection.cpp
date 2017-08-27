#include "../include/Reflection.h"
#include "../Mist_common/include/common/UtilityMacros.h"

MIST_NAMESPACE

void Merge(Reflection&& left, Reflection* output) {

	MIST_ASSERT(output != nullptr);

#if MIST_DEBUG

	size_t typeCount = output->GetTypes()->size();
	size_t functionCount = output->GetGlobalFunctions()->size();
	size_t objectCount = output->GetGlobalObjects()->size();

#endif

	// merge the typeinfos
	for (auto& typeInfo : *left.GetTypes()) {

		auto result = output->GetTypes()->emplace(typeInfo.first, std::move(typeInfo.second));
		MIST_ASSERT(result.second);
	}

	// merge the global functions
	for (auto& globalFunction : *left.GetGlobalFunctions()) {

		auto result = output->GetGlobalFunctions()->emplace(globalFunction.first, std::move(globalFunction.second));
		MIST_ASSERT(result.second);
	}

	// merge the global objects
	for (auto& globalObjects : *left.GetGlobalObjects()) {

		auto result = output->GetGlobalObjects()->emplace(globalObjects.first, std::move(globalObjects.second));
		MIST_ASSERT(result.second);
	}

#if MIST_DEBUG

	MIST_ASSERT(output->GetTypes()->size() == typeCount + left.GetTypes()->size());
	MIST_ASSERT(output->GetGlobalFunctions()->size() == functionCount + left.GetGlobalFunctions()->size());
	MIST_ASSERT(output->GetGlobalObjects()->size() == objectCount + left.GetGlobalObjects()->size());

#endif
}

MIST_NAMESPACE_END