#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/types.h>
#include <cts/typedefs/gl_pipeline.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipeline {
    CtsPipelineBindPoint bindPoint;

    union {
        CtsGlGraphicsPipeline* graphics;
    };
};

CtsResult ctsCreateGraphicsPipelinesImpl(
    CtsDevice pDevice,
    CtsPipelineCache pPipelineCache,
    uint32_t pCreateInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
);

void ctsDestroyPipelineImpl(
    CtsDevice pDevice,
    CtsPipeline pPipeline,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif