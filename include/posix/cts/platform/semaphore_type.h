#pragma once

#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPlatformSemaphore {
    sem_t semaphore;
    int maxValue;
} CtsPlatformSemaphore;

#ifdef __cplusplus
}
#endif