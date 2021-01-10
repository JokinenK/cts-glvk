#pragma once

#include <stdbool.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsThread {
    pthread_t thread;
    bool joined;
    bool detached;
};

#ifdef __cplusplus
}
#endif