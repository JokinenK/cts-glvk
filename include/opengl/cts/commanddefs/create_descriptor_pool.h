#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateDescriptorPool {
    CtsCmdBase base;
    CtsDevice device;
    const CtsDescriptorPoolCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsDescriptorPool* descriptorPool;
    CtsResult* result;
} CtsCreateDescriptorPool;

#ifdef __cplusplus
}
#endif