#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateDescriptorSetLayout {
    CtsCmdBase base;
    CtsDevice device;
    const CtsDescriptorSetLayoutCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsDescriptorSetLayout* descriptorSetLayout;
    CtsResult* result;
} CtsCreateDescriptorSetLayout;

#ifdef __cplusplus
}
#endif