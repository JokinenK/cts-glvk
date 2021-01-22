#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <cts/condition_variable.h>
#include <private/condition_variable_private.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateConditionVariable(
    const CtsConditionVariableCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsConditionVariable* pConditionVariable
) {
    CtsConditionVariable conditionVariable = ctsAllocation(
        pAllocator,
        sizeof(struct CtsConditionVariableImpl),
        alignof(struct CtsConditionVariableImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (conditionVariable == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    InitializeConditionVariable(&conditionVariable->conditionVariable);
    *pConditionVariable = conditionVariable;

    return CTS_SUCCESS;
}

void ctsDestroyConditionVariable(
    CtsConditionVariable conditionVariable,
    const CtsAllocationCallbacks* pAllocator
) {
    if (conditionVariable != NULL) {
        ctsFree(pAllocator, conditionVariable);
    }
}

void ctsConditionVariableSleep(
    CtsConditionVariable conditionVariable,
    CtsMutex mutex
) {
    if (conditionVariable != NULL && mutex != NULL) {
        SleepConditionVariableCS(
            &conditionVariable->conditionVariable,
            &mutex->criticalSection,
            INFINITE
        );
    }
}

void ctsConditionVariableWake(
    CtsConditionVariable conditionVariable
) {
    WakeConditionVariable(&conditionVariable->conditionVariable);
}

void ctsConditionVariableWakeAll(
    CtsConditionVariable conditionVariable
) {
    WakeAllConditionVariable(&conditionVariable->conditionVariable);
}

#ifdef __cplusplus
}
#endif