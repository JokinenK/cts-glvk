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
    const void* pNext;
    uint32_t stageCount;
    const CtsPipelineShaderStageCreateInfo* stages;
    const CtsPipelineVertexInputStateCreateInfo* pVertexInputState;
    const CtsPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
    const CtsPipelineTessellationStateCreateInfo* pTessellationState;
    const CtsPipelineViewportStateCreateInfo* pViewportState;
    const CtsPipelineRasterizationStateCreateInfo* pRasterizationState;
    const CtsPipelineMultisampleStateCreateInfo* pMultisampleState;
    const CtsPipelineDepthStencilStateCreateInfo* pDepthStencilState;
    const CtsPipelineColorBlendStateCreateInfo* pColorBlendState;
    const CtsPipelineDynamicStateCreateInfo* pDynamicState;
    CtsPipelineLayout layout;
    CtsRenderPass renderPass;
    uint32_t subpass;
    CtsPipeline basePipelineHandle;
    int32_t basePipelineIndex;
} CtsGraphicsPipelineCreateInfo;

#ifdef __cplusplus
}
#endif

