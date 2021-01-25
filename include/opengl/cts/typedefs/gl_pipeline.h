#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/types.h>
#include <cts/typedefs/gl_shader.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlPipelineVertexInputState {
    bool initialized;
    GLuint vao;
    uint32_t vertexBindingDescriptionCount;
    CtsVertexInputBindingDescription* pVertexBindingDescriptions;
} CtsGlPipelineVertexInputState;

typedef struct CtsGlPipelineInputAssemblyState {
    bool initialized;
    GLenum topology;
    bool primitiveRestartEnable;
} CtsGlPipelineInputAssemblyState;

typedef struct CtsGlPipelineTessellationState {
    bool initialized;
    uint32_t patchControlPoints;
} CtsGlPipelineTessellationState;

typedef struct CtsGlPipelineViewportState {
    bool initialized;
    uint32_t viewportCount;
    uint32_t scissorCount;
    CtsViewport* pViewports;
    CtsRect2D* pScissors;
} CtsGlPipelineViewportState;

typedef struct CtsGlPipelineRasterizationState {
    bool initialized;
    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    GLenum polygonMode;
    GLenum cullMode;
    GLenum frontFace;
    bool depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
} CtsGlPipelineRasterizationState;

typedef struct CtsGlPipelineMultisampleState {
    bool initialized;
} CtsGlPipelineMultisampleState;

typedef struct CtsGlPipelineDepthStencilState {
    bool initialized;

    bool depthTestEnable;
    bool depthWriteEnable;
    GLenum depthCompareOp;
    bool depthBoundsTestEnable;
    bool stencilTestEnable;
    float minDepthBounds;
    float maxDepthBounds;

    GLenum frontFailOp;
    GLenum frontPassOp;
    GLenum frontDepthFailOp;
    GLenum frontCompareOp;
    uint32_t frontCompareMask;
    uint32_t frontWriteMask;
    uint32_t frontReference;

    GLenum backFailOp;
    GLenum backPassOp;
    GLenum backDepthFailOp;
    GLenum backCompareOp;
    uint32_t backCompareMask;
    uint32_t backWriteMask;
    uint32_t backReference;
} CtsGlPipelineDepthStencilState;

typedef struct CtsGlPipelineColorBlendStateAttachment {
    bool initialized;
    bool blendEnable;
    GLenum srcColorBlendFactor;
    GLenum dstColorBlendFactor;
    GLenum colorBlendOp;
    GLenum srcAlphaBlendFactor;
    GLenum dstAlphaBlendFactor;
    GLenum alphaBlendOp;
    uint32_t colorWriteMask;
} CtsGlPipelineColorBlendStateAttachment;

typedef struct CtsGlPipelineColorBlendState {
    bool initialized;
    uint32_t attachmentCount;
    CtsGlPipelineColorBlendStateAttachment* pAttachments;
    float blendConstants[4];
    bool logicOpEnable;
    GLenum logicOp;
} CtsGlPipelineColorBlendState;

typedef struct CtsGlGraphicsPipeline {
    CtsGlShader shader;
    CtsGlPipelineVertexInputState vertexInputState;
    CtsGlPipelineInputAssemblyState inputAssemblyState;
    CtsGlPipelineTessellationState tessellationState;
    CtsGlPipelineViewportState viewportState;
    CtsGlPipelineRasterizationState rasterizationState;
    CtsGlPipelineMultisampleState multisampleState;
    CtsGlPipelineDepthStencilState depthStencilState;
    CtsGlPipelineColorBlendState colorBlendState;
    CtsFlags dynamicState;
    CtsPipelineLayout layout;
    CtsRenderPass renderPass;
    uint32_t subpass;
    CtsPipeline basePipelineHandle;
    int32_t basePipelineIndex;
} CtsGlGraphicsPipeline;

#ifdef __cplusplus
}
#endif