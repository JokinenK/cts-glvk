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

    ctsQueueDispatch(device->queue, &cmd.base);

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

    ctsQueueDispatch(device->queue, &cmd.base);
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
            sizeof(struct CtsPipelineImpl),
            alignof(struct CtsPipelineImpl),
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
                if (device->activeGraphicsPipeline == pipeline->graphics) {
                    device->activeGraphicsPipeline = NULL;
                }

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
    
    createShader(pCreateInfo->stageCount, pCreateInfo->pStages, pAllocator, &graphicsPipeline->shader);
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
    GLuint shaderType;

    for (uint32_t i = 0; i < stageCount; ++i) {
        const CtsPipelineShaderStageCreateInfo* pCreateInfo = &pCreateInfos[i];
        CtsGlShaderStage* stage = &pShader->pStages[i];

        shaderType = parseShaderType(pCreateInfo->stage);
        stage->pName = pCreateInfo->pName;
        stage->handle = glCreateShader(shaderType);
        const char* source = pCreateInfo->module->code;
        const int sourceLen = (const int)pCreateInfo->module->codeSize;

        glShaderSource(stage->handle, 1, &source, &sourceLen);
        glCompileShader(stage->handle);
        glGetShaderiv(stage->handle, GL_COMPILE_STATUS, &success);

        if (success) {
            glAttachShader(pShader->handle, stage->handle);
        }
        else {
            memset(buffer, 0, sizeof(buffer));
            glGetShaderInfoLog(stage->handle, sizeof(buffer), NULL, buffer);
            fprintf(stderr, "Shader compilation failed for stage %d named %s. Reason %s", shaderType, stage->pName, buffer);
        }
    }

    glLinkProgram(pShader->handle);
    glGetProgramiv(pShader->handle, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(pShader->handle, sizeof(buffer), NULL, buffer);
        fprintf(stderr, "Shader linking failed: %s", buffer);
    }
}

static void destroyShader(
    CtsGlShader* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    for (uint32_t i = 0; i < pInstance->stageCount; ++i) {
        glDeleteShader(pInstance->pStages[i].handle);
    }

    glDeleteProgram(pInstance->handle);
    ctsFree(pAllocator, pInstance->pStages);
}

static void createVertexInputState(
    const CtsPipelineVertexInputStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineVertexInputState* pVertexInputState
) {
    if (pCreateInfo == NULL) {
        pVertexInputState->initialized = false;
        return;
    }
    
    glGenVertexArrays(1, &pVertexInputState->vao);
    glBindVertexArray(pVertexInputState->vao);

    pVertexInputState->initialized = true;
    pVertexInputState->vertexBindingDescriptionCount = pCreateInfo->vertexBindingDescriptionCount;
    pVertexInputState->pVertexBindingDescriptions = ctsAllocation(
        pAllocator,
        sizeof(CtsVertexInputBindingDescription) * pCreateInfo->vertexBindingDescriptionCount,
        alignof(CtsVertexInputBindingDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(
        pVertexInputState->pVertexBindingDescriptions,
        pCreateInfo->pVertexBindingDescriptions,
        sizeof(CtsVertexInputBindingDescription) * pCreateInfo->vertexBindingDescriptionCount
    );

    for (uint32_t i = 0; i < pCreateInfo->vertexAttributeDescriptionCount; ++i) {
        const CtsVertexInputAttributeDescription* pVertexAttributeDescription = &pCreateInfo->pVertexAttributeDescriptions[i];
        CtsFormatData formatData = parseFormat(pVertexAttributeDescription->format);

        glVertexAttribFormat(
            pVertexAttributeDescription->location,
            formatData.numComponents,
            formatData.type,
            formatData.normalized,
            pVertexAttributeDescription->offset
        );

        glVertexAttribBinding(pVertexAttributeDescription->location, pVertexAttributeDescription->binding);
        glEnableVertexAttribArray(pVertexAttributeDescription->location);
    }

    glBindVertexArray(0);
}

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pVertexInputState,
    const CtsAllocationCallbacks* pAllocator
) {
    pVertexInputState->initialized = false;
    
    if (pVertexInputState->vao != 0) {
        glDeleteVertexArrays(1, &pVertexInputState->vao);
        pVertexInputState->vao = 0;
    }
    
    if (pVertexInputState->pVertexBindingDescriptions != NULL) {
        ctsFree(pAllocator, pVertexInputState->pVertexBindingDescriptions);
        pVertexInputState->pVertexBindingDescriptions = NULL;
    }
}

static void createInputAssemblyState(
    const CtsPipelineInputAssemblyStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineInputAssemblyState* pInputAssemblyState
) {
    if (pCreateInfo == NULL) {
        pInputAssemblyState->initialized = false;
        return;
    }

    pInputAssemblyState->initialized = true;
    pInputAssemblyState->topology = parsePrimitiveTopology(pCreateInfo->topology);
    pInputAssemblyState->primitiveRestartEnable = pCreateInfo->primitiveRestartEnable;
}

static void destroyInputAssemblyState(
    CtsGlPipelineInputAssemblyState* pInputAssemblyState,
    const CtsAllocationCallbacks* pAllocator
) {
    pInputAssemblyState->initialized = false;
}

static void createTessellationState(
    const CtsPipelineTessellationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineTessellationState* pTesselationState
) {
    if (pCreateInfo == NULL) {
        pTesselationState->initialized = false;
        return;
    }

    pTesselationState->initialized = true;
}

static void destroyTessellationState(
    CtsGlPipelineTessellationState* pTesselationState,
    const CtsAllocationCallbacks* pAllocator
) {
    pTesselationState->initialized = false;
}

static void createViewportState(
    const CtsPipelineViewportStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineViewportState* pViewportState
) {
    if (pCreateInfo == NULL) {
        pViewportState->initialized = false;
        return;
    }

    pViewportState->initialized = true;
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
    CtsGlPipelineViewportState* pViewportState,
    const CtsAllocationCallbacks* pAllocator
) {
    pViewportState->initialized = false;
    
    if (pViewportState->pViewports != NULL) {
        ctsFree(pAllocator, pViewportState->pViewports);
        pViewportState->pViewports = NULL;
    }

    if (pViewportState->pScissors != NULL) {
        ctsFree(pAllocator, pViewportState->pScissors);
        pViewportState->pScissors = NULL;
    }
}

static void createRasterizationState(
    const CtsPipelineRasterizationStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineRasterizationState* pRasterizationState
) {
    if (pCreateInfo == NULL) {
        pRasterizationState->initialized = false;
        return;
    }

    pRasterizationState->initialized = true;
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
    CtsGlPipelineRasterizationState* pRasterizationState,
    const CtsAllocationCallbacks* pAllocator
) {
    pRasterizationState->initialized = false;
}

static void createMultisampleState(
    const CtsPipelineMultisampleStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineMultisampleState* pMultisampleState
) {
    if (pCreateInfo == NULL) {
        pMultisampleState->initialized = false;
        return;
    }

    pMultisampleState->initialized = true;
}

static void destroyMultisampleState(
    CtsGlPipelineMultisampleState* pMultisampleState,
    const CtsAllocationCallbacks* pAllocator
) {
    pMultisampleState->initialized = false;
}

static void createDepthStencilState(
    const CtsPipelineDepthStencilStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineDepthStencilState* pDepthStencilState
) {
    if (pCreateInfo == NULL) {
        pDepthStencilState->initialized = false;
        return;
    }

    pDepthStencilState->initialized           = true;
    pDepthStencilState->depthTestEnable       = pCreateInfo->depthBoundsTestEnable;
    pDepthStencilState->depthWriteEnable      = pCreateInfo->depthWriteEnable;
    pDepthStencilState->depthCompareOp        = parseCompareOp(pCreateInfo->depthCompareOp);
    pDepthStencilState->depthBoundsTestEnable = pCreateInfo->depthBoundsTestEnable;
    pDepthStencilState->stencilTestEnable     = pCreateInfo->stencilTestEnable;
    pDepthStencilState->minDepthBounds        = pCreateInfo->minDepthBounds;
    pDepthStencilState->maxDepthBounds        = pCreateInfo->maxDepthBounds;
  
    pDepthStencilState->frontFailOp           = parseStencilOp(pCreateInfo->front.failOp);
    pDepthStencilState->frontPassOp           = parseStencilOp(pCreateInfo->front.passOp);
    pDepthStencilState->frontDepthFailOp      = parseStencilOp(pCreateInfo->front.depthFailOp);
    pDepthStencilState->frontCompareOp        = parseStencilOp(pCreateInfo->front.compareOp);
    pDepthStencilState->frontCompareMask      = pCreateInfo->front.compareMask;
    pDepthStencilState->frontWriteMask        = pCreateInfo->front.writeMask;
    pDepthStencilState->frontReference        = pCreateInfo->front.reference;

    pDepthStencilState->backFailOp            = parseStencilOp(pCreateInfo->back.failOp);
    pDepthStencilState->backPassOp            = parseStencilOp(pCreateInfo->back.passOp);
    pDepthStencilState->backDepthFailOp       = parseStencilOp(pCreateInfo->back.depthFailOp);
    pDepthStencilState->backCompareOp         = parseStencilOp(pCreateInfo->back.compareOp);
    pDepthStencilState->backCompareMask       = pCreateInfo->back.compareMask;
    pDepthStencilState->backWriteMask         = pCreateInfo->back.writeMask;
    pDepthStencilState->backReference         = pCreateInfo->back.reference;
}

static void destroyDepthStencilState(
    CtsGlPipelineDepthStencilState* pDepthStencilState,
    const CtsAllocationCallbacks* pAllocator
) {
    pDepthStencilState->initialized = false;
}

static void createColorBlendState(
    const CtsPipelineColorBlendStateCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlPipelineColorBlendState* pColorBlendState
) {
    if (pCreateInfo == NULL) {
        pColorBlendState->initialized = false;
        return;    
    }

    pColorBlendState->initialized = true;
    pColorBlendState->attachmentCount = pCreateInfo->attachmentCount;
    pColorBlendState->pAttachments = ctsAllocation(
        pAllocator,
        sizeof(CtsGlPipelineColorBlendStateAttachment) * pCreateInfo->attachmentCount,
        alignof(CtsGlPipelineColorBlendState),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    pColorBlendState->logicOpEnable = pCreateInfo->logicOpEnable;
    pColorBlendState->logicOp = parseLogicOp(pCreateInfo->logicOp);
    memcpy(pColorBlendState->blendConstants, pCreateInfo->blendConstants, sizeof(pCreateInfo->blendConstants));

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* target = &pColorBlendState->pAttachments[i];
        const CtsPipelineColorBlendAttachmentState* source = &pCreateInfo->pAttachments[i];
        target->blendEnable         = source->blendEnable;
        target->srcColorBlendFactor = parseBlendFactor(source->srcColorBlendFactor);
        target->dstColorBlendFactor = parseBlendFactor(source->dstColorBlendFactor);
        target->colorBlendOp        = parseBlendOperation(source->colorBlendOp);
        target->srcAlphaBlendFactor = parseBlendFactor(source->srcAlphaBlendFactor);
        target->dstAlphaBlendFactor = parseBlendFactor(source->dstAlphaBlendFactor);
        target->alphaBlendOp        = parseBlendOperation(source->alphaBlendOp);
        target->colorWriteMask      = source->colorWriteMask;
    }
}

static void destroyColorBlendState(
    CtsGlPipelineColorBlendState* pColorBlendState,
    const CtsAllocationCallbacks* pAllocator
) {
    pColorBlendState->initialized = false;

    if (pColorBlendState->pAttachments != NULL) {
        ctsFree(pAllocator, pColorBlendState->pAttachments);
        pColorBlendState->pAttachments = NULL;
    }
}

#ifdef __cplusplus
}
#endif