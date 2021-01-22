#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/typedefs/pipeline.h>
#include <cts/typedefs/gl_pipeline.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipelineImpl {
    CtsPipelineBindPoint bindPoint;

    union {
        CtsGlGraphicsPipeline* graphics;
    };
};

CtsResult ctsCreateGraphicsPipelinesImpl(
    CtsDevice device,
    CtsPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
);

void ctsDestroyPipelineImpl(
    CtsDevice device,
    CtsPipeline pipeline,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif