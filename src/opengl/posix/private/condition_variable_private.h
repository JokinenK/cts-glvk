#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsConditionVariable {
    pthread_cond_t conditionVariable;
};

#ifdef __cplusplus
}
#endif