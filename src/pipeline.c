#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <spirv_cross_c.h>
#include "cts/pipeline.h"
#include "cts/type_mapper.h"
#include "cts/commands.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/pipeline_private.h"
#include "cts/shader_module_private.h"
#include "cts/queue_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool hasFlag(
    VkFlags flags,
    VkFlags flag
);

static void spirvCallback(
    void* userdata, 
    const char* error
);

static CtsGlGraphicsPipeline* createGraphicsPipeline(
    const VkGraphicsPipelineCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator
);

static void destroyGraphicsPipeline(
    CtsGlGraphicsPipeline* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createShader(
    uint32_t stageCount,
    const VkPipelineShaderStageCreateInfo* stages,
    const VkAllocationCallbacks* pAllocator,
    CtsGlShader* pShader
);

static void destroyShader(
    CtsGlShader* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createVertexInputState(
    const VkPipelineVertexInputStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineVertexInputState* pVertexInputState
);

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createInputAssemblyState(
    const VkPipelineInputAssemblyStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineInputAssemblyState* pInputAssemblyState
);

static void destroyInputAssemblyState(
    CtsGlPipelineInputAssemblyState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createTessellationState(
    const VkPipelineTessellationStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineTessellationState* pTesselationState
);

static void destroyTessellationState(
    CtsGlPipelineTessellationState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createViewportState(
    const VkPipelineViewportStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineViewportState* pViewportState
);

static void destroyViewportState(
    CtsGlPipelineViewportState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createRasterizationState(
    const VkPipelineRasterizationStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineRasterizationState* pRasterizationState
);

static void destroyRasterizationState(
    CtsGlPipelineRasterizationState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createMultisampleState(
    const VkPipelineMultisampleStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineMultisampleState* pMultisampleState
);

static void destroyMultisampleState(
    CtsGlPipelineMultisampleState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createDepthStencilState(
    const VkPipelineDepthStencilStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineDepthStencilState* pDepthStencilState
);

static void destroyDepthStencilState(
    CtsGlPipelineDepthStencilState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

static void createColorBlendState(
    const VkPipelineColorBlendStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineColorBlendState* pColorBlendState
);

static void destroyColorBlendState(
    CtsGlPipelineColorBlendState* pInstance,
    const VkAllocationCallbacks* pAllocator
);

VkResult VKAPI_CALL ctsCreateGraphicsPipelines(
    VkDevice deviceHandle,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateGraphicsPipelines cmd;
    cmd.base.type = CTS_COMMAND_CREATE_GRAPHICS_PIPELINES;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pipelineCache = pipelineCache;
    cmd.createInfoCount = createInfoCount;
    cmd.pCreateInfos = pCreateInfos;
    cmd.pAllocator = pAllocator;
    cmd.pPipelines = pPipelines;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void VKAPI_CALL ctsDestroyPipeline(
    VkDevice deviceHandle,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroyPipeline cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_PIPELINE;
    cmd.base.pNext = NULL;
    cmd.device = deviceHandle;
    cmd.pipeline = pipeline;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsCreateGraphicsPipelinesImpl(
    VkDevice deviceHandle,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines
) {
    (void) deviceHandle;
    (void) pipelineCache;

    VkResult result = VK_SUCCESS;
    uint32_t i = 0;

    for (; i < createInfoCount; ++i) {
        struct CtsPipeline* pipeline = ctsAllocation(
            pAllocator,
            sizeof(struct CtsPipeline),
            alignof(struct CtsPipeline),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (pipeline == NULL) {
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
			break;
        }

        set_loader_magic_value(pipeline);

        pipeline->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        pipeline->graphics = createGraphicsPipeline(&pCreateInfos[i], pAllocator);

        pPipelines[i] = CtsPipelineToHandle(pipeline);
    }

    for (; i < createInfoCount; ++i) {
        pPipelines[i] = NULL;
    }

    return result;
}

void ctsDestroyPipelineImpl(
    VkDevice deviceHandle,
    VkPipeline pipelineHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsPipeline* pipeline = CtsPipelineFromHandle(pipelineHandle);

    if (pipeline != NULL) {
        set_loader_magic_value(pipeline);

        switch (pipeline->bindPoint) {
            case VK_PIPELINE_BIND_POINT_GRAPHICS: {
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
    VkFlags flags,
    VkFlags flag
) {
    return ((flags & flag) == flag);
}

static void spirvCallback(void* userdata, const char* error)
{
    fprintf(stderr, error);
}

static CtsGlGraphicsPipeline* createGraphicsPipeline(
    const VkGraphicsPipelineCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator
) {
    if (pCreateInfo == NULL) {
        return NULL;
    }

    CtsGlGraphicsPipeline* graphicsPipeline = ctsAllocation(
        pAllocator,
        sizeof(CtsGlGraphicsPipeline),
        alignof(CtsGlGraphicsPipeline),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
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
    const VkAllocationCallbacks* pAllocator
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
    const VkPipelineShaderStageCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
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
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    GLint success;
    GLchar buffer[512];
    GLuint shaderType;

    for (uint32_t i = 0; i < stageCount; ++i) {
        const VkPipelineShaderStageCreateInfo* pCreateInfo = &pCreateInfos[i];
        CtsGlShaderStage* stage = &pShader->pStages[i];

        shaderType = parseShaderType(pCreateInfo->stage);
        stage->pName = pCreateInfo->pName;
        stage->handle = glCreateShader(shaderType);

        struct CtsShaderModule* shaderModule = CtsShaderModuleFromHandle(pCreateInfo->module);

        const char* source = NULL;
        spvc_context context = NULL;
        spvc_parsed_ir ir = NULL;
        spvc_compiler compiler = NULL;
        spvc_compiler_options options = NULL;

        spvc_context_create(&context);
        spvc_context_set_error_callback(context, spirvCallback, NULL);
        spvc_context_parse_spirv(context, (const SpvId*) shaderModule->code, shaderModule->codeSize / sizeof(SpvId), &ir);
        spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);
        spvc_compiler_create_compiler_options(compiler, &options);
        spvc_compiler_install_compiler_options(compiler, options);
        spvc_compiler_compile(compiler, &source);
        const int sourceLen = strlen(source);

        glShaderSource(stage->handle, 1, &source, &sourceLen);
        glCompileShader(stage->handle);
        glGetShaderiv(stage->handle, GL_COMPILE_STATUS, &success);

        spvc_context_destroy(context);

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
    const VkAllocationCallbacks* pAllocator
) {
    for (uint32_t i = 0; i < pInstance->stageCount; ++i) {
        glDeleteShader(pInstance->pStages[i].handle);
    } 

    glDeleteProgram(pInstance->handle);
    ctsFree(pAllocator, pInstance->pStages);
}
 
static void createVertexInputState(
    const VkPipelineVertexInputStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
        sizeof(VkVertexInputBindingDescription) * pCreateInfo->vertexBindingDescriptionCount,
        alignof(VkVertexInputBindingDescription),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(
        pVertexInputState->pVertexBindingDescriptions,
        pCreateInfo->pVertexBindingDescriptions,
        sizeof(VkVertexInputBindingDescription) * pCreateInfo->vertexBindingDescriptionCount
    );

    for (uint32_t i = 0; i < pCreateInfo->vertexAttributeDescriptionCount; ++i) {
        const VkVertexInputAttributeDescription* pVertexAttributeDescription = &pCreateInfo->pVertexAttributeDescriptions[i];
        const CtsFormatData* formatData = parseFormat(pVertexAttributeDescription->format);

        glVertexAttribFormat(
            pVertexAttributeDescription->location,
            formatData->numComponents,
            formatData->type,
            formatData->normalized,
            pVertexAttributeDescription->offset
        );

        glVertexAttribBinding(pVertexAttributeDescription->location, pVertexAttributeDescription->binding);
        glEnableVertexAttribArray(pVertexAttributeDescription->location);
    }

    glBindVertexArray(0);
}

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pVertexInputState,
    const VkAllocationCallbacks* pAllocator
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
    const VkPipelineInputAssemblyStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
    const VkAllocationCallbacks* pAllocator
) {
    pInputAssemblyState->initialized = false;
}

static void createTessellationState(
    const VkPipelineTessellationStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
    const VkAllocationCallbacks* pAllocator
) {
    pTesselationState->initialized = false;
}

static void createViewportState(
    const VkPipelineViewportStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
        sizeof(VkViewport) * pCreateInfo->viewportCount,
        alignof(VkViewport),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
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
        sizeof(VkRect2D) * pCreateInfo->scissorCount,
        alignof(VkRect2D),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
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
    const VkAllocationCallbacks* pAllocator
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
    const VkPipelineRasterizationStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
    const VkAllocationCallbacks* pAllocator
) {
    pRasterizationState->initialized = false;
}

static void createMultisampleState(
    const VkPipelineMultisampleStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineMultisampleState* pMultisampleState
) {
    if (pCreateInfo == NULL) {
        pMultisampleState->initialized = false;
        return;
    }

    pMultisampleState->sampleShadingEnable = pCreateInfo->sampleShadingEnable;
    pMultisampleState->minSampleShading = pCreateInfo->minSampleShading;

    pMultisampleState->initialized = true;
}

static void destroyMultisampleState(
    CtsGlPipelineMultisampleState* pMultisampleState,
    const VkAllocationCallbacks* pAllocator
) {
    pMultisampleState->initialized = false;
}

static void createDepthStencilState(
    const VkPipelineDepthStencilStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGlPipelineDepthStencilState* pDepthStencilState
) {
    if (pCreateInfo == NULL) {
        pDepthStencilState->initialized = false;
        return;
    }

    pDepthStencilState->initialized           = true;
    pDepthStencilState->depthTestEnable       = pCreateInfo->depthTestEnable;
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
    const VkAllocationCallbacks* pAllocator
) {
    pDepthStencilState->initialized = false;
}

static void createColorBlendState(
    const VkPipelineColorBlendStateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
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
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    pColorBlendState->logicOpEnable = pCreateInfo->logicOpEnable;
    pColorBlendState->logicOp = parseLogicOp(pCreateInfo->logicOp);
    memcpy(pColorBlendState->blendConstants, pCreateInfo->blendConstants, sizeof(pCreateInfo->blendConstants));

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* target = &pColorBlendState->pAttachments[i];
        const VkPipelineColorBlendAttachmentState* source = &pCreateInfo->pAttachments[i];
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
    const VkAllocationCallbacks* pAllocator
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