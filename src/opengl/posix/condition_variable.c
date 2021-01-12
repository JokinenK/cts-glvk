#include <pthread.h>
#include <cts/condition_variable.h>
#include <private/condition_variable_private.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateConditionVariables(
    const CtsConditionVariableCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsConditionVariable* pConditionVariable
) {
    CtsConditionVariable conditionVariable = ctsAllocation(
        pAllocator,
        sizeof(struct CtsConditionVariable),
        alignof(struct CtsConditionVariable),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (conditionVariable == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    pthread_cond_init(&conditionVariable->conditionVariable, NULL);
    *pConditionVariable = conditionVariable;

    return CTS_SUCCESS;
}

void ctsDestroyConditionVariable(
    CtsConditionVariable pConditionVariable,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pConditionVariable != NULL) {
        ctsFree(pAllocator, pConditionVariable);
    }
}

void ctsConditionVariableSleep(
    CtsConditionVariable pConditionVariable,
    CtsMutex pMutex
) {
    if (pConditionVariable != NULL && pMutex != NULL) {
        pthread_cond_wait(
            &pConditionVariable->conditionVariable,
            &pMutex->mutex
        );
    }
}

void ctsConditionVariableWake(
    CtsConditionVariable pConditionVariable
) {
    pthread_cond_signal(&pConditionVariable->conditionVariable);
}

void ctsConditionVariableWakeAll(
    CtsConditionVariable pConditionVariable
) {
    pthread_cond_broadcast(&pConditionVariable->conditionVariable);
}

#ifdef __cplusplus
}
#endif