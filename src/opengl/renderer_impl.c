#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <glad/glad.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/bump_allocator_proxy.h>
#include <cts/bump_allocator.h>
#include <cts/renderer_impl.h>
#include <cts/renderer_proxy.h>
#include <cts/type_mapper.h>
#include <cts/os/semaphore.h>
#include <cts/os/thread.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/gl_enums.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_rasterization_state_changes.h>
#include <cts/typedefs/gl_depth_stencil_state_changes.h>
#include <cts/typedefs/gl_color_blend_state_changes.h>
#include <private/buffer_private.h>
#include <private/command_buffer_private.h>
#include <private/command_pool_private.h>
#include <private/descriptor_pool_private.h>
#include <private/descriptor_set_layout_private.h>
#include <private/descriptor_set_private.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/fence_private.h>
#include <private/pipeline_layout_private.h>
#include <private/pipeline_private.h>
#include <private/shader_private.h>
#include <private/image_view_private.h>
#include <private/image_private.h>
#include <private/instance_private.h>
#include <private/surface_private.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma region HelperMethodDeclarations

static bool hasFlag(CtsFlags flags, CtsFlags flag);

static CtsGlGraphicsPipeline* createGraphicsPipeline(
    const CtsGraphicsPipelineCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator
);

static void destroyGraphicsPipeline(
    CtsGlGraphicsPipeline* pInstance,
    const CtsAllocationCallbacks* pAllocator
);

static void createShader(
    uint32_t pStageCount,
    const CtsPipelineShaderStageCreateInfo* pStages,
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

static void bindDynamicState(CtsDevice pDevice, CtsFlags pState);
static void bindVertexInputState(CtsDevice pDevice, CtsGlPipelineVertexInputState* pState);
static void bindInputAssemblyState(CtsDevice pDevice, CtsGlPipelineInputAssemblyState* pState);
static void bindTessellationState(CtsDevice pDevice, CtsGlPipelineTessellationState* pState);
static void bindViewportState(CtsDevice pDevice, CtsGlPipelineViewportState* pState);
static void bindRasterizationState(CtsDevice pDevice, CtsGlPipelineRasterizationState* pState);
static void bindMultisampleState(CtsDevice pDevice, CtsGlPipelineMultisampleState* pState);
static void bindDepthStencilState(CtsDevice pDevice, CtsGlPipelineDepthStencilState* pState);
static void bindColorBlendState(CtsDevice pDevice, CtsGlPipelineColorBlendState* pState);

#pragma endregion

CtsResult ctsAllocateMemoryImpl(
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    (void) pDevice;
    
    CtsDeviceMemory memory = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDeviceMemory),
        alignof(struct CtsDeviceMemory),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (memory == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    memory->size = pAllocateInfo->allocationSize;

    glGenBuffers(1, &memory->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
    glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)(pAllocateInfo->allocationSize), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    *pMemory = memory;
    return CTS_SUCCESS;
}

CtsResult ctsMapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
) {
    if (pMemory == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    (void) pFlags;
    GLuint flags = GL_MAP_READ_BIT  | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBindBuffer(GL_COPY_WRITE_BUFFER, pMemory->handle);
    void* mappedMemory = glMapBufferRange(GL_COPY_WRITE_BUFFER, pOffset, pSize, flags);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    if (mappedMemory == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    *pData = mappedMemory;
    return CTS_SUCCESS;
}

void ctsUnmapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
) {
    if (pMemory == NULL) {
        return;
    }

    glBindBuffer(GL_COPY_WRITE_BUFFER, pMemory->handle);
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsFreeMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pMemory != NULL) {
        glDeleteBuffers(1, &pMemory->handle);
        ctsFree(pAllocator, pMemory);
    }
}

CtsResult ctsCreateCommandPoolImpl(
    CtsDevice pDevice,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
) {
    CtsCommandPool commandPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsCommandPool),
        alignof(struct CtsCommandPool),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (commandPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsBumpAllocatorCreateInfo bumpAllocatorCreateInfo;
    bumpAllocatorCreateInfo.allocator = pAllocator;
    bumpAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateBumpAllocator(&commandPool->bumpAllocatorInstance, &bumpAllocatorCreateInfo);
    ctsInitBumpAllocatorProxy(commandPool->bumpAllocatorInstance, &commandPool->bumpAllocator);

    *pCommandPool = commandPool;
    return CTS_SUCCESS;
}

void ctsDestroyCommandPoolImpl(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pCommandPool != NULL) {
        ctsDestroyBumpAllocator(pCommandPool->bumpAllocatorInstance);
        ctsFree(pAllocator, pCommandPool);
    }
}

CtsResult ctsCreateDescriptorPoolImpl(
    CtsDevice pDevice,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
) {
    (void) pDevice;

    CtsDescriptorPool descriptorPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorPool),
        alignof(struct CtsDescriptorPool),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsBumpAllocatorCreateInfo bumpAllocatorCreateInfo;
    bumpAllocatorCreateInfo.allocator = pAllocator;
    bumpAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateBumpAllocator(&descriptorPool->bumpAllocatorInstance, &bumpAllocatorCreateInfo);
    ctsInitBumpAllocatorProxy(descriptorPool->bumpAllocatorInstance, &descriptorPool->bumpAllocator);

    *pDescriptorPool = descriptorPool;
    return CTS_SUCCESS;
}

void ctsDestroyDescriptorPoolImpl(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pDescriptorPool != NULL) {
        ctsFree(pAllocator, pDescriptorPool);
    }
}

CtsResult ctsCreateDescriptorSetLayoutImpl(
    CtsDevice pDevice,
    const CtsDescriptorSetLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorSetLayout* pDescriptorSetLayout
) {
    (void) pDevice;

    CtsDescriptorSetLayout descriptorSetLayout = ctsAllocation(
        pAllocator,
        sizeof(CtsDescriptorSetLayout),
        alignof(CtsDescriptorSetLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorSetLayout == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    descriptorSetLayout->bindingCount = pCreateInfo->bindingCount;
    descriptorSetLayout->bindings = ctsAllocation(
        pAllocator,
        sizeof(CtsDescriptorSetLayoutBinding) * descriptorSetLayout->bindingCount,
        alignof(CtsDescriptorSetLayoutBinding),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < descriptorSetLayout->bindingCount; ++i) {
        CtsDescriptorSetLayoutBinding* target = &descriptorSetLayout->bindings[i];
        const CtsDescriptorSetLayoutBinding* source = &pCreateInfo->bindings[i];

        target->binding = source->binding;
        target->descriptorType = source->descriptorType;
        target->descriptorCount = source->descriptorCount;
        target->stageFlags = source->stageFlags;
        target->immutableSamplers = source->immutableSamplers;
    }
    
    *pDescriptorSetLayout = descriptorSetLayout;
    return CTS_SUCCESS;
}

void ctsDestroyDescriptorSetLayoutImpl(
    CtsDevice pDevice,
    CtsDescriptorSetLayout pDescriptorSetLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pDescriptorSetLayout != NULL) {
        ctsFree(pAllocator, pDescriptorSetLayout->bindings);
        ctsFree(pAllocator, pDescriptorSetLayout);
    }
}

CtsResult ctsCreatePipelineLayoutImpl(
    CtsDevice pDevice,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
) {
    (void) pDevice;

    CtsPipelineLayout pipelineLayout = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPipelineLayout),
        alignof(struct CtsPipelineLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (pipelineLayout == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    pipelineLayout->setLayoutCount = pCreateInfo->setLayoutCount;
    pipelineLayout->setLayouts = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorSetLayout) * pipelineLayout->setLayoutCount,
        alignof(struct CtsDescriptorSetLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->setLayoutCount; ++i) {
        pipelineLayout->setLayouts[i] = pCreateInfo->setLayouts[i];
    }

    pipelineLayout->pushConstantRangeCount = pCreateInfo->pushConstantRangeCount;
    pipelineLayout->pushConstantRanges = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPushConstantRange) * pipelineLayout->pushConstantRangeCount,
        alignof(struct CtsPushConstantRange),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->pushConstantRangeCount; ++i) {
        pipelineLayout->pushConstantRanges[i] = pCreateInfo->pushConstantRanges[i];
    }

    *pPipelineLayout = pipelineLayout;
    return CTS_SUCCESS;
}

void ctsDestroyPipelineLayoutImpl(
    CtsDevice pDevice,
    CtsPipelineLayout pPipelineLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pPipelineLayout != NULL) {
        ctsFree(pAllocator, pPipelineLayout->setLayouts);
        ctsFree(pAllocator, pPipelineLayout->pushConstantRanges);
        ctsFree(pAllocator, pPipelineLayout);
    }
}

CtsResult ctsCreateGraphicsPipelinesImpl(
    CtsDevice pDevice,
    CtsPipelineCache pPipelineCache,
    uint32_t pCreateInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
) {
    (void) pDevice;
    (void) pPipelineCache;

    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; i < pCreateInfoCount; ++i) {
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

    for (; i < pCreateInfoCount; ++i) {
        pPipelines[i] = NULL;
    }

    return result;
}

void ctsDestroyPipelineImpl(
    CtsDevice pDevice,
    CtsPipeline pPipeline,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pPipeline != NULL) {
        switch (pPipeline->bindPoint) {
            case CTS_PIPELINE_BIND_POINT_GRAPHICS: {
                destroyGraphicsPipeline(pPipeline->graphics, pAllocator);
            } break;
        }

        ctsFree(pAllocator, pPipeline);
    }
}

CtsResult ctsCreateBufferImpl(
    CtsDevice pDevice,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
) {
    (void) pDevice;
    
    CtsBuffer buffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsBuffer),
        alignof(struct CtsBuffer),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (buffer == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }
        
    //GLenum usage = mapBufferUsage(pCreateInfo->usage);
    buffer->handle = GL_INVALID_VALUE;
    buffer->type = parseBufferType(pCreateInfo->type);
    buffer->size = (GLsizei) pCreateInfo->size;
    buffer->offset = 0;

    *pBuffer = buffer;
    return CTS_SUCCESS;
}

CtsResult ctsBindBufferMemoryImpl(
    CtsDevice pDevice,
    CtsBuffer pBuffer, 
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset
) {
    if (pBuffer->size > pMemory->size - pOffset) {
        return CTS_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    pBuffer->handle = pMemory->handle;
    pBuffer->offset = (GLsizei)pOffset;
    return CTS_SUCCESS;
}

void ctsDestroyBufferImpl(
    CtsDevice pDevice,
    CtsBuffer pBuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pBuffer != NULL) {
        ctsFree(pAllocator, pBuffer);
    }
}

CtsResult ctsCreateShaderModuleImpl(
    CtsDevice pDevice,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
) {
    (void) pDevice;

    CtsShaderModule shaderModule = ctsAllocation(
        pAllocator,
        sizeof(struct CtsShaderModule),
        alignof(struct CtsShaderModule),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (shaderModule == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    shaderModule->codeSize = (GLint)pCreateInfo->codeSize;
    shaderModule->code = ctsAllocation(
        pAllocator,
        sizeof(char) * pCreateInfo->codeSize,
        alignof(char),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(shaderModule->code, pCreateInfo->code, pCreateInfo->codeSize);
    *pShaderModule = shaderModule;
    return CTS_SUCCESS;
}

void ctsDestroyShaderModuleImpl(
    CtsDevice pDevice,
    CtsShaderModule pShaderModule,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pShaderModule != NULL) {
        ctsFree(pAllocator, pShaderModule->code);
        ctsFree(pAllocator, pShaderModule);
    }
}

CtsResult ctsCreateFenceImpl(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    (void) pDevice;

    CtsFence fence = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFence),
        alignof(struct CtsFence),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (fence == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    fence->flags = 0;
    *pFence = fence;
    return CTS_SUCCESS;
}

void ctsDestroyFenceImpl(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pFence != NULL) {
        if (glIsSync(pFence->sync)) {
            glDeleteSync(pFence->sync);
        }

        ctsFree(pAllocator, pFence);
    }
}

CtsResult ctsAllocateCommandBuffersImpl(
    CtsDevice pDevice,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
) {
    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; pAllocateInfo->commandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = ctsAllocation(
            &pAllocateInfo->commandPool->bumpAllocator,
            sizeof(struct CtsCommandBuffer),
            alignof(struct CtsCommandBuffer),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (commandBuffer == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        commandBuffer->pool = pAllocateInfo->commandPool;
        commandBuffer->level = pAllocateInfo->level;
        pCommandBuffers[i] = commandBuffer;
    }

    for (; pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = NULL;
    }

    return result;
}

void ctsFreeCommandBuffersImpl(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    for (uint32_t i = 0; pCommandBufferCount; ++i) {
        ctsFree(
            &pCommandPool->bumpAllocator,
            pCommandBuffers[i]
        );
    }
}

CtsResult ctsAllocateDescriptorSetsImpl(
    CtsDevice pDevice,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
) {
    (void) pDevice;

    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = ctsAllocation(
            &pAllocateInfo->descriptorPool->bumpAllocator,
            sizeof(struct CtsDescriptorSet),
            alignof(struct CtsDescriptorSet),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (descriptorSet == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        const CtsDescriptorSetLayout setLayout = pAllocateInfo->setLayouts[i];

        descriptorSet->bindingCount = setLayout->bindingCount;
        descriptorSet->bindings = ctsAllocation(
            &pAllocateInfo->descriptorPool->bumpAllocator,
            sizeof(CtsGlDescriptorSetLayoutBinding) * descriptorSet->bindingCount,
            alignof(CtsGlDescriptorSetLayoutBinding),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        for (uint32_t j = 0; j < descriptorSet->bindingCount; ++j) {
            CtsDescriptorSetLayoutBinding* source = &setLayout->bindings[j];
            CtsGlDescriptorSetLayoutBinding* target = &descriptorSet->bindings[j];
            
            target->stageFlags = source->stageFlags;
            target->binding = source->binding;
            target->descriptorCount = source->descriptorCount;
            target->descriptorType = source->descriptorType;
            target->type = parseDescriptorType(source->descriptorType);
            target->imageInfo = NULL;
            target->bufferInfo = NULL;
            target->texelBufferView = NULL;

            if (target->type == CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO) {
                target->imageInfo = ctsAllocation(
                    &pAllocateInfo->descriptorPool->bumpAllocator,
                    sizeof(CtsDescriptorImageInfo) * target->descriptorCount,
                    alignof(CtsDescriptorImageInfo),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            } else if (target->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                target->bufferInfo = ctsAllocation(
                    &pAllocateInfo->descriptorPool->bumpAllocator,
                    sizeof(CtsDescriptorBufferInfo) * target->descriptorCount,
                    alignof(CtsDescriptorBufferInfo),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            } else if (target->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW) {
                target->texelBufferView = ctsAllocation(
                    &pAllocateInfo->descriptorPool->bumpAllocator,
                    sizeof(CtsBufferView) * target->descriptorCount,
                    alignof(CtsBufferView),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            }
        }

        pDescriptorSets[i] = descriptorSet;
    }

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        pDescriptorSets[i] = NULL;
    }

    return result;
}

void ctsUpdateDescriptorSetsImpl(
    CtsDevice pDevice,
    uint32_t pDescriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t pDescriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
) {
    (void) pDevice;

    for (uint32_t i = 0; i < pDescriptorWriteCount; ++i) {
        const CtsWriteDescriptorSet* source = &pDescriptorWrites[i];
        CtsDescriptorSet target = source->dstSet;
        CtsGlDescriptorSetLayoutBinding* binding = &target->bindings[source->dstBinding];

        if (false) {
            // Dynamic type is not supported as for now
            binding->type = parseDescriptorType(source->descriptorType);
        }

        for (uint32_t j = 0; j < binding->descriptorCount; ++j) {
            uint32_t idx = source->dstArrayElement + j;

            if (binding->type == CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO) {
                binding->imageInfo[idx] = source->imageInfo[j];
            } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                binding->bufferInfo[idx] = source->bufferInfo[j];
            } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW) {
                binding->texelBufferView[idx] = source->texelBufferView[j];
            }
        }
    }

    for (uint32_t i = 0; i < pDescriptorCopyCount; ++i) {
        // TODO: Handle copy case
        const CtsCopyDescriptorSet* descriptorSet = &pDescriptorCopies[i];
    }
}

CtsResult ctsFreeDescriptorSetsImpl(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
) {
    for (uint32_t i = 0; pDescriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = pDescriptorSets[i];

        for (uint32_t j = 0; j < descriptorSet->bindingCount; ++j) {
            CtsGlDescriptorSetLayoutBinding* binding = &descriptorSet->bindings[j];

            if (binding->imageInfo != NULL) {
                ctsFree(&pDescriptorPool->bumpAllocator, binding->imageInfo);
            }

            if (binding->bufferInfo != NULL) {
                ctsFree(&pDescriptorPool->bumpAllocator, binding->bufferInfo);
            }

            if (binding->texelBufferView != NULL) {
                ctsFree(&pDescriptorPool->bumpAllocator, binding->texelBufferView);
            }
        }

        ctsFree(&pDescriptorPool->bumpAllocator, descriptorSet->bindings);
        ctsFree(&pDescriptorPool->bumpAllocator, pDescriptorSets[i]);
    }

    return CTS_SUCCESS;
}

CtsResult ctsCreateImageViewImpl(
    CtsDevice pDevice,
    const CtsImageViewCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImageView* pImageView
) {
    (void) pDevice;

    CtsImageView imageView = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImageView),
        alignof(struct CtsImageView),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (imageView == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsFormatPair formatPair = parseFormatPair(pCreateInfo->format);
    (void) pCreateInfo->viewType;
    (void) pCreateInfo->components;

    glGenTextures(1, &imageView->handle);
    glTextureView(
        imageView->handle,
        pCreateInfo->image->target,
        pCreateInfo->image->handle,
        formatPair.internalFormat,
        pCreateInfo->subresourceRange.baseMipLevel,
        pCreateInfo->subresourceRange.levelCount,
        pCreateInfo->subresourceRange.baseArrayLayer,
        pCreateInfo->subresourceRange.layerCount
    );

    *pImageView = imageView;
    return CTS_SUCCESS;
}

void ctsDestroyImageViewImpl(
    CtsDevice pDevice,
    CtsImageView pImageView,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pImageView != NULL) {
        glDeleteTextures(1, &pImageView->handle);
        ctsFree(pAllocator, pImageView);
    }
}

CtsResult ctsCreateImageImpl(
    CtsDevice pDevice,
    const CtsImageCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsImage* pImage
) {
    (void) pDevice;

    CtsImage image = ctsAllocation(
        pAllocator,
        sizeof(struct CtsImage),
        alignof(struct CtsImage),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (image == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    const CtsImageCreateInfo* createInfo = pCreateInfo;

    (void) createInfo->mipLevels;
    (void) createInfo->arrayLayers;
    (void) createInfo->samples;
    //(void) createInfo->tiling;
    (void) createInfo->usage;
    //(void) createInfo->sharingMode;
    (void) createInfo->queueFamilyIndexCount;
    (void) createInfo->queueFamilyIndices;
    //(void) createInfo->initialLayout;

    CtsFormatPair formatPair = parseFormatPair(createInfo->imageFormat);

    glGenTextures(1, &image->handle);
    image->imageType = createInfo->imageType;
    image->target = parseImageType(createInfo->imageType);
    image->internalFormat = formatPair.internalFormat;
    glBindTexture(image->target, image->handle);

    // TODO: Support mip levels

    if (image->imageType == CTS_IMAGE_TYPE_1D) {
        glTexImage1D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            0,
            formatPair.format,
            GL_RGBA,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_2D) {
        glTexImage2D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            createInfo->extent.height,
            0,
            formatPair.format,
            GL_RGBA,
            NULL
        );
    } else if (image->imageType == CTS_IMAGE_TYPE_3D) {
        glTexImage3D(
            image->target,
            0,
            image->internalFormat,
            createInfo->extent.width,
            createInfo->extent.height,
            createInfo->extent.depth,
            0,
            formatPair.format,
            GL_RGBA,
            NULL
        );
    }

    glBindTexture(image->target, 0);

    *pImage = image;
    return CTS_SUCCESS;
}

void ctsDestroyImageImpl(
    CtsDevice pDevice,
    CtsImage pImage,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pImage != NULL) {
        glDeleteTextures(1, &pImage->handle);
        ctsFree(pAllocator, pImage);
    }
}

CtsResult ctsBeginCommandBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
) {
}

void ctsResetCommandBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandBufferResetFlags pFlags
) {
}

void ctsEndCommandBufferImpl(
    CtsCommandBuffer pCommandBuffer
) {
}

CtsResult ctsQueueSubmitImpl(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
) {
    if (glIsSync(pFence->sync)) {
    }

    for (uint32_t i = 0; i < pSubmitCount; ++i) {
        // TODO: Submit queue
        //pSubmits[i].
    }
}

void ctsCmdBeginQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
) {
}

void ctsCmdBeginRenderPassImpl(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* renderPassBegin,
    CtsSubpassContents contents
) {
}

void ctsCmdBindDescriptorSetsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipelineLayout pPipelineLayout,
    uint32_t pFirstSet,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    (void) pPipelineBindPoint;
    (void) pPipelineLayout;
    (void) dynamicOffsetCount;
    (void) pDynamicOffsets;

    for (uint32_t i = pFirstSet; i < pDescriptorSetCount; ++i) {
        const CtsDescriptorSet descriptorSet = pDescriptorSets[i];

        for (uint32_t j = 0; j < descriptorSet->bindingCount; ++j) {
            CtsGlDescriptorSetLayoutBinding* binding = &descriptorSet->bindings[j];

            for (uint32_t k = 0; k < binding->descriptorCount; ++k) {
                if (binding->type == CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO) {
                    CtsDescriptorImageInfo* imageInfo = &binding->imageInfo[k];
                    CtsImageView imageView = imageInfo->imageView;
                    
                    glActiveTexture(GL_TEXTURE0 + binding->binding + k);
                    glBindTexture(imageView->target, imageView->handle);

                    if (imageView->activeSampler != imageInfo->sampler) {
                        if (imageInfo->sampler != NULL) {
                            // TODO: Read from actual sampler object
                            glTexParameteri(imageView->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(imageView->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
                            glTexParameteri(imageView->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            glTexParameteri(imageView->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        }

                        imageView->activeSampler = imageInfo->sampler;
                    }
                } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                    CtsDescriptorBufferInfo* bufferInfo = &binding->bufferInfo[k];
                    CtsBuffer buffer = bufferInfo->buffer;

                    GLsizei offset = buffer->offset + bufferInfo->offset;
                    GLsizei size = (bufferInfo->range > buffer->size)
                        ? buffer->size
                        : bufferInfo->range;

                    glBindBuffer(buffer->type, buffer->handle);
                    glBindBufferRange(buffer->type, binding->binding + k, buffer->handle, offset, size);
                } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW) {
                    // TODO: Not implemented yet
                    CtsBufferView texelBufferView = binding->texelBufferView[k];
                }
            }
        }
    }
}

void ctsCmdBindIndexBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    CtsIndexType pIndexType
) {
    CtsDevice device = pCommandBuffer->device;

    device->activeIndexBuffer = pBuffer;
    glBindBuffer(pBuffer->type, pBuffer->handle);
}

void ctsCmdBindPipelineImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipeline pPipeline
) {
    CtsDevice device = pCommandBuffer->device;

    CtsPipeline pipeline = pPipeline;
    if (pipeline->bindPoint == CTS_PIPELINE_BIND_POINT_GRAPHICS) {
        CtsGlGraphicsPipeline* graphicsPipeline = pipeline->graphics;
        
        bindDynamicState(device, graphicsPipeline->dynamicState);
        bindVertexInputState(device, &graphicsPipeline->vertexInputState);
        bindInputAssemblyState(device, &graphicsPipeline->inputAssemblyState);
        bindTessellationState(device, &graphicsPipeline->tessellationState);
        bindViewportState(device, &graphicsPipeline->viewportState);
        bindRasterizationState(device, &graphicsPipeline->rasterizationState);
        bindMultisampleState(device, &graphicsPipeline->multisampleState);
        bindDepthStencilState(device, &graphicsPipeline->depthStencilState);
        bindColorBlendState(device, &graphicsPipeline->colorBlendState);
    }
}

void ctsCmdBindVertexBuffersImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstBinding,
    uint32_t pBindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
) {
    CtsDevice device = pCommandBuffer->device;

    for (uint32_t i = pFirstBinding; i < pBindingCount; ++i) {
        const CtsBuffer buffer = pBuffers[i];
        struct CtsGlVertexInputAttributeDescription* description = &device->activeVertexInputState->vertexAttributeDescriptions[pFirstBinding + i];

        glBindBuffer(buffer->type, buffer->handle);
        glEnableVertexAttribArray(description->location);
        glVertexAttribPointer(
            description->location,
            description->numComponents,
            description->format,
            GL_FALSE,
            description->stride,
            (GLvoid*)(description->offset + pOffsets[i])
        );
    }
}

void ctsCmdBlitImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageBlit* pRegions,
    CtsFilter pFilter
) {
}

void ctsCmdClearAttachmentsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pAttachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t pRectCount,
    const CtsClearRect* pRects
) {
}

void ctsCmdClearColorImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearColorValue* pColor,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    // TODO: WTF
    glClear(GL_COLOR_BUFFER_BIT);
}

void ctsCmdClearDepthStencilImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    // TODO: WTF
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void ctsCmdCopyBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferCopy* pRegions
) {
    glBindBuffer(GL_COPY_READ_BUFFER, pSrcBuffer->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->handle);

    for (uint32_t i = 0; i < pRegionCount; ++i) {
        const CtsBufferCopy* region = &pRegions[i];
        glCopyBufferSubData(
            GL_COPY_READ_BUFFER,
            GL_COPY_WRITE_BUFFER,
            region->srcOffset,
            region->dstOffset,
            region->size
        );
    }

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsCmdCopyBufferToImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    (void) pDstImageLayout;

    GLenum target = pDstImage->target;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pSrcBuffer->handle);
    glBindTexture(target, pDstImage->handle);
    
    for (uint32_t i = 0; i < pRegionCount; ++i) {
        const CtsBufferImageCopy* region = &pRegions[i];
        glPixelStorei(GL_UNPACK_ROW_LENGTH, region->bufferRowLength);

        (void) region->bufferImageHeight;
        (void) region->imageSubresource.aspectMask;
        (void) region->imageSubresource.baseArrayLayer;
        (void) region->imageSubresource.layerCount;

        if (target == GL_TEXTURE_1D) {
            glTexSubImage1D(
                target,
                region->imageSubresource.mipLevel,
                region->imageOffset.x,
                region->imageExtent.width,
                pDstImage->internalFormat,
                pDstImage->type,
                (const void*)region->bufferOffset
            );
        } else if (
            target == GL_TEXTURE_2D || 
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            target == GL_TEXTURE_CUBE_MAP_POSITIVE_Z ||
            target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ||
            target == GL_TEXTURE_1D_ARRAY
        ) {
            glTexSubImage2D(
                target,
                region->imageSubresource.mipLevel,
                region->imageOffset.x,
                region->imageOffset.y,
                region->imageExtent.width,
                region->imageExtent.height,
                pDstImage->internalFormat,
                pDstImage->type,
                (const void*)region->bufferOffset
            );
        } else if (
            target == GL_TEXTURE_3D ||
            target == GL_TEXTURE_2D_ARRAY ||
            target == GL_TEXTURE_CUBE_MAP_ARRAY_ARB
        ) {
            glTexSubImage3D(
                target,
                region->imageSubresource.mipLevel,
                region->imageOffset.x,
                region->imageOffset.y,
                region->imageOffset.z,
                region->imageExtent.width,
                region->imageExtent.height,
                region->imageExtent.depth,
                pDstImage->internalFormat,
                pDstImage->type,
                (const void*)region->bufferOffset
            );
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    glBindTexture(target, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void ctsCmdCopyImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageCopy* pRegions
) {
    (void) pSrcImageLayout;
    (void) pDstImageLayout;
    
    for (uint32_t i = 0; i < pRegionCount; ++i) {
        const CtsImageCopy* region = &pRegions[i];

        glCopyImageSubData(
            pSrcImage->handle,
            pSrcImage->target,
            region->srcSubresource.mipLevel,
            region->srcOffset.x,
            region->srcOffset.y,
            region->srcOffset.z,
            pDstImage->handle,
            pDstImage->target,
            region->dstSubresource.mipLevel,
            region->dstOffset.x,
            region->dstOffset.y,
            region->dstOffset.z,
            region->extent.width,
            region->extent.height,
            region->extent.depth
        );
    }
}

void ctsCmdCopyImageToBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    (void) pSrcImageLayout;

    GLenum target = pSrcImage->target;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pDstBuffer->handle);
    glBindTexture(target, pSrcImage->handle);
    
    for (uint32_t i = 0; i < pRegionCount; ++i) {
        const CtsBufferImageCopy* region = &pRegions[i];

        (void) region->bufferImageHeight;
        (void) region->imageSubresource.aspectMask;

        glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->handle);

        void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_WRITE_ONLY);
        size_t bufferOffset = (pDstBuffer->offset + region->bufferOffset);
        void* bufferPos = (char*)bufferData + pDstBuffer->offset;
        GLsizei bufferSize = (pDstBuffer->size - bufferOffset);

        bool isOneDimensionalArray = (target == GL_TEXTURE_1D_ARRAY);
        bool isTwoDimensionalArray = (target == GL_TEXTURE_2D_ARRAY);

        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTextureSubImage.xhtml
        glGetTextureSubImage(
            target,
            region->imageSubresource.mipLevel,
            region->imageOffset.x,
            isOneDimensionalArray
                ? region->imageSubresource.baseArrayLayer
                : region->imageOffset.y,
            isTwoDimensionalArray
                ? region->imageSubresource.baseArrayLayer
                : region->imageOffset.z,
            region->imageExtent.width,
            isOneDimensionalArray
                ? region->imageSubresource.layerCount
                : region->imageExtent.height,
            isTwoDimensionalArray
                ? region->imageSubresource.layerCount
                : region->imageExtent.depth,
            pSrcImage->internalFormat,
            pSrcImage->type,
            bufferSize,
            bufferPos
        );

        glUnmapBuffer(GL_COPY_WRITE_BUFFER);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    glBindTexture(target, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void ctsCmdCopyQueryPoolResultsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pStride,
    CtsQueryResultFlags pFlags
) {
}

void ctsCmdDispatchImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pGroupCountX,
    uint32_t pGroupCountY,
    uint32_t pGroupCountZ
) {
}

void ctsCmdDispatchIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
) {
}

void ctsCmdDrawImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pVertexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstVertex,
    uint32_t pFirstInstance
) {
    CtsDevice device = pCommandBuffer->device;

    glDrawArraysInstancedBaseInstance(
        device->activeInputAssemblyState->polygonMode,
        pFirstVertex,
        pVertexCount,
        pInstanceCount,
        pFirstInstance
    );
}

void ctsCmdDrawIndexedImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pIndexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstIndex,
    int32_t pVertexOffset,
    uint32_t pFirstInstance
) {
    CtsDevice device = pCommandBuffer->device;

    size_t indexSize = (device->activeIndexBuffer->type == GL_UNSIGNED_SHORT)
        ? sizeof(GLushort)
        : sizeof(GLuint);

    glDrawElementsInstancedBaseVertexBaseInstance(
        device->activeInputAssemblyState->polygonMode,
        pIndexCount,
        device->activeIndexBuffer->type,
        (GLvoid*)(pFirstIndex * indexSize),
        pInstanceCount,
        pVertexOffset,
        pFirstInstance
    );
}

void ctsCmdDrawIndexedIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsDevice device = pCommandBuffer->device;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pBuffer->handle);
    glMultiDrawArraysIndirect(
        device->activeInputAssemblyState->polygonMode,
        (GLvoid*)pOffset,
        pDrawCount,
        pStride
    );
}

void ctsCmdDrawIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsDevice device = pCommandBuffer->device;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pBuffer->handle);
    glMultiDrawElementsIndirect(
        device->activeInputAssemblyState->polygonMode,
        device->activeIndexBuffer->type,
        (GLvoid*)pOffset,
        pDrawCount,
        pStride
    );
}

void ctsCmdEndQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
}

void ctsCmdEndRenderPassImpl(
    CtsCommandBuffer pCommandBuffer
) {
}

void ctsCmdExecuteCommandsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    for (uint32_t i = 0; i < pCommandBufferCount; ++i) {
        const CtsCommandBuffer commandBuffer = pCommandBuffers[i];
        const CtsCmdBase* next = commandBuffer->root;

        while (next != NULL) {
            const CtsCommandMetadata* commandMetadata = ctsGetCommandMetadata(next->type);

            size_t size = commandMetadata->size;
            size_t align = commandMetadata->align;

            CtsCmdBase* copy = ctsAllocation(
                &commandBuffer->pool->bumpAllocator,
                size,
                align,
                CTS_SYSTEM_ALLOCATION_SCOPE_COMMAND
            );

            memcpy(copy, next, size);
            copy->next = NULL;

            // Move this to some helper method, called similarly from renderer.c
            if (pCommandBuffer->root == NULL) {
                pCommandBuffer->root = copy;
            } else {
                pCommandBuffer->current->next = copy;
            }

            next = next->next;
        }
    }
}

void ctsCmdFillBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pSize,
    uint32_t pData
) {
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->handle);
    void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_READ_WRITE);

    size_t bufferOffset = (pDstBuffer->offset + pDstOffset);
    void* bufferPos = (char*)bufferData + bufferOffset;
    
    size_t availableSize = (pDstBuffer->size - bufferOffset);
    size_t fillSize = (pSize > availableSize)
        ? availableSize
        : pSize;

    memset(bufferPos, pData, fillSize);

    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsCmdNextSubpassImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsSubpassContents pContents
) {
}

void ctsCmdPipelineBarrierImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlags pSrcStageMask,
    CtsPipelineStageFlags pDstStageMask,
    CtsDependencyFlags pDependencyFlags,
    uint32_t pMemoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t pBufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t pImageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
}

void ctsCmdPushConstantsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineLayout pLayout,
    CtsShaderStageFlags pStageFlags,
    uint32_t pOffset,
    uint32_t pSize,
    const void* pValues
) {
}

void ctsCmdResetEventImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
}

void ctsCmdResetQueryPoolImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount
) {
}

void ctsCmdResolveImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageResolve* pRegions
) {
}

void ctsCmdSetBlendConstantsImpl(
    CtsCommandBuffer pCommandBuffer,
    const float pBlendConstants[4]
) {
    CtsDevice device = pCommandBuffer->device;

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)) {
        glBlendColor(
            pBlendConstants[0],
            pBlendConstants[1],
            pBlendConstants[2],
            pBlendConstants[3]
        );
    }
}

void ctsCmdSetDepthBiasImpl(
    CtsCommandBuffer pCommandBuffer,
    float pDepthBiasConstantFactor,
    float pDepthBiasClamp,
    float pDepthBiasSlopeFactor
) {
}

void ctsCmdSetDepthBoundsImpl(
    CtsCommandBuffer pCommandBuffer,
    float pMinDepthBounds,
    float pMaxDepthBounds
) {
}

void ctsCmdSetDeviceMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pDeviceMask
) {
}

void ctsCmdSetEventImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
}

void ctsCmdSetLineWidthImpl(
    CtsCommandBuffer pCommandBuffer,
    float pLineWidth
) {
    CtsDevice device = pCommandBuffer->device;

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT)) {
        glLineWidth(pLineWidth);
    }
}

void ctsCmdSetScissorImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstScissor,
    uint32_t pScissorCount,
    const CtsRect2D* pScissors
) {
    CtsDevice device = pCommandBuffer->device;

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = pFirstScissor; i < pScissorCount; ++i) {
            const CtsRect2D* scissor = &pScissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height); 
        }
    }
}

void ctsCmdSetStencilCompareMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pCompareMask
) {
}

void ctsCmdSetStencilReferenceImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pReference
) {
}

void ctsCmdSetStencilWriteMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pWriteMask
) {
}

void ctsCmdSetViewportImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstViewport,
    uint32_t pViewportCount,
    const CtsViewport* pViewports
) {
    CtsDevice device = pCommandBuffer->device;

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = pFirstViewport; i < pViewportCount; ++i) {
            const CtsViewport* viewport = &pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }
}

void ctsCmdUpdateBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pDataSize,
    const void* pData
) {
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->handle);
    void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_READ_WRITE);

    size_t bufferOffset = (pDstBuffer->offset + pDstOffset);
    void* bufferPos = (char*)bufferData + bufferOffset;
    
    size_t availableSize = (pDstBuffer->size - bufferOffset);
    size_t copySize = (pDataSize > availableSize)
        ? availableSize
        : pDataSize;

    memcpy(bufferPos, pData, copySize);

    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsCmdWaitEventsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pEventCount,
    const CtsEvent* pEvents,
    CtsPipelineStageFlags pSrcStageMask,
    CtsPipelineStageFlags pDstStageMask,
    uint32_t pMemoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t pBufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t pImageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
}

void ctsCmdWriteTimestampImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlagBits pPipelineStage,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
}

#pragma endregion
#pragma region HelperMethodBodies

static bool hasFlag(CtsFlags flags, CtsFlagBit flag) {
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
    createVertexInputState(pCreateInfo->vertexInputState, pAllocator, &graphicsPipeline->vertexInputState);
    createInputAssemblyState(pCreateInfo->inputAssemblyState, pAllocator, &graphicsPipeline->inputAssemblyState);
    createTessellationState(pCreateInfo->tessellationState, pAllocator, &graphicsPipeline->tessellationState);
    createViewportState(pCreateInfo->viewportState, pAllocator, &graphicsPipeline->viewportState);
    createRasterizationState(pCreateInfo->rasterizationState, pAllocator, &graphicsPipeline->rasterizationState);
    createMultisampleState(pCreateInfo->multisampleState, pAllocator, &graphicsPipeline->multisampleState);
    createDepthStencilState(pCreateInfo->depthStencilState, pAllocator, &graphicsPipeline->depthStencilState);
    createColorBlendState(pCreateInfo->colorBlendState, pAllocator, &graphicsPipeline->colorBlendState);
    graphicsPipeline->dynamicState       = parseDynamicStateFlagBits(pCreateInfo->dynamicState);
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
    uint32_t pStageCount,
    const CtsPipelineShaderStageCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsGlShader* pShader
) {
    if (pStageCount == 0 || pCreateInfos == NULL) {
        return;
    }

    pShader->handle = glCreateProgram();
    pShader->stageCount = pStageCount;
    pShader->stages = ctsAllocation(
        pAllocator,
        sizeof(CtsGlShaderStage) * pStageCount,
        alignof(CtsGlShaderStage),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    GLint success;
    GLchar buffer[512];
    GLsizei bufferLen;

    for (uint32_t i = 0; i < pStageCount; ++i) {
        const CtsPipelineShaderStageCreateInfo* createInfo = &pCreateInfos[i];
        CtsGlShaderStage* stage = &pShader->stages[i];

        stage->name = createInfo->name;
        stage->handle = glCreateShader(parseShaderType(createInfo->stage));
        const char* source = createInfo->module->code;
        const int sourceLen = createInfo->module->codeSize;

        glShaderSource(stage->handle, 1, &source, &sourceLen);
        glCompileShader(stage->handle);
        glGetShaderiv(stage->handle, GL_COMPILE_STATUS, &success);

        if (success) {
            glAttachShader(pShader->handle, pShader->handle);
        }
        else {
            glGetShaderInfoLog(pShader->handle, sizeof(buffer), &bufferLen, buffer);
            fprintf(stderr, "Shader compilation failed for stage %s. Reason %s", stage->name, buffer);
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
            glDeleteShader(pInstance->stages[i].handle);
        }

        glDeleteProgram(pInstance->handle);
        ctsFree(pAllocator, pInstance->stages);
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
    pVertexInputState->vertexAttributeDescriptions = ctsAllocation(
        pAllocator,
        sizeof(CtsGlVertexInputAttributeDescription) * pCreateInfo->vertexAttributeDescriptionCount,
        alignof(CtsGlVertexInputAttributeDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->vertexAttributeDescriptionCount; ++i) {
        uint32_t binding = pCreateInfo->vertexAttributeDescriptions[i].binding;
        CtsAttributeMapping attribute = parseAttributeMapping(pCreateInfo->vertexAttributeDescriptions[i].format);

        // From attribute descriptions
        pVertexInputState->vertexAttributeDescriptions[i].binding = binding;
        pVertexInputState->vertexAttributeDescriptions[i].location = pCreateInfo->vertexAttributeDescriptions[i].location;
        pVertexInputState->vertexAttributeDescriptions[i].format = attribute.type;
        pVertexInputState->vertexAttributeDescriptions[i].numComponents = attribute.numComponent;
        pVertexInputState->vertexAttributeDescriptions[i].offset = pCreateInfo->vertexAttributeDescriptions[i].offset;

        // From binding descriptions
        pVertexInputState->vertexAttributeDescriptions[i].stride = pCreateInfo->vertexBindingDescriptions[binding].stride;
        pVertexInputState->vertexAttributeDescriptions[i].inputRate = pCreateInfo->vertexBindingDescriptions[binding].inputRate;
    }
}

static void destroyVertexInputState(
    CtsGlPipelineVertexInputState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        ctsFree(pAllocator, pInstance->vertexAttributeDescriptions);
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
    pViewportState->viewports = ctsAllocation(
        pAllocator,
        sizeof(CtsViewport) * pCreateInfo->viewportCount,
        alignof(CtsViewport),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->viewportCount; ++i) {
        pViewportState->viewports[i].x        = pCreateInfo->viewports[i].x;
        pViewportState->viewports[i].y        = pCreateInfo->viewports[i].y;
        pViewportState->viewports[i].width    = pCreateInfo->viewports[i].width;
        pViewportState->viewports[i].height   = pCreateInfo->viewports[i].height;
        pViewportState->viewports[i].minDepth = pCreateInfo->viewports[i].minDepth;
        pViewportState->viewports[i].maxDepth = pCreateInfo->viewports[i].maxDepth;
    }

    pViewportState->scissorCount = pCreateInfo->scissorCount;
    pViewportState->scissors = ctsAllocation(
        pAllocator,
        sizeof(CtsRect2D) * pCreateInfo->scissorCount,
        alignof(CtsRect2D),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pCreateInfo->scissorCount; ++i) {
        pViewportState->scissors[i].extent.width  = pCreateInfo->scissors[i].extent.width;
        pViewportState->scissors[i].extent.height = pCreateInfo->scissors[i].extent.height;
        pViewportState->scissors[i].offset.x      = pCreateInfo->scissors[i].offset.x;
        pViewportState->scissors[i].offset.y      = pCreateInfo->scissors[i].offset.y;
    }
}

static void destroyViewportState(
    CtsGlPipelineViewportState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        ctsFree(pAllocator, pInstance->viewports);
        ctsFree(pAllocator, pInstance->scissors);
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

    if (pCreateInfo->attachmentCount > 1) {
        // TODO: Unsupported
    }

    const CtsPipelineColorBlendAttachmentState* attachment = &pCreateInfo->attachments[0];
    pColorBlendState->blendEnable         = attachment->blendEnable;
    pColorBlendState->srcColorBlendFactor = parseBlendFunc(attachment->srcColorBlendFactor);
    pColorBlendState->dstColorBlendFactor = parseBlendFunc(attachment->dstColorBlendFactor);
    pColorBlendState->colorBlendOp        = parseBlendOperation(attachment->colorBlendOp);
    pColorBlendState->srcAlphaBlendFactor = parseBlendFunc(attachment->srcAlphaBlendFactor);
    pColorBlendState->dstAlphaBlendFactor = parseBlendFunc(attachment->dstAlphaBlendFactor);
    pColorBlendState->alphaBlendOp        = parseBlendOperation(attachment->alphaBlendOp);
    pColorBlendState->colorWriteMask      = attachment->colorWriteMask;
    memcpy(pColorBlendState->blendConstants, pCreateInfo->blendConstants, sizeof(pCreateInfo->blendConstants));
}

static void destroyColorBlendState(
    CtsGlPipelineColorBlendState* pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    // Nothing to do
}

static void bindDynamicState(
    CtsDevice pDevice,
    CtsFlags pState
) {
    pDevice->activeDynamicState = pState;
}

static void bindVertexInputState(
    CtsDevice pDevice,
    CtsGlPipelineVertexInputState* pState
) {
    if (pDevice->activeVertexInputState == pState) {
        return;
    }

    pDevice->activeVertexInputState = pState;
}

static void bindInputAssemblyState(
    CtsDevice pDevice,
    CtsGlPipelineInputAssemblyState* pState
) {
    if (pDevice->activeInputAssemblyState == pState) {
        return;
    }

    pDevice->activeInputAssemblyState = pState;
}

static void bindTessellationState(
    CtsDevice pDevice,
    CtsGlPipelineTessellationState* pState
) {
    if (pDevice->activeTessellationState == pState) {
        return;
    }

    pDevice->activeTessellationState = pState;
}

static void bindViewportState(
    CtsDevice pDevice,
    CtsGlPipelineViewportState* pState
) {
    if (pDevice->activeViewportState == pState) {
        return;
    }

    uint32_t viewportCount = pState->viewportCount;
    uint32_t scissorCount = pState->scissorCount;

    if (!hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = 0; i < viewportCount; ++i) {
            CtsViewport* viewport = &pState->viewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }

    if (!hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = 0; i < scissorCount; ++i) {
            CtsRect2D* scissor = &pState->scissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height);    
        }
    }
}

static void bindRasterizationState(
    CtsDevice pDevice,
    CtsGlPipelineRasterizationState* pState
) {
    if (pDevice->activeRasterizationState == pState) {
        return;
    }

    CtsGlRasterizationStateChanges changes;
    parseRasterizationStateChanges(pState, pDevice->activeRasterizationState, &changes);
    pDevice->activeRasterizationState = pState;

    if (changes.depthClampEnableChanged) {
        if (pState->depthClampEnable) {
            glEnable(GL_DEPTH_CLAMP);
        } else {
            glDisable(GL_DEPTH_CLAMP);
        }
    }
    
    if (changes.rasterizerDiscardEnableChanged) {
        if (pState->rasterizerDiscardEnable) {
            glEnable(GL_RASTERIZER_DISCARD);
        } else {
            glDisable(GL_RASTERIZER_DISCARD);
        }
    }
    
    if (changes.polygonModeChanged) {
        glPolygonMode(GL_FRONT_AND_BACK, pState->polygonMode);
    }

    if (changes.cullModeChanged) {
        if (pState->cullMode != GL_NONE) {
            glEnable(GL_CULL_FACE);
            glCullFace(pState->cullMode);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    if (changes.frontFaceChanged) {
        glFrontFace(pState->frontFace);
    }

    if (!hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_DEPTH_BIAS_BIT) && changes.depthBiasChanged) {
        if (pState->depthBiasEnable) {
            glPolygonOffset(pState->depthBiasConstantFactor, 0);
        } else {
            glPolygonOffset(0, 0);
        }

        // Not sure how to map depthBiasClamp or depthBiasSlopeFactor
    }

    if (!hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT) && changes.lineWidthChanged) {
        glLineWidth(pState->lineWidth);
    }
}

static void bindMultisampleState(
    CtsDevice pDevice,
    CtsGlPipelineMultisampleState* pState)
{
    if (pDevice->activeMultisampleState == pState) {
        return;
    }

    pDevice->activeMultisampleState = pState;
}

static void bindDepthStencilState(
    CtsDevice pDevice,
    CtsGlPipelineDepthStencilState* pState
) {
    if (pDevice->activeDepthStencilState == pState) {
        return;
    }

    CtsGlDepthStencilStateChanges changes;
    parseDepthStencilStateChanges(pState, pDevice->activeDepthStencilState, &changes);
    pDevice->activeDepthStencilState = pState;

    if (changes.depthTestEnableChanged) {
        if (pState->depthTestEnable) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    if (changes.depthWriteEnableChanged) {
        if (pState->depthWriteEnable) {
            glDepthMask(GL_TRUE);
        } else {
            glDepthMask(GL_FALSE);
        }
    }

    if (changes.depthCompareOpChanged) {
        glDepthFunc(pState->depthCompareOp);
    }

    if (changes.depthBoundsTestEnableChanged) {
        // TODO: Requires extension
    }

    if (changes.stencilTestEnableChanged) {
        if (pState->stencilTestEnable) {
            glEnable(GL_STENCIL_TEST);
        } else {
            glDisable(GL_STENCIL_TEST);
        }
    }

    if (changes.depthBoundsChanged) {
        // TODO: Requires extension
    }

    if (changes.frontFuncChanged) {
        glStencilFuncSeparate(
            GL_FRONT,
            pState->frontCompareOp,
            pState->frontReference,
            pState->frontCompareMask);
    }

    if (changes.frontStencilChanged) {
        glStencilOpSeparate(
            GL_FRONT,
            pState->frontFailOp,
            pState->frontDepthFailOp,
            pState->frontPassOp);
    }

    if (changes.frontFaceWriteMaskChanged) {
        glStencilMaskSeparate(GL_FRONT, pState->frontWriteMask);
    }

    if (changes.backFuncChanged) {
        glStencilFuncSeparate(
            GL_BACK,
            pState->backCompareOp,
            pState->backReference,
            pState->backCompareMask);
    }

    if (changes.backStencilChanged) {
        glStencilOpSeparate(
            GL_BACK,
            pState->backFailOp,
            pState->backDepthFailOp,
            pState->backPassOp);
    }

    if (changes.backFaceWriteMaskChanged) {
        glStencilMaskSeparate(GL_BACK, pState->backWriteMask);
    }
}


static void bindColorBlendState(
    CtsDevice pDevice,
    CtsGlPipelineColorBlendState* pState
) {
    if (pDevice->activeColorBlendState == pState) {
        return;
    }

    CtsGlColorBlendStateChanges changes;
    parseColorBlendStateChanges(pState, pDevice->activeColorBlendState, &changes);
    pDevice->activeColorBlendState = pState;

    if (changes.blendEnableChanged) {
        if (pState->blendEnable) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }

    if (changes.blendConstantsChanged && !hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)) {
        glBlendColor(
            pState->blendConstants[0],
            pState->blendConstants[1],
            pState->blendConstants[2],
            pState->blendConstants[3]
        );
    }

    if (changes.colorWriteMaskChanged) {
        glColorMask(
            (pState->colorWriteMask & CTS_COLOR_COMPONENT_R_BIT) == CTS_COLOR_COMPONENT_R_BIT,
            (pState->colorWriteMask & CTS_COLOR_COMPONENT_G_BIT) == CTS_COLOR_COMPONENT_G_BIT,
            (pState->colorWriteMask & CTS_COLOR_COMPONENT_B_BIT) == CTS_COLOR_COMPONENT_B_BIT,
            (pState->colorWriteMask & CTS_COLOR_COMPONENT_A_BIT) == CTS_COLOR_COMPONENT_A_BIT
        );
    }

    if (changes.blendFactorChanged) {
        glBlendFuncSeparate(
            pState->srcColorBlendFactor,
            pState->dstColorBlendFactor,
            pState->srcAlphaBlendFactor,
            pState->dstAlphaBlendFactor
        );
    }

    if (changes.blendOpChanged) {
        glBlendEquationSeparate(
            pState->colorBlendOp,
            pState->alphaBlendOp
        );
    }
}

#pragma endregion

#ifdef __cplusplus
}
#endif