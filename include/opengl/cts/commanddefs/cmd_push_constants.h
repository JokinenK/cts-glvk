#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdPushConstants {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsPipelineLayout layout;
    CtsShaderStageFlags stageFlags;
    uint32_t offset;
    uint32_t size;
    const void* values;
} CtsCmdPushConstants;

#ifdef __cplusplus
}
#endif