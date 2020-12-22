#include <windows.h>
#include <cts/os/condition_variable.h>
#include <private/condition_variable_private.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateConditionVariables(
    const CtsConditionVariableCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pConditionVariableCount,
    CtsConditionVariable* pConditionVariables
) {
    for (uint32_t i = 0; i < pConditionVariableCount; ++i) {
        CtsConditionVariable conditionVariable = ctsAllocation(
            pAllocator,
            sizeof(struct CtsConditionVariable),
            alignof(struct CtsConditionVariable),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!conditionVariable) {
            return false;
        }

        InitializeConditionVariable(&conditionVariable->conditionVariable);
        pConditionVariables[i] = conditionVariable;
    }

    return true;
}

bool ctsDestroyConditionVariable(
    CtsConditionVariable pConditionVariable,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pConditionVariable) {
        ctsFree(pAllocator, pConditionVariable);
        return true;
    }

    return false;
}

void ctsConditionVariableSleep(
    CtsConditionVariable pConditionVariable,
    CtsMutex pMutex
) {
    if (pConditionVariable != NULL && pMutex != NULL) {
        SleepConditionVariableCS(
            &pConditionVariable->conditionVariable,
            &pMutex->criticalSection,
            INFINITE
        );
    }
}

void ctsConditionVariableWake(
    CtsConditionVariable pConditionVariable
) {
    WakeConditionVariable(&pConditionVariable->conditionVariable);
}

void ctsConditionVariableWakeAll(
    CtsConditionVariable pConditionVariable
) {
    WakeAllConditionVariable(&pConditionVariable->conditionVariable);
}

#ifdef __cplusplus
}
#endif