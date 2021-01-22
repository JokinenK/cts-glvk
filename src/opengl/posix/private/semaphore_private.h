#pragma once

#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphoreImpl {
    sem_t semaphore;
};

#ifdef __cplusplus
}
#endif