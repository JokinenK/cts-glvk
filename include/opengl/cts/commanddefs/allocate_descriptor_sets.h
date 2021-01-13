#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateDescriptorSets {
    CtsCmdBase base;
    CtsDevice device;
    const CtsDescriptorSetAllocateInfo* pAllocateInfo;
    CtsDescriptorSet* pDescriptorSets;
    CtsResult* pResult;
} CtsAllocateDescriptorSets;

#ifdef __cplusplus
}
#endif