#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/types.h>
#include <cts/typedefs/gl_shader.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlVertexInputAttributeDescription {
    GLuint location;
    uint32_t binding;
    GLenum format;
    GLuint numComponents;
    GLsizei offset;
    GLsizei stride;
    CtsVertexInputRate inputRate;
} CtsGlVertexInputAttributeDescription;

typedef struct CtsGlPipelineVertexInputState {
    uint32_t vertexAttributeDescriptionCount;
    CtsGlVertexInputAttributeDescription* vertexAttributeDescriptions;
} CtsGlPipelineVertexInputState;

typedef struct CtsGlPipelineInputAssemblyState {
    GLenum polygonMode;
    bool primitiveRestartEnable;
} CtsGlPipelineInputAssemblyState;

typedef struct CtsGlPipelineTessellationState {
    uint32_t patchControlPoints;
} CtsGlPipelineTessellationState;

typedef struct CtsGlPipelineViewportState {
    uint32_t viewportCount;
    uint32_t scissorCount;
    CtsViewport* viewports;
    CtsRect2D* scissors;
} CtsGlPipelineViewportState;

typedef struct CtsGlPipelineRasterizationState {
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
  char placeholder;
} CtsGlPipelineMultisampleState;

typedef struct CtsGlPipelineDepthStencilState {
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
    bool blendEnable;
    GLenum srcColorBlendFactor;
    GLenum dstColorBlendFactor;
    GLenum colorBlendOp;
    GLenum srcAlphaBlendFactor;
    GLenum dstAlphaBlendFactor;
    GLenum alphaBlendOp;
    uint32_t colorWriteMask;
    float blendConstants[4];
} CtsGlPipelineColorBlendStateAttachment;

typedef struct CtsGlPipelineColorBlendState {
    uint32_t attachmentCount;
    CtsGlPipelineColorBlendStateAttachment* attachments;
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