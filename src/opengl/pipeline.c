#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cts/pipeline.h>
#include <cts/type_mapper.h>
#include <cts/commands.h>
#include <private/device_private.h>
#include <private/pipeline_private.h>
#include <private/shader_module_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool hasFlag(
    CtsFlags flags,
    CtsFlags flag
);

static CtsGlGraphicsPipeline* createGraphicsPipeline(
    const CtsGraphicsPipelineCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator
);

static void destroyGraphicsPipeline(
    CtsGlGraphicsPipeline* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createShader(
    uint32_t stageCount,
    const CtsPipelineShaderStageCreateInfo* stages,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlShader* pShader
);

static void destroyShader(
    CtsGlShader* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createVertexInputState(
    const CtsPipelineVertexInputStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineVertexInputState* pVertexInputState
);

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createInputAssemblyState(
    const CtsPipelineInputAssemblyStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineInputAssemblyState* pInputAssemblyState
);

static void destroyInputAssemblyState(
    CtsGlPipelineInputAssemblyState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createTessellationState(
    const CtsPipelineTessellationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineTessellationState* pTesselationState
);

static void destroyTessellationState(
    CtsGlPipelineTessellationState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createViewportState(
    const CtsPipelineViewportStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineViewportState* pViewportState
);

static void destroyViewportState(
    CtsGlPipelineViewportState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createRasterizationState(
    const CtsPipelineRasterizationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineRasterizationState* pRasterizationState
);

static void destroyRasterizationState(
    CtsGlPipelineRasterizationState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createMultisampleState(
    const CtsPipelineMultisampleStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineMultisampleState* pMultisampleState
);

static void destroyMultisampleState(
    CtsGlPipelineMultisampleState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createDepthStencilState(
    const CtsPipelineDepthStencilStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineDepthStencilState* pDepthStencilState
);

static void destroyDepthStencilState(
    CtsGlPipelineDepthStencilState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createColorBlendState(
    const CtsPipelineColorBlendStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineColorBlendState* pColorBlendState
);

static void destroyColorBlendState(
    CtsGlPipelineColorBlendState* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsCreateGraphicsPipelines(
    CtsDevice device,
    CtsPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
) {
    CtsResult result;
    CtsCreateGraphicsPipelines cmd;
    cmd.base.type = CTS_COMMAND_CREATE_GRAPHICS_PIPELINES;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pipelineCache = pipelineCache;
    cmd.createInfoCount = createInfoCount;
    cmd.pCreateInfos = pCreateInfos;
    cmd.pAllocator = pAllocator;
    cmd.pPipelines = pPipelines;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatch.mutex, device->dispatch.conditionVariable);

    return result;
}

void ctsDestroyPipeline(
    CtsDevice device,
    CtsPipeline pipeline,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyPipeline cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_PIPELINE;
    cmd.base.pNext = NULL;
    cmd.device = device;
    cmd.pipeline = pipeline;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatch.mutex, device->dispatch.conditionVariable);
}

CtsResult ctsCreateGraphicsPipelinesImpl(
    CtsDevice device,
    CtsPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
) {
    (void) device;
    (void) pipelineCache;

    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; i < createInfoCount; ++i) {
        CtsPipeline pipeline = ctsAllocation(
            pAllocator,
            sizeof(struct CtsPipeline),
            alignof(struct CtsPipeline),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (pipeline == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
			break;
        }

        pipeline->bindPoint = CTS_PIPELINE_BIND_POINT_GRAPHICS;
        pipeline->graphics = createGraphicsPipeline(&pCreateInfos[i], pAllocator);

        pPipelines[i] = pipeline;
    }

    for (; i < createInfoCount; ++i) {
        pPipelines[i] = NULL;
    }

    return result;
}

void ctsDestroyPipelineImpl(
    CtsDevice device,
    CtsPipeline pipeline,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (pipeline != NULL) {
        switch (pipeline->bindPoint) {
            case CTS_PIPELINE_BIND_POINT_GRAPHICS: {
                destroyGraphicsPipeline(pipeline->graphics, pAllocator);
            } break;
        }

        ctsFree(pAllocator, pipeline);
    }
}

#pragma endregion
#pragma region HelperMethodBodies

static bool hasFlag(
    CtsFlags flags,
    CtsFlagBit flag
) {
    return ((flags & flag) == flag);
}

static CtsGlGraphicsPipeline* createGraphicsPipeline(
    const CtsGraphicsPipelineCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pCreateInfo == NULL) {
        return NULL;
    }

    CtsGlGraphicsPipeline* graphicsPipeline = ctsAllocation(
        pAllocator,
        sizeof(CtsGlGraphicsPipeline),
        alignof(CtsGlGraphicsPipeline),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 
    
    createShader(pCreateInfo->stageCount, pCreateInfo->stages, pAllocator, &graphicsPipeline->shader);
    createVertexInputState(pCreateInfo->pVertexInputState, pAllocator, &graphicsPipeline->vertexInputState);
    createInputAssemblyState(pCreateInfo->pInputAssemblyState, pAllocator, &graphicsPipeline->inputAssemblyState);
    createTessellationState(pCreateInfo->pTessellationState, pAllocator, &graphicsPipeline->tessellationState);
    createViewportState(pCreateInfo->pViewportState, pAllocator, &graphicsPipeline->viewportState);
    createRasterizationState(pCreateInfo->pRasterizationState, pAllocator, &graphicsPipeline->rasterizationState);
    createMultisampleState(pCreateInfo->pMultisampleState, pAllocator, &graphicsPipeline->multisampleState);
    createDepthStencilState(pCreateInfo->pDepthStencilState, pAllocator, &graphicsPipeline->depthStencilState);
    createColorBlendState(pCreateInfo->pColorBlendState, pAllocator, &graphicsPipeline->colorBlendState);
    graphicsPipeline->dynamicState       = parseDynamicStateFlagBits(pCreateInfo->pDynamicState);
    graphicsPipeline->layout             = pCreateInfo->layout;
    graphicsPipeline->renderPass         = pCreateInfo->renderPass;
    graphicsPipeline->subpass            = pCreateInfo->subpass;
    graphicsPipeline->basePipelineHandle = pCreateInfo->basePipelineHandle;
    graphicsPipeline->basePipelineIndex  = pCreateInfo->basePipelineIndex;

    return graphicsPipeline;
}

static void destroyGraphicsPipeline(
    CtsGlGraphicsPipeline* pGraphicsPipeline,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pGraphicsPipeline != NULL) {
        destroyShader(&pGraphicsPipeline->shader, pAllocator);
        destroyVertexInputState(&pGraphicsPipeline->vertexInputState, pAllocator);
        destroyInputAssemblyState(&pGraphicsPipeline->inputAssemblyState, pAllocator);
        destroyTessellationState(&pGraphicsPipeline->tessellationState, pAllocator);
        destroyViewportState(&pGraphicsPipeline->viewportState, pAllocator);
        destroyRasterizationState(&pGraphicsPipeline->rasterizationState, pAllocator);
        destroyMultisampleState(&pGraphicsPipeline->multisampleState, pAllocator);
        destroyDepthStencilState(&pGraphicsPipeline->depthStencilState, pAllocator);
        destroyColorBlendState(&pGraphicsPipeline->colorBlendState, pAllocator);

        ctsFree(pAllocator, pGraphicsPipeline);
    }
}

static void createShader(
    uint32_t stageCount,
    const CtsPipelineShaderStageCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlShader* pShader
) {
    if (stageCount == 0 || pCreateInfos == NULL) {
        return;
    }

    pShader->handle = glCreateProgram();
    pShader->stageCount = stageCount;
    pShader->pStages = ctsAllocation(
        pAllocator,
        sizeof(CtsGlShaderStage) * stageCount,
        alignof(CtsGlShaderStage),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    GLint success;
    GLchar buffer[512];
    GLsizei bufferLen;

    for (uint32_t i = 0; i < stageCount; ++i) {
        const CtsPipelineShaderStageCreateInfo* pCreateInfo = &pCreateInfos[i];
        CtsGlShaderStage* stage = &pShader->pStages[i];

        stage->pName = pCreateInfo->pName;
        stage->handle = glCreateShader(parseShaderType(pCreateInfo->stage));
        const char* source = pCreateInfo->module->code;
        const int sourceLen = (const int)pCreateInfo->module->codeSize;

        glShaderSource(stage->handle, 1, &source, &sourceLen);
        glCompileShader(stage->handle);
        glGetShaderiv(stage->handle, GL_COMPILE_STATUS, &success);

        if (success) {
            glAttachShader(pShader->handle, pShader->handle);
        }
        else {
            glGetShaderInfoLog(pShader->handle, sizeof(buffer), &bufferLen, buffer);
            fprintf(stderr, "Shader compilation failed for stage %s. Reason %s", stage->pName, buffer);
        }
    }

    glLinkProgram(pShader->handle);
    glGetProgramiv(pShader->handle, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(pShader->handle, sizeof(buffer), &bufferLen, buffer);
        fprintf(stderr, "Shader linking failed: %s", buffer);
    }
}

static void destroyShader(
    CtsGlShader* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        for (uint32_t i = 0; i < pInstance->stageCount; ++i) {
            glDeleteShader(pInstance->pStages[i].handle);
        }

        glDeleteProgram(pInstance->handle);
        ctsFree(pAllocator, pInstance->pStages);
    }
}

static void createVertexInputState(
    const CtsPipelineVertexInputStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineVertexInputState* pVertexInputState
) {
    if (pCreateInfo == NULL) {
        return;
    }

    pVertexInputState->vertexAttributeDescriptionCount = pCreateInfo->vertexAttributeDescriptionCount;
    pVertexInputState->pVertexAttributeDescriptions = ctsAllocation(
        pAllocator,
        sizeof(CtsGlVertexInputAttributeDescription) * pCreateInfo->vertexAttributeDescriptionCount,
        alignof(CtsGlVertexInputAttributeDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->vertexAttributeDescriptionCount; ++i) {
        uint32_t binding = pCreateInfo->pVertexAttributeDescriptions[i].binding;
        CtsAttributeMapping attribute = parseAttributeMapping(pCreateInfo->pVertexAttributeDescriptions[i].format);

        // From attribute descriptions
        pVertexInputState->pVertexAttributeDescriptions[i].binding = binding;
        pVertexInputState->pVertexAttributeDescriptions[i].location = pCreateInfo->pVertexAttributeDescriptions[i].location;
        pVertexInputState->pVertexAttributeDescriptions[i].format = attribute.type;
        pVertexInputState->pVertexAttributeDescriptions[i].numComponents = attribute.numComponent;
        pVertexInputState->pVertexAttributeDescriptions[i].offset = pCreateInfo->pVertexAttributeDescriptions[i].offset;

        // From binding descriptions
        pVertexInputState->pVertexAttributeDescriptions[i].stride = pCreateInfo->pVertexBindingDescriptions[binding].stride;
        pVertexInputState->pVertexAttributeDescriptions[i].inputRate = pCreateInfo->pVertexBindingDescriptions[binding].inputRate;
    }
}

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        ctsFree(pAllocator, pInstance->pVertexAttributeDescriptions);
    }
}

static void createInputAssemblyState(
    const CtsPipelineInputAssemblyStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineInputAssemblyState* pInputAssemblyState
) {
    if (pCreateInfo == NULL) {
        return;
    }

    pInputAssemblyState->polygonMode = parsePolygonMode(pCreateInfo->polygonMode);
    pInputAssemblyState->primitiveRestartEnable = pCreateInfo->primitiveRestartEnable;
}

static void destroyInputAssemblyState(
    CtsGlPipelineInputAssemblyState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Nothing to do
}

static void createTessellationState(
    const CtsPipelineTessellationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineTessellationState* pTesselationState
) {
    // Not implemented yet
    if (pCreateInfo == NULL) {
        return;
    }
}

static void destroyTessellationState(
    CtsGlPipelineTessellationState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Not implemented yet
}

static void createViewportState(
    const CtsPipelineViewportStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineViewportState* pViewportState
) {
    if (pCreateInfo == NULL) {
        return;
    }

    pViewportState->viewportCount = pCreateInfo->viewportCount;
    pViewportState->pViewports = ctsAllocation(
        pAllocator,
        sizeof(CtsViewport) * pCreateInfo->viewportCount,
        alignof(CtsViewport),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->viewportCount; ++i) {
        pViewportState->pViewports[i].x        = pCreateInfo->pViewports[i].x;
        pViewportState->pViewports[i].y        = pCreateInfo->pViewports[i].y;
        pViewportState->pViewports[i].width    = pCreateInfo->pViewports[i].width;
        pViewportState->pViewports[i].height   = pCreateInfo->pViewports[i].height;
        pViewportState->pViewports[i].minDepth = pCreateInfo->pViewports[i].minDepth;
        pViewportState->pViewports[i].maxDepth = pCreateInfo->pViewports[i].maxDepth;
    }

    pViewportState->scissorCount = pCreateInfo->scissorCount;
    pViewportState->pScissors = ctsAllocation(
        pAllocator,
        sizeof(CtsRect2D) * pCreateInfo->scissorCount,
        alignof(CtsRect2D),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->scissorCount; ++i) {
        pViewportState->pScissors[i].extent.width  = pCreateInfo->pScissors[i].extent.width;
        pViewportState->pScissors[i].extent.height = pCreateInfo->pScissors[i].extent.height;
        pViewportState->pScissors[i].offset.x      = pCreateInfo->pScissors[i].offset.x;
        pViewportState->pScissors[i].offset.y      = pCreateInfo->pScissors[i].offset.y;
    }
}

static void destroyViewportState(
    CtsGlPipelineViewportState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        ctsFree(pAllocator, pInstance->pViewports);
        ctsFree(pAllocator, pInstance->pScissors);
    }
}

static void createRasterizationState(
    const CtsPipelineRasterizationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineRasterizationState* pRasterizationState
) {
    if (pCreateInfo == NULL) {
        return;
    }

    pRasterizationState->depthClampEnable        = pCreateInfo->depthClampEnable;
    pRasterizationState->rasterizerDiscardEnable = pCreateInfo->rasterizerDiscardEnable;
    pRasterizationState->polygonMode             = parsePolygonMode(pCreateInfo->polygonMode);
    pRasterizationState->cullMode                = parseCullMode(pCreateInfo->cullMode);
    pRasterizationState->frontFace               = parseFrontFace(pCreateInfo->frontFace);
    pRasterizationState->depthBiasEnable         = pCreateInfo->depthBiasEnable;
    pRasterizationState->depthBiasConstantFactor = pCreateInfo->depthBiasConstantFactor;
    pRasterizationState->depthBiasClamp          = pCreateInfo->depthBiasClamp;
    pRasterizationState->depthBiasSlopeFactor    = pCreateInfo->depthBiasSlopeFactor;
    pRasterizationState->lineWidth               = pCreateInfo->lineWidth;
}

static void destroyRasterizationState(
    CtsGlPipelineRasterizationState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Nothing to do
}

static void createMultisampleState(
    const CtsPipelineMultisampleStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineMultisampleState* pMultisampleState
) {
    // Not implemented yet
    if (pCreateInfo == NULL) {
        return;
    }
}

static void destroyMultisampleState(
    CtsGlPipelineMultisampleState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Not implemented yet
}

static void createDepthStencilState(
    const CtsPipelineDepthStencilStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineDepthStencilState* pDepthStencilState
) {
    if (pCreateInfo == NULL) {
        return;
    }

    pDepthStencilState->depthTestEnable       = pCreateInfo->depthBoundsTestEnable;
    pDepthStencilState->depthWriteEnable      = pCreateInfo->depthWriteEnable;
    pDepthStencilState->depthCompareOp        = parseCompareOperator(pCreateInfo->depthCompareOp);
    pDepthStencilState->depthBoundsTestEnable = pCreateInfo->depthBoundsTestEnable;
    pDepthStencilState->stencilTestEnable     = pCreateInfo->stencilTestEnable;
    pDepthStencilState->minDepthBounds        = pCreateInfo->minDepthBounds;
    pDepthStencilState->maxDepthBounds        = pCreateInfo->maxDepthBounds;
  
    pDepthStencilState->frontFailOp           = parseStencilAction(pCreateInfo->front.failOp);
    pDepthStencilState->frontPassOp           = parseStencilAction(pCreateInfo->front.passOp);
    pDepthStencilState->frontDepthFailOp      = parseStencilAction(pCreateInfo->front.depthFailOp);
    pDepthStencilState->frontCompareOp        = parseStencilAction(pCreateInfo->front.compareOp);
    pDepthStencilState->frontCompareMask      = pCreateInfo->front.compareMask;
    pDepthStencilState->frontWriteMask        = pCreateInfo->front.writeMask;
    pDepthStencilState->frontReference        = pCreateInfo->front.reference;

    pDepthStencilState->backFailOp            = parseStencilAction(pCreateInfo->back.failOp);
    pDepthStencilState->backPassOp            = parseStencilAction(pCreateInfo->back.passOp);
    pDepthStencilState->backDepthFailOp       = parseStencilAction(pCreateInfo->back.depthFailOp);
    pDepthStencilState->backCompareOp         = parseStencilAction(pCreateInfo->back.compareOp);
    pDepthStencilState->backCompareMask       = pCreateInfo->back.compareMask;
    pDepthStencilState->backWriteMask         = pCreateInfo->back.writeMask;
    pDepthStencilState->backReference         = pCreateInfo->back.reference;
}

static void destroyDepthStencilState(
    CtsGlPipelineDepthStencilState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Nothing to do
}

static void createColorBlendState(
    const CtsPipelineColorBlendStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineColorBlendState* pColorBlendState
) {
    if (pCreateInfo == NULL) {
        return;    
    }

    pColorBlendState->attachmentCount = pCreateInfo->attachmentCount;
    pColorBlendState->pAttachments = ctsAllocation(
        pAllocator,
        sizeof(CtsGlPipelineColorBlendStateAttachment) * pCreateInfo->attachmentCount,
        alignof(CtsGlPipelineColorBlendState),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(pColorBlendState->blendConstants, pCreateInfo->blendConstants, sizeof(pCreateInfo->blendConstants));

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* target = &pColorBlendState->pAttachments[i];
        const CtsPipelineColorBlendAttachmentState* source = &pCreateInfo->pAttachments[i];
        target->blendEnable         = source->blendEnable;
        target->srcColorBlendFactor = parseBlendFunc(source->srcColorBlendFactor);
        target->dstColorBlendFactor = parseBlendFunc(source->dstColorBlendFactor);
        target->colorBlendOp        = parseBlendOperation(source->colorBlendOp);
        target->srcAlphaBlendFactor = parseBlendFunc(source->srcAlphaBlendFactor);
        target->dstAlphaBlendFactor = parseBlendFunc(source->dstAlphaBlendFactor);
        target->alphaBlendOp        = parseBlendOperation(source->alphaBlendOp);
        target->colorWriteMask      = source->colorWriteMask;
    }
}

static void destroyColorBlendState(
    CtsGlPipelineColorBlendState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance->pAttachments != NULL) {
        ctsFree(pAllocator, pInstance->pAttachments);
    }
}

#ifdef __cplusplus
}
#endif