#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/mutex.h>
#include <cts/typedefs/condition_variable.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateConditionVariable(
    const CtsConditionVariableCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsConditionVariable* pConditionVariable
);

void ctsDestroyConditionVariable(
    CtsConditionVariable conditionVariable,
    const CtsAllocationCallbacks* pAllocator
);

void ctsConditionVariableSleep(
    CtsConditionVariable conditionVariable,
    CtsMutex mutex
);

void ctsConditionVariableWake(
    CtsConditionVariable conditionVariable
);

void ctsConditionVariableWakeAll(
    CtsConditionVariable conditionVariable
);

#ifdef __cplusplus
}
#endif