#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyDescriptorPool {
    CtsCmdBase base;
    CtsDevice device;
    CtsDescriptorPool descriptorPool;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyDescriptorPool;

#ifdef __cplusplus
}
#endif