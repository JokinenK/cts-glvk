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

typedef struct CtsFormatData {
    GLenum format;
    GLenum type;
    GLenum internalFormat;
} CtsFormatData;

typedef struct CtsAttributeInfo {
    GLenum type;
    GLsizei size;
    GLint numComponent;
} CtsAttributeMapping;

const GLenum parseCompareOperator(CtsCompareOperator value);
const GLenum parseStencilAction(CtsStencilAction value);
const GLenum parseCullMode(CtsCullMode value);
const GLenum parseFrontFace(CtsFrontFace value);
const GLenum parsePolygonMode(CtsPolygonMode value);
const GLenum parseSamplerAddressMode(CtsSamplerAddressMode value);
const GLenum parseFilter(CtsFilter value);
const GLenum parseIndexType(CtsIndexType value);
const GLenum parseShaderType(CtsShaderType value);
const GLenum parseImageType(CtsImageType value);
const GLenum parsePrimitiveType(CtsPrimitiveType value);
const GLenum parseBufferUsage(CtsBufferUsage value);
const GLenum parseBufferType(CtsBufferType value);
const GLenum parseBlendFunc(CtsBlendFunc value);
const GLenum parseBlendOperation(CtsBlendOp value);
const GLenum parseRenderTargetAttachment(CtsRenderTargetAttachment value);
const GLenum parseRenderTargetBind(CtsRenderTargetBind value);

const CtsFlags parseDynamicStateFlag(CtsDynamicState value);
const CtsAttribType parseCtsAttribType(GLenum pType);
const CtsFormatData parseFormat(CtsFormat value);
const CtsAttributeMapping parseAttributeMapping(CtsAttribType value);
const CtsGlDescriptorType parseDescriptorType(CtsDescriptorType value);

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

bool parseColorBlendStateBlendConstantChanged(
    CtsGlPipelineColorBlendState* pLhs,
    CtsGlPipelineColorBlendState* pRhs
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