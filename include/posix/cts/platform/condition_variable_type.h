#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_cond_t CtsPlatformConditionVariable;

#ifdef __cplusplus
}
#endif