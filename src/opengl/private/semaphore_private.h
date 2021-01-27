#pragma once

#include <cts/platform_semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphoreImpl {
    CtsPlatformSemaphore semaphore;
};

#ifdef __cplusplus
}
#endif