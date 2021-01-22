#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/command_pool.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCommandBufferImpl* CtsCommandBuffer;

typedef struct CtsCommandBufferAllocateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsCommandPool commandPool;
    CtsCommandBufferLevel level;
    uint32_t commandBufferCount;
} CtsCommandBufferAllocateInfo;

#ifdef __cplusplus
}
#endif