#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/command_pool_create_flags.h>
#include <cts/typedefs/descriptor_pool_create_flags.h>
#include <cts/typedefs/descriptor_pool_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorPoolImpl* CtsDescriptorPool;

typedef struct CtsDescriptorPoolCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsDescriptorPoolCreateFlags flags;
    uint32_t maxSets;
    uint32_t poolSizeCount;
    const CtsDescriptorPoolSize* pPoolSizes;
} CtsDescriptorPoolCreateInfo;

#ifdef __cplusplus
}
#endif