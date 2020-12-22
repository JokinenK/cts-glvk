#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreatePipelineLayout {
    CtsCmdBase base;
    CtsDevice device;
    const CtsPipelineLayoutCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsPipelineLayout* pipelineLayout;
    CtsResult* result;
} CtsCreatePipelineLayout;

#ifdef __cplusplus
}
#endif