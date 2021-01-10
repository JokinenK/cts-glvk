#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphore {
    HANDLE semaphore;
};

#ifdef __cplusplus
}
#endif