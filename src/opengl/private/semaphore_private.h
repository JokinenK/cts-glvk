#pragma once

#include "cts/platform/platform_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphore {
    CtsPlatformSemaphore semaphore;
};

#ifdef __cplusplus
}
#endif