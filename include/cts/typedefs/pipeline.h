#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/color_component_flags.h>
#include <cts/typedefs/rect.h>
#include <cts/typedefs/shader_module.h>
#include <cts/typedefs/stencil.h>
#include <cts/typedefs/viewport.h>
#include <cts/typedefs/vertex_input.h>
#include <cts/typedefs/specialization_info.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPipeline* CtsPipeline;
typedef struct CtsPipelineCache* CtsPipelineCache;

typedef struct CtsPipelineShaderStageCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsShaderType stage;
    CtsShaderModule module;
    const char* pName;
    const CtsSpecializationInfo* pSpecializationInfo;
} CtsPipelineShaderStageCreateInfo;

typedef struct CtsPipelineVertexInputStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    uint32_t vertexBindingDescriptionCount;
    const CtsVertexInputBindingDescription* pVertexBindingDescriptions;
    uint32_t vertexAttributeDescriptionCount;
    const CtsVertexInputAttributeDescription* pVertexAttributeDescriptions;
} CtsPipelineVertexInputStateCreateInfo;

typedef struct CtsPipelineInputAssemblyStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsPolygonMode polygonMode;
    bool primitiveRestartEnable;
} CtsPipelineInputAssemblyStateCreateInfo;

typedef struct CtsPipelineTessellationStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    uint32_t patchControlPoints;
} CtsPipelineTessellationStateCreateInfo;

typedef struct CtsPipelineViewportStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    uint32_t viewportCount;
    const CtsViewport* pViewports;
    uint32_t scissorCount;
    const CtsRect2D* pScissors;
} CtsPipelineViewportStateCreateInfo;

typedef struct CtsPipelineRasterizationStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    CtsPolygonMode polygonMode;
    CtsCullMode cullMode;
    CtsFrontFace frontFace;
    bool depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
} CtsPipelineRasterizationStateCreateInfo;

typedef struct CtsPipelineMultisampleStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsSampleCountFlagBits rasterizationSamples;
    bool sampleShadingEnable;
    float minSampleShading;
    const uint32_t* pSampleMask;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
} CtsPipelineMultisampleStateCreateInfo;

typedef struct CtsPipelineDepthStencilStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    bool depthTestEnable;
    bool depthWriteEnable;
    CtsCompareOperator depthCompareOp;
    bool depthBoundsTestEnable;
    bool stencilTestEnable;
    CtsStencilOpState front;
    CtsStencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;
} CtsPipelineDepthStencilStateCreateInfo;

typedef struct CtsPipelineColorBlendAttachmentState {
    CtsStructureType sType;
    const void* pNext;
    bool blendEnable;
    CtsBlendFunc srcColorBlendFactor;
    CtsBlendFunc dstColorBlendFactor;
    CtsBlendOp colorBlendOp;
    CtsBlendFunc srcAlphaBlendFactor;
    CtsBlendFunc dstAlphaBlendFactor;
    CtsBlendOp alphaBlendOp;
    CtsColorComponentFlags colorWriteMask;
} CtsPipelineColorBlendAttachmentState;

typedef struct CtsPipelineColorBlendStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    bool logicOpEnable;
    CtsLogicOp logicOp;
    uint32_t attachmentCount;
    const CtsPipelineColorBlendAttachmentState* pAttachments;
    float blendConstants[4];
} CtsPipelineColorBlendStateCreateInfo;

typedef struct CtsPipelineDynamicStateCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    uint32_t dynamicStateCount;
    const CtsDynamicState* pDynamicStates;
} CtsPipelineDynamicStateCreateInfo;

#ifdef __cplusplus
}
#endif