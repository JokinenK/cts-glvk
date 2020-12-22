#pragma once

#include <stdint.h>
#include <cts/typedefs/pipeline.h>
#include <cts/typedefs/pipeline_layout.h>
#include <cts/typedefs/render_pass.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGraphicsPipelineCreateInfo {
    CtsStructureType sType;
    const void* next;
    uint32_t stageCount;
    const CtsPipelineShaderStageCreateInfo* stages;
    const CtsPipelineVertexInputStateCreateInfo* vertexInputState;
    const CtsPipelineInputAssemblyStateCreateInfo* inputAssemblyState;
    const CtsPipelineTessellationStateCreateInfo* tessellationState;
    const CtsPipelineViewportStateCreateInfo* viewportState;
    const CtsPipelineRasterizationStateCreateInfo* rasterizationState;
    const CtsPipelineMultisampleStateCreateInfo* multisampleState;
    const CtsPipelineDepthStencilStateCreateInfo* depthStencilState;
    const CtsPipelineColorBlendStateCreateInfo* colorBlendState;
    const CtsPipelineDynamicStateCreateInfo* dynamicState;
    CtsPipelineLayout layout;
    CtsRenderPass renderPass;
    uint32_t subpass;
    CtsPipeline basePipelineHandle;
    int32_t basePipelineIndex;
} CtsGraphicsPipelineCreateInfo;

#ifdef __cplusplus
}
#endif

