#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsConditionVariableImpl {
    pthread_cond_t conditionVariable;
};

#ifdef __cplusplus
}
#endif