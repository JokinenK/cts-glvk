#pragma once

#include <stdint.h>
#include <cts/typedefs/shader_stage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPushConstantRange {
    CtsShaderStageFlags stageFlags;
    uint32_t offset;
    uint32_t size;
} CtsPushConstantRange;

#ifdef __cplusplus
}
#endif
