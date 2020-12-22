#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyDescriptorSetLayout {
    CtsCmdBase base;
    CtsDevice device;
    CtsDescriptorSetLayout descriptorSetLayout;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyDescriptorSetLayout;

#ifdef __cplusplus
}
#endif