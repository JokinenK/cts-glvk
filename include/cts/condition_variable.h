#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/mutex.h>
#include <cts/typedefs/condition_variable.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateConditionVariables(
    const CtsConditionVariableCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pConditionVariableCount,
    CtsConditionVariable* pConditionVariables
);

bool ctsDestroyConditionVariable(
    CtsConditionVariable pConditionVariable,
    const CtsAllocationCallbacks* pAllocator
);

void ctsConditionVariableSleep(
    CtsConditionVariable pConditionVariable,
    CtsMutex pMutex
);

void ctsConditionVariableWake(
    CtsConditionVariable pConditionVariable
);

void ctsConditionVariableWakeAll(
    CtsConditionVariable pConditionVariable
);

#ifdef __cplusplus
}
#endif