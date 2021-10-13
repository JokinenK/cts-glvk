#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include <cts/typedefs/gl_rasterization_state_changes.h>
#include <cts/typedefs/gl_depth_stencil_state_changes.h>
#include <cts/typedefs/gl_color_blend_state_changes.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFormatData {
    VkFormat vkFormat;
    GLenum format;
    GLenum type;
    GLenum internalFormat;
    GLint numComponents;
    GLboolean normalized;
} CtsFormatData;

const GLenum parseCompareOp(VkCompareOp value);
const GLenum parseStencilOp(VkStencilOp value);
const GLuint parseCullMode(VkCullModeFlags value);
const GLenum parseFrontFace(VkFrontFace value);
const GLenum parsePrimitiveTopology(VkPrimitiveTopology value);
const GLenum parsePolygonMode(VkPolygonMode value);
const GLenum parseSamplerAddressMode(VkSamplerAddressMode value);
const GLenum parseMinFilter(VkFilter filter, VkSamplerMipmapMode mipmapMode);
const GLenum parseMagFilter(VkFilter filter, VkSamplerMipmapMode mipmapMode);
const GLenum parseShaderType(VkShaderStageFlagBits value);
const GLenum parseImageType(VkImageType value, bool isMultisample, bool isArray);
const GLenum parseImageViewType(VkImageViewType value, bool isMultisample);
const GLenum parseBufferUsage(VkBufferUsageFlags value);
const GLenum parseBlendFactor(VkBlendFactor value);
const GLenum parseBlendOperation(VkBlendOp value);
const GLenum parseLogicOp(VkLogicOp value);
const bool isFloatBorderColor(VkBorderColor borderColor);
const float* parseBorderColorFloat(VkBorderColor borderColor);
const int* parseBorderColorInt(VkBorderColor borderColor);

const VkFlags parseDynamicStateFlag(VkDynamicState value);
const CtsFormatData* parseFormat(VkFormat value);

bool blendStateBlendConstantChanged(
    const CtsGlPipelineColorBlendState* pLhs,
    const CtsGlPipelineColorBlendState* pRhs
);

void parseRasterizationStateChanges(
    const CtsGlPipelineRasterizationState* pLhs, 
    const CtsGlPipelineRasterizationState* pRhs, 
    CtsGlRasterizationStateChanges* pChanges
);

void parseDepthStencilStateChanges(
    const CtsGlPipelineDepthStencilState* pLhs, 
    const CtsGlPipelineDepthStencilState* pRhs, 
    CtsGlDepthStencilStateChanges* pChanges
);

void parseColorBlendStateChanges(
    const CtsGlPipelineColorBlendStateAttachment* pLhs, 
    const CtsGlPipelineColorBlendStateAttachment* pRhs, 
    CtsGlColorBlendStateChanges* pChanges
);

uint32_t parseDynamicStateFlagBits(
    const VkPipelineDynamicStateCreateInfo* pCreateInfo
);

#ifdef __cplusplus
}
#endif