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