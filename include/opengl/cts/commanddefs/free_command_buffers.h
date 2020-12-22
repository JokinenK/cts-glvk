#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeCommandBuffers {
    CtsCmdBase base;
    CtsDevice device;
    CtsCommandPool commandPool;
    uint32_t commandBufferCount;
    const CtsCommandBuffer* commandBuffers;
} CtsFreeCommandBuffers;

#ifdef __cplusplus
}
#endif