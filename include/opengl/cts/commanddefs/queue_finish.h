#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueFinish {
    CtsCmdBase base;
    CtsDevice device;
    uint32_t semaphoreCount;
    const CtsSemaphore* semaphores;
    CtsCommandBuffer commandBuffer;
} CtsQueueFinish;

#ifdef __cplusplus
}
#endif