#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphoreImpl {
    HANDLE semaphore;
};

#ifdef __cplusplus
}
#endif