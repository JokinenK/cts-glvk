#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/command_pool_create_flags.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCommandPoolImpl* CtsCommandPool;

typedef struct CtsCommandPoolCreateInfo {
    CtsStructureType            sType;
    const void*                 pNext;
    CtsCommandPoolCreateFlags   flags;
    uint32_t                    queueFamilyIndex;
} CtsCommandPoolCreateInfo;

#ifdef __cplusplus
}
#endif