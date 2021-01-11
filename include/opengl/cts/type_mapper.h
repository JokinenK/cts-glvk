#pragma once

#include <glad/glad.h>
#include <cts/types.h>
#include <cts/typedefs/gl_rasterization_state_changes.h>
#include <cts/typedefs/gl_depth_stencil_state_changes.h>
#include <cts/typedefs/gl_color_blend_state_changes.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsTextureFormatPair {
    GLenum format;
    GLint internalFormat;
} CtsFormatPair;

typedef struct CtsAttributeInfo {
    GLenum type;
    GLsizei size;
    GLint numComponent;
} CtsAttributeMapping;

const GLenum parseCompareOperator(CtsCompareOperator pValue);
const GLenum parseStencilAction(CtsStencilAction pValue);
const GLenum parseCullMode(CtsCullMode pValue);
const GLenum parseFrontFace(CtsFrontFace pValue);
const GLenum parsePolygonMode(CtsPolygonMode pValue);
const GLenum parseSamplerAddressMode(CtsSamplerAddressMode pValue);
const GLenum parseFilter(CtsFilter pValue);
const GLenum parseIndexType(CtsIndexType pValue);
const GLenum parseShaderType(CtsShaderType pValue);
const GLenum parseImageType(CtsImageType pValue);
const GLenum parsePrimitiveType(CtsPrimitiveType pValue);
const GLenum parseBufferUsage(CtsBufferUsage pValue);
const GLenum parseBufferType(CtsBufferType pValue);
const GLenum parseBlendFunc(CtsBlendFunc pValue);
const GLenum parseBlendOperation(CtsBlendOp pValue);
const GLenum parseRenderTargetAttachment(CtsRenderTargetAttachment pValue);
const GLenum parseRenderTargetBind(CtsRenderTargetBind pValue);

const CtsFlags parseDynamicStateFlag(CtsDynamicState pValue);
const CtsAttribType parseCtsAttribType(GLenum pType);
const CtsFormatPair parseFormatPair(CtsFormat pValue);
const CtsAttributeMapping parseAttributeMapping(CtsAttribType pValue);
const CtsGlDescriptorType parseDescriptorType(CtsDescriptorType pValue);

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

CtsFlagBit parseDynamicStateFlagBits(
    const CtsPipelineDynamicStateCreateInfo* pCreateInfo
);

#ifdef __cplusplus
}
#endif