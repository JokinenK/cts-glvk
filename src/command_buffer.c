#include <assert.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/semaphore.h"
#include "cts/command_buffer.h"
#include "cts/commands.h"
#include "cts/command_dispatcher.h"
#include "cts/type_mapper.h"
#include "cts/gl_helper.h"
#include "cts/gl_enums.h"
#include "cts/gl_pipeline.h"
#include "cts/gl_shader.h"
#include "cts/gl_texture.h"
#include "cts/gl_descriptor.h"
#include "cts/private.h"
#include "cts/command_buffer_private.h"
#include "cts/command_pool_private.h"
#include "cts/buffer_private.h"
#include "cts/buffer_view_private.h"
#include "cts/device_memory_private.h"
#include "cts/device_private.h"
#include "cts/descriptor_set_private.h"
#include "cts/descriptor_set_layout_private.h"
#include "cts/fence_private.h"
#include "cts/framebuffer_private.h"
#include "cts/pipeline_private.h"
#include "cts/queue_private.h"
#include "cts/render_pass_private.h"
#include "cts/semaphore_private.h"
#include "cts/image_private.h"
#include "cts/image_view_private.h"
#include "cts/sampler_private.h"
#include "cts/physical_device_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static void* advance(void* pPtr, size_t amount);
static bool hasFlag(VkFlags flags, VkFlags flag);

static bool shouldUpdateBool(bool enable, bool* pCurrent, bool* pPrevious);
static bool shouldUpdateHandle(GLuint handle, GLuint* pCurrent, GLuint* pPrevious);
static void enableFeature(GLenum feature, bool enable);
static void enableFeatureIndexed(GLenum feature, uint32_t index, bool enable);
static void enableDepthClamp(struct CtsDevice* device, bool enable, bool* pPrevious);
static void enableRasterizerDiscard(struct CtsDevice* device, bool enable, bool* pPrevious);
static void enableCullFace(struct CtsDevice* device, bool enable, bool* pPrevious);
static void enableDepthTest(struct CtsDevice* device, bool enable, bool* pPrevious);
static void enableStencilTest(struct CtsDevice* device, bool enable, bool* pPrevious);
static void enableBlend(struct CtsDevice* device, uint32_t index, bool enable, bool* pPrevious);
static void enableColorLogicOp(struct CtsDevice* device, bool enable, bool* pPrevious);
static void useProgram(struct CtsDevice* device, GLuint program, GLuint* pPrevious);

static void bindTexture(struct CtsDevice* device, uint32_t pSlot, GLenum pTarget, uint32_t pHandle, CtsGlTextureBinding* pPrevious);
static void bindSampler(struct CtsDevice* device, uint32_t unit, uint32_t sampler, uint32_t* pPrevious);
static void bindRenderPass(struct CtsDevice* device, struct CtsFramebuffer* framebuffer, struct CtsRenderPass* renderPass, uint32_t subpassNumber);
static void bindDynamicState(struct CtsDevice* device, VkFlags pState);
static void bindVertexInputState(struct CtsDevice* device, CtsGlPipelineVertexInputState* pState);
static void bindInputAssemblyState(struct CtsDevice* device, CtsGlPipelineInputAssemblyState* pState);
static void bindTessellationState(struct CtsDevice* device, CtsGlPipelineTessellationState* pState);
static void bindViewportState(struct CtsDevice* device, CtsGlPipelineViewportState* pState);
static void bindRasterizationState(struct CtsDevice* device, CtsGlPipelineRasterizationState* pState);
static void bindMultisampleState(struct CtsDevice* device, CtsGlPipelineMultisampleState* pState);
static void bindDepthStencilState(struct CtsDevice* device, CtsGlPipelineDepthStencilState* pState);
static void bindColorBlendState(struct CtsDevice* device, CtsGlPipelineColorBlendState* pState);

static void resolveRenderPass(struct CtsDevice* device, struct CtsRenderPass* renderPass, uint32_t subpassNumber);

static void* allocateCommand(struct CtsCommandBuffer* pCommandBuffer, CtsCommandType commandType, size_t extraDataLen);

VkResult VKAPI_CALL ctsBeginCommandBuffer(
    VkCommandBuffer commandBufferHandle,
    const VkCommandBufferBeginInfo* pBeginInfo
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    (void) pBeginInfo->pInheritanceInfo;

    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_INITIAL) {
        commandBuffer->flags = pBeginInfo->flags;
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsResetCommandBuffer(
    VkCommandBuffer commandBufferHandle,
    VkCommandBufferResetFlags flags
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING || commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    ctsLinearAllocatorReset(commandBuffer->linearAllocator);

    if (flags & VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) {
        ctsLinearAllocatorTrim(commandBuffer->linearAllocator);    
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsEndCommandBuffer(
    VkCommandBuffer commandBufferHandle
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING) {
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsQueueSubmit(
    VkQueue queueHandle,
    uint32_t submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence fence
) {
    struct CtsQueue* queue = CtsQueueFromHandle(queueHandle);

    for (uint32_t i = 0; i < submitCount; ++i) {
        const VkSubmitInfo* submit = &pSubmits[i];

        // TODO: Handle waitDstStageMask
        ctsWaitSemaphores(submit->waitSemaphoreCount, submit->pWaitSemaphores);

        for (uint32_t j = 0; j < submit->commandBufferCount; ++j) {
            VkCommandBuffer commandBufferHandle = submit->pCommandBuffers[j];
            struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

            if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
                commandBuffer->state = CTS_COMMAND_BUFFER_STATE_PENDING;

                ctsQueueDispatch(queue, commandBuffer->root);
                ctsSignalSemaphores(submit->signalSemaphoreCount, submit->pSignalSemaphores);

                if (fence != NULL) {
                    ctsSignalFence(CtsDeviceToHandle(commandBuffer->device), fence);
                }

                if (commandBuffer->flags & VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
                    commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
                } else {
                    commandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
                }
            }
        }
    }
   
    return VK_SUCCESS;
}

void VKAPI_CALL ctsCmdBeginQuery(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdBeginQuery* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_QUERY,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->queryPool = queryPool;
    cmd->query = query;
    cmd->flags = flags;
}

void VKAPI_CALL ctsCmdEndQuery(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t query
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdEndQuery* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_QUERY,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->queryPool = queryPool;
    cmd->query = query;
}

void VKAPI_CALL ctsCmdBeginRenderPass(
    VkCommandBuffer commandBufferHandle,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t renderPassBeginSize = sizeof(VkRenderPassBeginInfo);
    size_t clearValueSize = pRenderPassBegin->clearValueCount * sizeof(VkClearValue);

    CtsCmdBeginRenderPass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_RENDER_PASS,
        renderPassBeginSize + clearValueSize
    );

    void* pRenderPassBeginCopy = advance(cmd, sizeof(CtsCmdBeginRenderPass));
    void* pClearValuesCopy = advance(pRenderPassBeginCopy, renderPassBeginSize);

    memcpy(pRenderPassBeginCopy, pRenderPassBegin, renderPassBeginSize);
    memcpy(pClearValuesCopy, pRenderPassBegin->pClearValues, clearValueSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->pRenderPassBegin = pRenderPassBeginCopy;
    cmd->pRenderPassBegin->pClearValues = pClearValuesCopy;
    cmd->contents = contents;
}

void VKAPI_CALL ctsCmdEndRenderPass(
    VkCommandBuffer commandBufferHandle
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdEndRenderPass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_RENDER_PASS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
}

void VKAPI_CALL ctsCmdBindDescriptorSets(
    VkCommandBuffer commandBufferHandle,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t descriptorSetSize = descriptorSetCount * sizeof(VkDescriptorSet);
    size_t dynamicOffsetSize = dynamicOffsetCount * sizeof(uint32_t);

    CtsCmdBindDescriptorSets* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS,
        descriptorSetSize + dynamicOffsetSize
    );

    void* pDescriptorSetsCopy = advance(cmd, sizeof(CtsCmdBindDescriptorSets));
    void* pDynamicOffsetsCopy = advance(pDescriptorSetsCopy, descriptorSetSize);

    memcpy(pDescriptorSetsCopy, pDescriptorSets, descriptorSetSize);
    memcpy(pDynamicOffsetsCopy, pDynamicOffsets, dynamicOffsetSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->pipelineBindPoint = pipelineBindPoint;
    cmd->firstSet = firstSet;
    cmd->descriptorSetCount = descriptorSetCount;
    cmd->pDescriptorSets = pDescriptorSetsCopy;
    cmd->dynamicOffsetCount = dynamicOffsetCount;
    cmd->pDynamicOffsets = pDynamicOffsetsCopy;
}

void VKAPI_CALL ctsCmdBindIndexBuffer(
    VkCommandBuffer commandBufferHandle,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkIndexType indexType
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdBindIndexBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_INDEX_BUFFER,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->indexType = indexType;
}

void VKAPI_CALL ctsCmdBindPipeline(
    VkCommandBuffer commandBufferHandle,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdBindPipeline* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_PIPELINE,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->pipelineBindPoint = pipelineBindPoint;
    cmd->pipeline = pipeline;
}

void VKAPI_CALL ctsCmdBindVertexBuffers(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t buffersSize = bindingCount * sizeof(VkBuffer);
    size_t offsetsSize = bindingCount * sizeof(VkDeviceSize);

    CtsCmdBindVertexBuffers* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS,
        buffersSize + offsetsSize
    );

    void* pBuffersCopy = advance(cmd, sizeof(CtsCmdBindVertexBuffers));
    void* pOffsetsCopy = advance(pBuffersCopy, buffersSize);

    memcpy(pBuffersCopy, pBuffers, buffersSize);
    memcpy(pOffsetsCopy, pOffsets, offsetsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->firstBinding = firstBinding;
    cmd->bindingCount = bindingCount;
    cmd->pBuffers = pBuffersCopy;
    cmd->pOffsets = pOffsetsCopy;
}

void VKAPI_CALL ctsCmdBlitImage(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionsSize = regionCount * sizeof(VkImageBlit);

    CtsCmdBlitImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BLIT_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdBlitImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
    cmd->filter = filter;
}

void VKAPI_CALL ctsCmdClearAttachments(
    VkCommandBuffer commandBufferHandle,
    uint32_t attachmentCount,
    const VkClearAttachment* pAttachments,
    uint32_t rectCount,
    const VkClearRect* pRects
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t attachmentsSize = attachmentCount * sizeof(VkClearAttachment);
    size_t rectsSize = rectCount * sizeof(VkClearRect);

    CtsCmdClearAttachments* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_ATTACHMENTS,
        attachmentsSize + rectsSize
    );

    void* pAttachmentsCopy = advance(cmd, sizeof(CtsCmdClearAttachments));
    void* pReVkCopy = advance(pAttachmentsCopy, attachmentsSize);
    
    memcpy(pAttachmentsCopy, pAttachments, attachmentsSize);
    memcpy(pReVkCopy, pRects, rectsSize);
    
    cmd->commandBuffer = commandBufferHandle;
    cmd->attachmentCount = attachmentCount;
    cmd->pAttachments = pAttachmentsCopy;
    cmd->rectCount = rectCount;
    cmd->pRects = pReVkCopy;
}

void VKAPI_CALL ctsCmdClearColorImage(
    VkCommandBuffer commandBufferHandle,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearColorValue* pColor,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t colorSize = sizeof(VkClearColorValue);
    size_t rangesSize = rangeCount * sizeof(VkImageSubresourceRange);

    CtsCmdClearColorImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE,
        colorSize + rangesSize
    );

    void* pColorCopy = advance(cmd, sizeof(CtsCmdClearColorImage));
    void* pRangesCopy = advance(pColorCopy, colorSize);

    memcpy(pColorCopy, pColor, colorSize);
    memcpy(pRangesCopy, pRanges, rangesSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pColor = pColorCopy;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRangesCopy;
}

void VKAPI_CALL ctsCmdClearDepthStencilImage(
    VkCommandBuffer commandBufferHandle,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t depthStencilSize = sizeof(VkClearDepthStencilValue);
    size_t rangesSize = rangeCount * sizeof(VkImageSubresourceRange);

    CtsCmdClearDepthStencilImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
        depthStencilSize + rangesSize
    );

    void* pDepthStencilCopy = advance(cmd, sizeof(CtsCmdClearDepthStencilImage));
    void* pRangesCopy = advance(pDepthStencilCopy, depthStencilSize);
    
    memcpy(pDepthStencilCopy, pDepthStencil, depthStencilSize);
    memcpy(pRangesCopy, pRanges, rangesSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pDepthStencil = pDepthStencilCopy;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRangesCopy;
}

void VKAPI_CALL ctsCmdCopyBuffer(
    VkCommandBuffer commandBufferHandle,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionSize = regionCount * sizeof(VkBufferCopy);

    CtsCmdCopyBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER,
        regionSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyBuffer));
    memcpy(pRegionsCopy, pRegions, regionSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcBuffer = srcBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void VKAPI_CALL ctsCmdCopyBufferToImage(
    VkCommandBuffer commandBufferHandle,
    VkBuffer srcBuffer,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionSize = regionCount * sizeof(VkBufferImageCopy);

    CtsCmdCopyBufferToImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE,
        regionSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyBufferToImage));
    memcpy(pRegionsCopy, pRegions, regionSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcBuffer = srcBuffer;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void VKAPI_CALL ctsCmdCopyImage(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionsSize = regionCount * sizeof(VkImageCopy);

    CtsCmdCopyImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void VKAPI_CALL ctsCmdCopyImageToBuffer(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionsSize = regionCount * sizeof(VkBufferImageCopy);

    CtsCmdCopyImageToBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void VKAPI_CALL ctsCmdCopyQueryPoolResults(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize stride,
    VkQueryResultFlags flags
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdCopyQueryPoolResults* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->queryPool = queryPool;
    cmd->firstQuery = firstQuery;
    cmd->queryCount = queryCount;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->stride = stride;
    cmd->flags = flags;
}

void VKAPI_CALL ctsCmdDispatch(
    VkCommandBuffer commandBufferHandle,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDispatch* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->groupCountX = groupCountX;
    cmd->groupCountY = groupCountY;
    cmd->groupCountZ = groupCountZ;
}

void VKAPI_CALL ctsCmdDispatchBase(
    VkCommandBuffer commandBufferHandle,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDispatchBase* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH_BASE,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->baseGroupX = baseGroupX;
    cmd->baseGroupY = baseGroupY;
    cmd->baseGroupZ = baseGroupZ;
    cmd->groupCountX = groupCountX;
    cmd->groupCountY = groupCountY;
    cmd->groupCountZ = groupCountZ;
}

void VKAPI_CALL ctsCmdDispatchIndirect(
    VkCommandBuffer commandBufferHandle,
    VkBuffer buffer,
    VkDeviceSize offset
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDispatchIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->buffer = buffer;
    cmd->offset = offset;
}

void VKAPI_CALL ctsCmdDraw(
    VkCommandBuffer commandBufferHandle,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDraw* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->vertexCount = vertexCount;
    cmd->instanceCount = instanceCount;
    cmd->firstVertex = firstVertex;
    cmd->firstInstance = firstInstance;
}

void VKAPI_CALL ctsCmdDrawIndexed(
    VkCommandBuffer commandBufferHandle,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDrawIndexed* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->indexCount = indexCount;
    cmd->instanceCount = instanceCount;
    cmd->firstIndex = firstIndex;
    cmd->vertexOffset = vertexOffset;
    cmd->firstInstance = firstInstance;
}

void VKAPI_CALL ctsCmdDrawIndexedIndirect(
    VkCommandBuffer commandBufferHandle,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDrawIndexedIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->drawCount = drawCount;
    cmd->stride = stride;
}

void VKAPI_CALL ctsCmdDrawIndirect(
    VkCommandBuffer commandBufferHandle,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdDrawIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->drawCount = drawCount;
    cmd->stride = stride;
}

void VKAPI_CALL ctsCmdExecuteCommands(
    VkCommandBuffer commandBufferHandle,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t commandBuffersSize = commandBufferCount * sizeof(VkCommandBuffer);

    CtsCmdExecuteCommands* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_EXECUTE_COMMANDS,
        commandBuffersSize
    );

    void* pCommandBuffersCopy = advance(cmd, sizeof(CtsCmdExecuteCommands));
    memcpy(pCommandBuffersCopy, pCommandBuffers, commandBuffersSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->commandBufferCount = commandBufferCount;
    cmd->pCommandBuffers = pCommandBuffersCopy;
}

void VKAPI_CALL ctsCmdFillBuffer(
    VkCommandBuffer commandBufferHandle,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize size,
    uint32_t data
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdFillBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_FILL_BUFFER,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->size = size;
    cmd->data = data;
}

void VKAPI_CALL ctsCmdNextSubpass(
    VkCommandBuffer commandBufferHandle,
    VkSubpassContents contents
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdNextSubpass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_NEXT_SUBPASS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->contents = contents;
}

void VKAPI_CALL ctsCmdPipelineBarrier(
    VkCommandBuffer commandBufferHandle,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t memoryBarrierSize = memoryBarrierCount * sizeof(VkMemoryBarrier);
    size_t bufferMemoryBarrierSize = bufferMemoryBarrierCount * sizeof(VkBufferMemoryBarrier);
    size_t imageMemoryBarrierSize = imageMemoryBarrierCount * sizeof(VkImageMemoryBarrier);

    CtsCmdPipelineBarrier* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_PIPELINE_BARRIER,
        memoryBarrierSize + bufferMemoryBarrierSize + imageMemoryBarrierSize
    );

    void* pMemoryBarriersCopy = advance(cmd, sizeof(CtsCmdPipelineBarrier));
    void* pBufferMemoryBarriersCopy = advance(pMemoryBarriersCopy, memoryBarrierSize);
    void* pImageMemoryBarriersCopy = advance(pBufferMemoryBarriersCopy, bufferMemoryBarrierSize);

    memcpy(pMemoryBarriersCopy, pMemoryBarriers, memoryBarrierSize);
    memcpy(pBufferMemoryBarriersCopy, pBufferMemoryBarriers, bufferMemoryBarrierSize);
    memcpy(pImageMemoryBarriersCopy, pImageMemoryBarriers, imageMemoryBarrierSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcStageMask = srcStageMask;
    cmd->dstStageMask = dstStageMask;
    cmd->dependencyFlags = dependencyFlags;
    cmd->memoryBarrierCount = memoryBarrierCount;
    cmd->pMemoryBarriers = pMemoryBarriersCopy;
    cmd->bufferMemoryBarrierCount = bufferMemoryBarrierCount;
    cmd->pBufferMemoryBarriers = pBufferMemoryBarriersCopy;
    cmd->imageMemoryBarrierCount = imageMemoryBarrierCount;
    cmd->pImageMemoryBarriers = pImageMemoryBarriersCopy;
}

void VKAPI_CALL ctsCmdPushConstants(
    VkCommandBuffer commandBufferHandle,
    VkPipelineLayout layout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdPushConstants* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_PUSH_CONSTANTS,
        size
    );

    void* pValuesCopy = advance(cmd, sizeof(CtsCmdPushConstants));
    memcpy(pValuesCopy, pValues, size);

    cmd->commandBuffer = commandBufferHandle;
    cmd->layout = layout;
    cmd->stageFlags = stageFlags;
    cmd->offset = offset;
    cmd->size = size;
    cmd->pValues = pValuesCopy;
}

void VKAPI_CALL ctsCmdResetEvent(
    VkCommandBuffer commandBufferHandle,
    VkEvent event,
    VkPipelineStageFlags stageMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdResetEvent* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_EVENT,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->event = event;
    cmd->stageMask = stageMask;
}

void VKAPI_CALL ctsCmdResetQueryPool(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdResetQueryPool* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_QUERY_POOL,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->queryPool = queryPool;
    cmd->firstQuery = firstQuery;
    cmd->queryCount = queryCount;
}

void VKAPI_CALL ctsCmdResolveImage(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageResolve* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t regionsSize = regionCount * sizeof(VkImageBlit);

    CtsCmdResolveImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESOLVE_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdBlitImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void VKAPI_CALL ctsCmdSetBlendConstants(
    VkCommandBuffer commandBufferHandle,
    const float blendConstants[4]
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    // TODO: This might need attention
    CtsCmdSetBlendConstants* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_BLEND_CONSTANTS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    memcpy(cmd->blendConstants, blendConstants, sizeof(float[4]));
}

void VKAPI_CALL ctsCmdSetDepthBias(
    VkCommandBuffer commandBufferHandle,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetDepthBias* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BIAS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->depthBiasConstantFactor = depthBiasConstantFactor;
    cmd->depthBiasClamp = depthBiasClamp;
    cmd->depthBiasSlopeFactor = depthBiasSlopeFactor;
}

void VKAPI_CALL ctsCmdSetDepthBounds(
    VkCommandBuffer commandBufferHandle,
    float minDepthBounds,
    float maxDepthBounds
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetDepthBounds* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BOUNDS,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->minDepthBounds = minDepthBounds;
    cmd->maxDepthBounds = maxDepthBounds;
}

void VKAPI_CALL ctsCmdSetDeviceMask(
    VkCommandBuffer commandBufferHandle,
    uint32_t deviceMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetDeviceMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEVICE_MASK,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->deviceMask = deviceMask;
}

void VKAPI_CALL ctsCmdSetEvent(
    VkCommandBuffer commandBufferHandle,
    VkEvent event,
    VkPipelineStageFlags stageMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetEvent* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_EVENT,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->event = event;
    cmd->stageMask = stageMask;
}

void VKAPI_CALL ctsCmdSetLineWidth(
    VkCommandBuffer commandBufferHandle,
    float lineWidth
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetLineWidth* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_LINE_WIDTH,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->lineWidth = lineWidth;
}

void VKAPI_CALL ctsCmdSetScissor(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t scissorsSize = scissorCount * sizeof(VkRect2D);

    CtsCmdSetScissor* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_SCISSOR,
        scissorsSize
    );

    void* pScissorsCopy = advance(cmd, sizeof(CtsCmdSetScissor));
    memcpy(pScissorsCopy, pScissors, scissorsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->firstScissor = firstScissor;
    cmd->scissorCount = scissorCount;
    cmd->pScissors = pScissorsCopy;
}

void VKAPI_CALL ctsCmdSetStencilCompareMask(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t compareMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetStencilCompareMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->faceMask = faceMask;
    cmd->compareMask = compareMask;
}

void VKAPI_CALL ctsCmdSetStencilReference(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t reference
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetStencilReference* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_REFERENCE,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->faceMask = faceMask;
    cmd->reference = reference;
}

void VKAPI_CALL ctsCmdSetStencilWriteMask(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t writeMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdSetStencilWriteMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->faceMask = faceMask;
    cmd->writeMask = writeMask;
}

void VKAPI_CALL ctsCmdSetViewport(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t viewportsSize = viewportCount * sizeof(VkViewport);

    CtsCmdSetViewport* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_VIEWPORT,
        viewportsSize
    );

    void* pViewportsCopy = advance(cmd, sizeof(CtsCmdSetViewport));
    memcpy(pViewportsCopy, pViewports, viewportsSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->firstViewport = firstViewport;
    cmd->viewportCount = viewportCount;
    cmd->pViewports = pViewportsCopy;
}

void VKAPI_CALL ctsCmdUpdateBuffer(
    VkCommandBuffer commandBufferHandle,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize dataSize,
    const void* pData
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdUpdateBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_UPDATE_BUFFER,
        dataSize
    );

    void* pDataCopy = advance(cmd, sizeof(CtsCmdUpdateBuffer));
    memcpy(pDataCopy, pData, dataSize); 

    cmd->commandBuffer = commandBufferHandle;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->dataSize = dataSize;
    cmd->pData = pDataCopy;
}

void VKAPI_CALL ctsCmdWaitEvents(
    VkCommandBuffer commandBufferHandle,
    uint32_t eventCount,
    const VkEvent* events,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    size_t memoryBarriersSize = memoryBarrierCount * sizeof(VkMemoryBarrier);
    size_t bufferMemoryBarriersSize = bufferMemoryBarrierCount * sizeof(VkBufferMemoryBarrier);
    size_t imageMemoryBarriersSize = imageMemoryBarrierCount * sizeof(VkImageMemoryBarrier);

    CtsCmdWaitEvents* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_WAIT_EVENTS,
        memoryBarriersSize + bufferMemoryBarriersSize + imageMemoryBarriersSize
    );

    void* pMemoryBarriersCopy = advance(cmd, sizeof(CtsCmdWaitEvents));
    void* pBufferMemoryBarriersCopy = advance(pMemoryBarriersCopy, memoryBarriersSize);
    void* pImageMemoryBarriersCopy = advance(pBufferMemoryBarriersCopy, bufferMemoryBarriersSize);

    memcpy(pMemoryBarriersCopy, pMemoryBarriers, memoryBarriersSize);
    memcpy(pBufferMemoryBarriersCopy, pBufferMemoryBarriers, bufferMemoryBarriersSize);
    memcpy(pImageMemoryBarriersCopy, pImageMemoryBarriers, imageMemoryBarriersSize);

    cmd->commandBuffer = commandBufferHandle;
    cmd->eventCount = eventCount;
    cmd->events = events;
    cmd->srcStageMask = srcStageMask;
    cmd->dstStageMask = dstStageMask;
    cmd->memoryBarrierCount = memoryBarrierCount;
    cmd->pMemoryBarriers = pMemoryBarriersCopy;
    cmd->bufferMemoryBarrierCount = bufferMemoryBarrierCount;
    cmd->pBufferMemoryBarriers = pBufferMemoryBarriersCopy;
    cmd->imageMemoryBarrierCount = imageMemoryBarrierCount;
    cmd->pImageMemoryBarriers = pImageMemoryBarriersCopy;
}

void VKAPI_CALL ctsCmdWriteTimestamp(
    VkCommandBuffer commandBufferHandle,
    VkPipelineStageFlagBits pipelineStage,
    VkQueryPool queryPool,
    uint32_t query
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    CtsCmdWriteTimestamp* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_WRITE_TIMESTAMP,
        0
    );

    cmd->commandBuffer = commandBufferHandle;
    cmd->pipelineStage = pipelineStage;
    cmd->queryPool = queryPool;
    cmd->query = query;
}

void ctsCmdBeginQueryImpl(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    // TODO: Implement this
    //glBeginQuery()
}

void ctsCmdEndQueryImpl(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t query
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    //glEndQuery()
}

void ctsCmdBeginRenderPassImpl(
    VkCommandBuffer commandBufferHandle,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    (void) pRenderPassBegin->renderArea;
    (void) contents;

    struct CtsDevice* device = commandBuffer->device;
    struct CtsFramebuffer* framebuffer = CtsFramebufferFromHandle(pRenderPassBegin->framebuffer);
    struct CtsRenderPass* renderPass = CtsRenderPassFromHandle(pRenderPassBegin->renderPass);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    bindRenderPass(device, framebuffer, renderPass, 0);

    uint32_t lastAttachment = (pRenderPassBegin->clearValueCount < framebuffer->attachmentCount)
        ? pRenderPassBegin->clearValueCount
        : framebuffer->attachmentCount;

    for (uint32_t i = 0; i < lastAttachment; ++i) {
        const VkClearValue* clearValue = &pRenderPassBegin->pClearValues[i];
        const VkAttachmentDescription* description = &renderPass->pAttachments[i];

        if (description->format == VK_FORMAT_D16_UNORM ||
            description->format == VK_FORMAT_D32_SFLOAT
        ) {
            glClearBufferfv(GL_DEPTH, 0, &clearValue->depthStencil.depth);
        } else if (
            description->format == VK_FORMAT_D24_UNORM_S8_UINT ||
            description->format == VK_FORMAT_D32_SFLOAT_S8_UINT
        ) {
            glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue->depthStencil.depth, clearValue->depthStencil.stencil);
        } else {
            glClearBufferfv(GL_COLOR, i, clearValue->color.float32);
        }
    }

    device->activeWriteFramebuffer = framebuffer;
    device->activeRenderPass = renderPass;
}

void ctsCmdEndRenderPassImpl(
    VkCommandBuffer commandBufferHandle
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    resolveRenderPass(device, device->activeRenderPass, device->activeSubpassNumber);

    device->activeWriteFramebuffer = NULL;
    device->activeSubpassNumber = 0;
    device->activeSubpass = NULL;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ctsCmdBindDescriptorSetsImpl(
    VkCommandBuffer commandBufferHandle,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    (void) pipelineBindPoint;
    (void) pipelineLayout;
    (void) dynamicOffsetCount;
    (void) pDynamicOffsets;

    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;
    const CtsGlShader* shader = &device->activeGraphicsPipeline->shader;

    for (uint32_t i = firstSet; i < descriptorSetCount; ++i) {
        struct CtsDescriptorSet* descriptorSet = CtsDescriptorSetFromHandle(pDescriptorSets[i]);
        struct CtsDescriptorSetLayout* layout = descriptorSet->layout;

        for (uint32_t j = 0; j < layout->bindingCount; ++j) {
            const struct CtsGlDescriptorSetLayoutBinding* binding = &layout->bindings[j];
            CtsGlDescriptor* descriptor = &descriptorSet->descriptors[binding->binding];

            switch (descriptor->type) {
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                    CtsGlDescriptorImageView* imageViewContainer = &descriptor->imageViewContainer;
                    struct CtsImageView* imageView = CtsImageViewFromHandle(imageViewContainer->imageView);
                    struct CtsSampler* sampler = CtsSamplerFromHandle(imageViewContainer->sampler);

                    if (imageView != NULL) {
                        bindTexture(device, binding->binding, imageView->target, imageView->handle, NULL);
                    }

                    if (sampler != NULL) {
                        bindSampler(device, binding->binding, sampler->handle, NULL);
                    }
                } break;

                case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
                    CtsGlDescriptorBufferView* bufferViewContainer = &descriptor->bufferViewContainer;
                    struct CtsBufferView* bufferView = CtsBufferViewFromHandle(bufferViewContainer->bufferView);

                    if (bufferView != NULL) {
                        bindTexture(device, binding->binding, GL_TEXTURE_BUFFER, bufferView->handle, NULL);
                        glTexBufferRange(
                            GL_TEXTURE_BUFFER,
                            bufferView->format,
                            bufferView->buffer->memory->handle,
                            bufferView->buffer->offset + bufferView->offset,
                            bufferView->range
                        );
                    }
                } break;

                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
                    CtsGlDescriptorBuffer* bufferContainer = &descriptor->bufferContainer;
                    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferContainer->buffer);

                    if (buffer != NULL) {
                        glBindBuffer(buffer->type, buffer->memory->handle);
                        glBindBufferRange(
                            buffer->type,
                            binding->binding,
                            buffer->memory->handle,
                            bufferContainer->offset + buffer->offset,
                            bufferContainer->range
                        );
                    }
                } break;

                default: {
                    /* Unreachable */
                } break;
            }
        }
    }
}

void ctsCmdBindIndexBufferImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer bufferHandle,
    VkDeviceSize offset,
    VkIndexType indexType
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    struct CtsDevice* device = commandBuffer->device;
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    device->activeIndexBuffer.buffer = buffer;
    device->activeIndexBuffer.offset = 0;
    device->activeIndexBuffer.indexType = (indexType == VK_INDEX_TYPE_UINT16)
        ? GL_UNSIGNED_SHORT
        : GL_UNSIGNED_INT;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->memory->handle);
}

void ctsCmdBindPipelineImpl(
    VkCommandBuffer commandBufferHandle,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipelineHandle
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;
    struct CtsPipeline* pipeline = CtsPipelineFromHandle(pipelineHandle);

    if (pipeline->bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) {
        CtsGlGraphicsPipeline* graphicsPipeline = pipeline->graphics;

        if (device->activeGraphicsPipeline != graphicsPipeline) {
            device->activeGraphicsPipeline = graphicsPipeline;
            
            useProgram(device, graphicsPipeline->shader.handle, NULL);
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
}

void ctsCmdBindVertexBuffersImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    struct CtsDevice* device = commandBuffer->device;

    for (uint32_t i = 0; i < bindingCount; ++i) {
        struct CtsBuffer* buffer = CtsBufferFromHandle(pBuffers[i]);
        VkDeviceSize offset = pOffsets[i];

        const CtsGlPipelineVertexInputState* pVertexInputState = &device->activeGraphicsPipeline->vertexInputState;
        const VkVertexInputBindingDescription* vertexBindingDescription = &pVertexInputState->pVertexBindingDescriptions[i + firstBinding];

        glBindVertexBuffer(
            vertexBindingDescription->binding,
            buffer->memory->handle,
            buffer->offset + offset,
            vertexBindingDescription->stride
        );
    }
}

void ctsCmdBlitImageImpl(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImageHandle,
    VkImageLayout srcImageLayout,
    VkImage dstImageHandle,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsPhysicalDevice* physicalDevice = commandBuffer->device->physicalDevice;
    struct CtsGlContext* context = &physicalDevice->context;

    (void) srcImageLayout;
    (void) dstImageLayout;

    struct CtsImage* srcImage = CtsImageFromHandle(srcImageHandle);
    struct CtsImage* dstImage = CtsImageFromHandle(dstImageHandle);

    ctsGlHelperBlitTexture(
        commandBuffer->device,
        srcImage,
        dstImage,
        regionCount,
        pRegions,
        filter
    );
}

void ctsCmdClearAttachmentsImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t attachmentCount,
    const VkClearAttachment* pAttachments,
    uint32_t rectCount,
    const VkClearRect* pRects
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdClearColorImageImpl(
    VkCommandBuffer commandBufferHandle,
    VkImage imageHandle,
    VkImageLayout imageLayout,
    const VkClearColorValue* pColor,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsImage* image = CtsImageFromHandle(imageHandle);

    for (uint32_t i = 0; i < rangeCount; ++i) {
        const VkImageSubresourceRange* pRange = &pRanges[i];
        // TODO: Check if format is float or int
        glClearTexImage(image->handle, pRange->baseMipLevel, image->format, image->type, &pColor->float32);
    }
}

void ctsCmdClearDepthStencilImageImpl(
    VkCommandBuffer commandBufferHandle,
    VkImage imageHandle,
    VkImageLayout imageLayout,
    const VkClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsImage* image = CtsImageFromHandle(imageHandle);

    for (uint32_t i = 0; i < rangeCount; ++i) {
        const VkImageSubresourceRange* pRange = &pRanges[i];
        // TODO: Check if format is float or int
        glClearTexImage(image->handle, pRange->baseMipLevel, image->format, image->type, &pDepthStencil->depth);
    }
}

void ctsCmdCopyBufferImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer srcBufferHandle,
    VkBuffer dstBufferHandle,
    uint32_t regionCount,
    const VkBufferCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsBuffer* srcBuffer = CtsBufferFromHandle(srcBufferHandle);
    struct CtsBuffer* dstBuffer = CtsBufferFromHandle(dstBufferHandle);

    glBindBuffer(GL_COPY_READ_BUFFER, srcBuffer->memory->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer->memory->handle);

    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkBufferCopy* region = &pRegions[i];
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
    VkCommandBuffer commandBufferHandle,
    VkBuffer srcBufferHandle,
    VkImage dstImageHandle,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsBuffer* srcBuffer = CtsBufferFromHandle(srcBufferHandle);
    struct CtsImage* dstImage = CtsImageFromHandle(dstImageHandle);

    (void) dstImageLayout;

    struct CtsDevice* device = commandBuffer->device;
    GLenum target = dstImage->target;
 
    CtsGlTextureBinding previous;
    bindTexture(device, 0, target, dstImage->handle, &previous);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, srcBuffer->memory->handle);

    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkBufferImageCopy* region = &pRegions[i];
        glPixelStorei(GL_UNPACK_ROW_LENGTH, region->bufferRowLength);    

        (void) region->bufferImageHeight;
        (void) region->imageSubresource.aspectMask;
        (void) region->imageSubresource.baseArrayLayer;
        (void) region->imageSubresource.layerCount;

        ctsBufferToImage(dstImage, 0);
    }

    bindTexture(device, 0, previous.target, previous.texture, NULL);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void ctsCmdCopyImageImpl(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImageHandle,
    VkImageLayout srcImageLayout,
    VkImage dstImageHandle,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsImage* srcImage = CtsImageFromHandle(srcImageHandle);
    struct CtsImage* dstImage = CtsImageFromHandle(dstImageHandle);

    (void) srcImageLayout;
    (void) dstImageLayout;
    
    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkImageCopy* region = &pRegions[i];

        glCopyImageSubData(
            srcImage->handle,
            srcImage->target,
            region->srcSubresource.mipLevel,
            region->srcOffset.x,
            region->srcOffset.y,
            region->srcOffset.z,
            dstImage->handle,
            dstImage->target,
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
    VkCommandBuffer commandBufferHandle,
    VkImage srcImageHandle,
    VkImageLayout srcImageLayout,
    VkBuffer dstBufferHandle,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsImage* srcImage = CtsImageFromHandle(srcImageHandle);
    struct CtsBuffer* dstBuffer = CtsBufferFromHandle(dstBufferHandle);

    (void) srcImageLayout;

    GLenum target = srcImage->target;
    struct CtsDevice* device = commandBuffer->device;

    CtsGlTextureBinding previous;
    bindTexture(device, 0, target, srcImage->handle, &previous);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, dstBuffer->memory->handle);
    
    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkBufferImageCopy* region = &pRegions[i];

        (void) region->bufferImageHeight;
        (void) region->imageSubresource.aspectMask;

        glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer->memory->handle);

        void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_WRITE_ONLY);
        size_t bufferOffset = (dstBuffer->offset + region->bufferOffset);
        void* bufferPos = (char*)bufferData + dstBuffer->offset;
        size_t bufferSize = (dstBuffer->size - bufferOffset);

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
            srcImage->internalFormat,
            srcImage->type,
            (GLsizei)bufferSize,
            bufferPos
        );

        glUnmapBuffer(GL_COPY_WRITE_BUFFER);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    bindTexture(device, 0, previous.target, previous.texture, NULL);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void ctsCmdCopyQueryPoolResultsImpl(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize stride,
    VkQueryResultFlags flags
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdDispatchImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    glDispatchCompute(groupCountX, groupCountY, groupCountZ);
}

void ctsCmdDispatchBaseImpl(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    fprintf(stderr, "ctsCmdDispatchBaseImpl - not implemented");
    abort();
}

void ctsCmdDispatchIndirectImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer bufferHandle,
    VkDeviceSize offset
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer->memory->handle);
    glDispatchComputeIndirect((GLintptr) buffer->offset);
}

void ctsCmdDrawImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    glDrawArraysInstancedBaseInstance(
        device->state.primitiveTopology,
        firstVertex,
        vertexCount,
        instanceCount,
        firstInstance
    );
}

void ctsCmdDrawIndexedImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;
    CtsGlIndexBuffer* activeIndexBuffer = &device->activeIndexBuffer;

    size_t indexSize = (activeIndexBuffer->indexType == GL_UNSIGNED_SHORT)
        ? sizeof(GLushort)
        : sizeof(GLuint);

    glDrawElementsInstancedBaseVertexBaseInstance(
        device->state.primitiveTopology,
        indexCount,
        activeIndexBuffer->indexType,
        (GLvoid*)(activeIndexBuffer->offset + firstIndex * indexSize),
        instanceCount,
        vertexOffset,
        firstInstance
    );
}

void ctsCmdDrawIndexedIndirectImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer bufferHandle,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    CtsGlIndexBuffer* activeIndexBuffer = &device->activeIndexBuffer;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->memory->handle);
    glMultiDrawElementsIndirect(
        device->state.primitiveTopology,
        activeIndexBuffer->indexType,
        (GLvoid*)offset,
        drawCount,
        stride
    );
}

void ctsCmdDrawIndirectImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer bufferHandle,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->memory->handle);
    glMultiDrawArraysIndirect(
        device->state.primitiveTopology,
        (GLvoid*)offset,
        drawCount,
        stride
    );
}

void ctsCmdExecuteCommandsImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    const CtsCommandMetadata* commandMetadata;

    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        const struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(pCommandBuffers[i]);

        if (commandBuffer->state != CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
            continue;
        }

        const CtsCmdBase* next = commandBuffer->root;
        while (next != NULL) {
            commandMetadata = ctsGetCommandMetadata(next->type);
            commandMetadata->handler(next);
            next = next->pNext;
        }
    }
}

void ctsCmdFillBufferImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer dstBufferHandle,
    VkDeviceSize dstOffset,
    VkDeviceSize size,
    uint32_t data
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsBuffer* dstBuffer = CtsBufferFromHandle(dstBufferHandle);

    glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer->memory->handle);
    void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_READ_WRITE);

    size_t bufferOffset = (dstBuffer->offset + dstOffset);
    void* bufferPos = (char*)bufferData + bufferOffset;
    
    size_t availableSize = (dstBuffer->size - bufferOffset);
    size_t fillSize = (size > availableSize)
        ? availableSize
        : size;

    memset(bufferPos, data, fillSize);

    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsCmdNextSubpassImpl(
    VkCommandBuffer commandBufferHandle,
    VkSubpassContents contents
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    resolveRenderPass(device, device->activeRenderPass, device->activeSubpassNumber);
    bindRenderPass(device, device->activeReadFramebuffer, device->activeRenderPass, device->activeSubpassNumber + 1);
}

void ctsCmdPipelineBarrierImpl(
    VkCommandBuffer commandBufferHandle,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdPushConstantsImpl(
    VkCommandBuffer commandBufferHandle,
    VkPipelineLayout layout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdResetEventImpl(
    VkCommandBuffer commandBufferHandle,
    VkEvent event,
    VkPipelineStageFlags stageMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdResetQueryPoolImpl(
    VkCommandBuffer commandBufferHandle,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdResolveImageImpl(
    VkCommandBuffer commandBufferHandle,
    VkImage srcImageHandle,
    VkImageLayout srcImageLayout,
    VkImage dstImageHandle,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageResolve* pRegions
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsPhysicalDevice* physicalDevice = commandBuffer->device->physicalDevice;
    struct CtsGlContext* context = &physicalDevice->context;
    struct CtsImage* srcImage = CtsImageFromHandle(srcImageHandle);
    struct CtsImage* dstImage = CtsImageFromHandle(dstImageHandle);

    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkImageResolve* pRegion = &pRegions[i];
        
        VkImageBlit imageBlit;
        imageBlit.srcSubresource = pRegion->srcSubresource;
        imageBlit.srcOffsets[0] = pRegion->srcOffset;
        imageBlit.srcOffsets[1] = (VkOffset3D){
            .x = pRegion->srcOffset.x + pRegion->extent.width,
            .y = pRegion->srcOffset.y + pRegion->extent.height,
            .z = pRegion->srcOffset.z + pRegion->extent.depth
        };

        imageBlit.dstSubresource = pRegion->dstSubresource;
        imageBlit.dstOffsets[0] = pRegion->dstOffset;
        imageBlit.dstOffsets[1] = (VkOffset3D){
            .x = pRegion->dstOffset.x + pRegion->extent.width,
            .y = pRegion->dstOffset.y + pRegion->extent.height,
            .z = pRegion->dstOffset.z + pRegion->extent.depth
        };

        ctsGlHelperBlitTexture(
            commandBuffer->device,
            srcImage,
            dstImage,
            1,
            &imageBlit,
            VK_FILTER_LINEAR
        );
    }
}

void ctsCmdSetBlendConstantsImpl(
    VkCommandBuffer commandBufferHandle,
    const float blendConstants[4]
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)) {
        glBlendColor(
            blendConstants[0],
            blendConstants[1],
            blendConstants[2],
            blendConstants[3]
        );
    }
}

void ctsCmdSetDepthBiasImpl(
    VkCommandBuffer commandBufferHandle,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdSetDepthBoundsImpl(
    VkCommandBuffer commandBufferHandle,
    float minDepthBounds,
    float maxDepthBounds
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);

    glDepthBoundsEXT(minDepthBounds, maxDepthBounds);
}

void ctsCmdSetDeviceMaskImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t deviceMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdSetEventImpl(
    VkCommandBuffer commandBufferHandle,
    VkEvent event,
    VkPipelineStageFlags stageMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdSetLineWidthImpl(
    VkCommandBuffer commandBufferHandle,
    float lineWidth
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT)) {
        glLineWidth(lineWidth);
    }
}

void ctsCmdSetScissorImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = firstScissor; i < scissorCount; ++i) {
            const VkRect2D* scissor = &pScissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height); 
        }
    }
}

void ctsCmdSetStencilCompareMaskImpl(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t compareMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    if (faceMask & VK_STENCIL_FACE_FRONT_BIT) {
        CtsGlStencilState* state = &device->state.frontStencil;
        state->compareMask = compareMask;
        glStencilMaskSeparate(GL_FRONT, state->compareMask);
    } 

    if (faceMask & VK_STENCIL_FACE_BACK_BIT) {
        CtsGlStencilState* state = &device->state.frontStencil;
        state->compareMask = compareMask;
        glStencilMaskSeparate(GL_BACK, state->compareMask);
    }
}

void ctsCmdSetStencilReferenceImpl(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t reference
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    // Add some helper here
    if (faceMask & VK_STENCIL_FACE_FRONT_BIT) {
        CtsGlStencilState* state = &device->state.frontStencil;
        state->ref = reference;
        glStencilFuncSeparate(GL_FRONT, state->func, state->ref, state->mask);
    } 

    if (faceMask & VK_STENCIL_FACE_BACK_BIT) {
        CtsGlStencilState* state = &device->state.backStencil;
        state->ref = reference;
        glStencilFuncSeparate(GL_BACK, state->func, state->ref, state->mask);
    }
}

void ctsCmdSetStencilWriteMaskImpl(
    VkCommandBuffer commandBufferHandle,
    VkStencilFaceFlags faceMask,
    uint32_t writeMask
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    // Add some helper here
    if (faceMask & VK_STENCIL_FACE_FRONT_BIT) {
        CtsGlStencilState* state = &device->state.frontStencil;
        state->mask = writeMask;
        glStencilFuncSeparate(GL_FRONT, state->func, state->ref, state->mask);
    } 

    if (faceMask & VK_STENCIL_FACE_BACK_BIT) {
        CtsGlStencilState* state = &device->state.backStencil;
        state->mask = writeMask;
        glStencilFuncSeparate(GL_BACK, state->func, state->ref, state->mask);
    }
}

void ctsCmdSetViewportImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsDevice* device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = firstViewport; i < viewportCount; ++i) {
            const VkViewport* viewport = &pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }
}

void ctsCmdUpdateBufferImpl(
    VkCommandBuffer commandBufferHandle,
    VkBuffer dstBufferHandle,
    VkDeviceSize dstOffset,
    VkDeviceSize dataSize,
    const void* pData
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
    struct CtsBuffer* dstBuffer = CtsBufferFromHandle(dstBufferHandle);

    glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer->memory->handle);
    void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_READ_WRITE);

    size_t bufferOffset = (dstBuffer->offset + dstOffset);
    void* bufferPos = (char*)bufferData + bufferOffset;
    
    size_t availableSize = (dstBuffer->size - bufferOffset);
    size_t copySize = (dataSize > availableSize)
        ? availableSize
        : dataSize;

    memcpy(bufferPos, pData, copySize);

    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsCmdWaitEventsImpl(
    VkCommandBuffer commandBufferHandle,
    uint32_t eventCount,
    const VkEvent* events,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

void ctsCmdWriteTimestampImpl(
    VkCommandBuffer commandBufferHandle,
    VkPipelineStageFlagBits pipelineStage,
    VkQueryPool queryPool,
    uint32_t query
) {
    struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(commandBufferHandle);
}

static void* advance(void* pPtr, size_t amount) {
    return (char*)pPtr + amount;
}

static bool hasFlag(VkFlags flags, uint32_t flag) {
    return ((flags & flag) == flag);
}

static bool shouldUpdateBool(bool enable, bool* pCurrent, bool* pPrevious) {
    if (pPrevious != NULL) {
        *pPrevious = *pCurrent;
    }

    if (*pCurrent != enable) {
        *pCurrent = enable;
        return true;
    }

    return false;
}

static bool shouldUpdateHandle(GLuint handle, GLuint* pCurrent, GLuint* pPrevious) {
    if (pPrevious != NULL) {
        *pPrevious = *pCurrent;
    }

    if (*pCurrent != handle) {
        *pCurrent = handle;
        return true;
    }

    return false;
}

static void enableFeature(GLenum feature, bool enable) {
    if (enable) {
        glEnable(feature);
    } else {
        glDisable(feature);
    }
}

static void enableFeatureIndexed(GLenum feature, uint32_t index, bool enable) {
    if (enable) {
        glEnablei(feature, index);
    } else {
        glDisablei(feature, index);
    }
}

static void enableDepthClamp(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.depthClamp, pPrevious)) {
        enableFeature(GL_DEPTH_CLAMP, enable);
    }
}

static void enableRasterizerDiscard(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.rasterizerDiscard, pPrevious)) {
        enableFeature(GL_RASTERIZER_DISCARD, enable);
    }
}

static void enableCullFace(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.cullFace, pPrevious)) {
        enableFeature(GL_CULL_FACE, enable);
    }
}

static void enableDepthTest(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.depthTest, pPrevious)) {
        enableFeature(GL_DEPTH_TEST, enable);
    }
}

static void enableStencilTest(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.stencilTest, pPrevious)) {
        enableFeature(GL_STENCIL_TEST, enable);
    }
}

static void enableBlend(struct CtsDevice* device, uint32_t index, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.blend[index], pPrevious)) {
        enableFeatureIndexed(GL_BLEND, index, enable);
    }
}

static void enableColorLogicOp(struct CtsDevice* device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.colorLogicOp, pPrevious)) {
        enableFeature(GL_COLOR_LOGIC_OP, enable);
    }
}

static void useProgram(struct CtsDevice* device, GLuint program, GLuint* pPrevious) {
    if (shouldUpdateHandle(program, &device->state.program, pPrevious)) {
        glUseProgram(program);
    }
}

static void bindTexture(struct CtsDevice* device, uint32_t unit, GLenum target, uint32_t texture, CtsGlTextureBinding* pPrevious) {
    if (target == GL_NONE) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture);

    CtsGlTextureBinding* data = &device->state.texture[unit];

    if (pPrevious != NULL) {
        pPrevious->target = data->target;
        pPrevious->texture = data->texture;
    }
    
    data->target = target;
    data->texture = texture;
}

static void bindSampler(struct CtsDevice* device, uint32_t unit, uint32_t sampler, uint32_t* pPrevious) {
    glBindSampler(unit, sampler);
    uint32_t* data = &device->state.sampler[unit];

    if (pPrevious != NULL) {
        *pPrevious = *data;
    }

    *data = sampler;
}

static void bindRenderPass(struct CtsDevice* device, struct CtsFramebuffer* framebuffer, struct CtsRenderPass* renderPass, uint32_t subpassNumber) {
    const CtsGlSubpassDescription* subpassDescription = &renderPass->pSubpasses[subpassNumber];

    (void) subpassDescription->flags;
    (void) subpassDescription->pipelineBindPoint;
    (void) subpassDescription->pResolveAttachments;
    (void) subpassDescription->preserveAttachmentCount;
    (void) subpassDescription->pPreserveAttachments;

    for (uint32_t i = 0; i < subpassDescription->inputAttachmentCount; ++i) {
        const VkAttachmentReference* inputAttachment = &subpassDescription->pInputAttachments[i];
        //bindTexture(device, inputAttachment->attachment, imageView->target, imageView->handle, NULL);
    }

    uint32_t drawBufferCount = 0;
    for (uint32_t i = 0; i < subpassDescription->colorAttachmentCount; ++i) {
        const VkAttachmentReference* colorAttachment = &subpassDescription->pColorAttachments[i];
        uint32_t attachmentIndex = colorAttachment->attachment;
        
        if (attachmentIndex != VK_ATTACHMENT_UNUSED) {
            struct CtsImageView* imageView = framebuffer->ppAttachments[attachmentIndex];
            GLenum buffer = framebuffer->types[attachmentIndex];

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, buffer, imageView->target, imageView->handle, 0);
            renderPass->pDrawBuffers[drawBufferCount++] = buffer;
        }
    }

    if (subpassDescription->pDepthStencilAttachment != NULL) {
        uint32_t attachmentIndex = subpassDescription->pDepthStencilAttachment->attachment;
        struct CtsImageView* imageView = framebuffer->ppAttachments[attachmentIndex];
        GLenum buffer = framebuffer->types[attachmentIndex];
        
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, buffer, imageView->target, imageView->handle, 0);
    } else {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    glDrawBuffers(drawBufferCount, renderPass->pDrawBuffers);

    device->activeRenderPass = renderPass;
    device->activeSubpassNumber = subpassNumber;
    device->activeSubpass = subpassDescription;
}

static void bindDynamicState(
    struct CtsDevice* device,
    VkFlags state
) {
    device->dynamicStateFlags = state;
}

static void bindVertexInputState(
    struct CtsDevice* device,
    CtsGlPipelineVertexInputState* pState
) {
    glBindVertexArray(pState->vao);
}

static void bindInputAssemblyState(
    struct CtsDevice* device,
    CtsGlPipelineInputAssemblyState* pState
) {
    device->state.primitiveTopology  = pState->topology;
    device->state.primitiveRestartEnable = pState->primitiveRestartEnable;
}

static void bindTessellationState(
    struct CtsDevice* device,
    CtsGlPipelineTessellationState* pState
) {
}

static void bindViewportState(
    struct CtsDevice* device,
    CtsGlPipelineViewportState* pState
) {
    uint32_t viewportCount = pState->viewportCount;
    uint32_t scissorCount = pState->scissorCount;

    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = 0; i < viewportCount; ++i) {
            VkViewport* viewport = &pState->pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }

    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = 0; i < scissorCount; ++i) {
            VkRect2D* scissor = &pState->pScissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height);    
        }
    }
}

static void bindRasterizationState(
    struct CtsDevice* device,
    CtsGlPipelineRasterizationState* pState
) {
    enableDepthClamp(device, pState->depthClampEnable, NULL);
    enableRasterizerDiscard(device, pState->rasterizerDiscardEnable, NULL);
    enableCullFace(device, pState->cullMode != GL_NONE, NULL);

    glPolygonMode(GL_FRONT_AND_BACK, pState->polygonMode);
    glCullFace(pState->cullMode);
    glFrontFace(pState->frontFace);

    if (!hasFlag(device->activeGraphicsPipeline->dynamicState, CTS_GL_DYNAMIC_STATE_DEPTH_BIAS_BIT)) {
        if (pState->depthBiasEnable) {
            glPolygonOffset(pState->depthBiasConstantFactor, 0);
        } else {
            glPolygonOffset(0, 0);
        }

        // Not sure how to map depthBiasClamp or depthBiasSlopeFactor
    }

    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT)) {
        glLineWidth(pState->lineWidth);
    }
}

static void bindMultisampleState(
    struct CtsDevice* device,
    CtsGlPipelineMultisampleState* pState)
{
    enableFeature(GL_SAMPLE_SHADING, pState->sampleShadingEnable);
    glMinSampleShading(pState->minSampleShading);
}

static void bindDepthStencilState(
    struct CtsDevice* device,
    CtsGlPipelineDepthStencilState* pState
) {
    enableDepthTest(device, pState->depthTestEnable, NULL);
    enableStencilTest(device, pState->stencilTestEnable, NULL);

    glDepthMask(pState->depthWriteEnable ? GL_TRUE : GL_FALSE);
    glDepthFunc(pState->depthCompareOp);

    (void) pState->depthBoundsTestEnable;
    (void) pState->minDepthBounds;
    (void) pState->maxDepthBounds;

    glStencilFuncSeparate(
        GL_FRONT,
        pState->frontCompareOp,
        pState->frontReference,
        pState->frontCompareMask
    );

    glStencilOpSeparate(
        GL_FRONT,
        pState->frontFailOp,
        pState->frontDepthFailOp,
        pState->frontPassOp
    );

    glStencilMaskSeparate(GL_FRONT, pState->frontWriteMask);
    glStencilFuncSeparate(
        GL_BACK,
        pState->backCompareOp,
        pState->backReference,
        pState->backCompareMask
    );

    glStencilOpSeparate(
        GL_BACK,
        pState->backFailOp,
        pState->backDepthFailOp,
        pState->backPassOp
    );

    glStencilMaskSeparate(GL_BACK, pState->backWriteMask);
}

static void resolveRenderPass(struct CtsDevice* device, struct CtsRenderPass* renderPass, uint32_t subpassNumber) {
    const CtsGlSubpassDescription* subpassDescription = &renderPass->pSubpasses[subpassNumber];
    struct CtsFramebuffer* framebuffer = device->activeWriteFramebuffer;
    struct CtsPhysicalDevice* physicalDevice = device->physicalDevice;
    struct CtsGlContext* context = &physicalDevice->context;

    VkImageBlit imageBlit;
    imageBlit.srcSubresource = (VkImageSubresourceLayers) {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseArrayLayer = 1,
        .layerCount = 1,
        .mipLevel = 0
    };

    imageBlit.dstSubresource = (VkImageSubresourceLayers) {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseArrayLayer = 1,
        .layerCount = 1,
        .mipLevel = 0
    };

    for (uint32_t i = 0; i < subpassDescription->colorAttachmentCount; ++i) {
        const VkAttachmentReference* colorAttachment = &subpassDescription->pColorAttachments[i];
        const VkAttachmentReference* resolveAttachment = &subpassDescription->pResolveAttachments[i];

        if (colorAttachment->attachment != VK_ATTACHMENT_UNUSED && resolveAttachment->attachment != VK_ATTACHMENT_UNUSED) {
            struct CtsImage* srcImage = framebuffer->ppAttachments[colorAttachment->attachment]->image;
            struct CtsImage* dstImage = framebuffer->ppAttachments[resolveAttachment->attachment]->image;

            imageBlit.srcOffsets[0] = (VkOffset3D){0, 0, 1};
            imageBlit.srcOffsets[1] = (VkOffset3D){srcImage->width, srcImage->height, 1};
            imageBlit.dstOffsets[0] = (VkOffset3D){0, 0, 1};
            imageBlit.dstOffsets[1] = (VkOffset3D){dstImage->width, dstImage->height, 1};

            ctsGlHelperBlitTexture(device, srcImage, dstImage, 1, &imageBlit, VK_FILTER_LINEAR);
        }
    }
}

static void bindColorBlendState(
    struct CtsDevice* device,
    CtsGlPipelineColorBlendState* pState
) {
    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)) {
        glBlendColor(
            pState->blendConstants[0],
            pState->blendConstants[1],
            pState->blendConstants[2],
            pState->blendConstants[3]
        );
    }

    enableColorLogicOp(device, pState->logicOpEnable, NULL);
    glLogicOp(pState->logicOp);

    for (uint32_t i = 0; i < pState->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* attachmentState = &pState->pAttachments[i];
        enableBlend(device, i, attachmentState->blendEnable, NULL);

        glColorMaski(
            i,
            (attachmentState->colorWriteMask & VK_COLOR_COMPONENT_R_BIT) == VK_COLOR_COMPONENT_R_BIT,
            (attachmentState->colorWriteMask & VK_COLOR_COMPONENT_G_BIT) == VK_COLOR_COMPONENT_G_BIT,
            (attachmentState->colorWriteMask & VK_COLOR_COMPONENT_B_BIT) == VK_COLOR_COMPONENT_B_BIT,
            (attachmentState->colorWriteMask & VK_COLOR_COMPONENT_A_BIT) == VK_COLOR_COMPONENT_A_BIT
        );

        glBlendFuncSeparatei(
            i,
            attachmentState->srcColorBlendFactor,
            attachmentState->dstColorBlendFactor,
            attachmentState->srcAlphaBlendFactor,
            attachmentState->dstAlphaBlendFactor
        );

        glBlendEquationSeparatei(
            i,
            attachmentState->colorBlendOp,
            attachmentState->alphaBlendOp
        );
    }
}


static void* allocateCommand(
    struct CtsCommandBuffer* pCommandBuffer,
    CtsCommandType commandType,
    size_t extraDataLen
) {
    const CtsCommandMetadata* metadata = ctsGetCommandMetadata(commandType);
    CtsCmdBase* cmd = ctsAllocation(
        &pCommandBuffer->allocator,
        metadata->size + extraDataLen,
        metadata->align,
        VK_SYSTEM_ALLOCATION_SCOPE_COMMAND
    );

    cmd->type = commandType;
    cmd->pNext = NULL;

    if (pCommandBuffer->root == NULL) {
        pCommandBuffer->root = cmd;
    } else {
        pCommandBuffer->current->pNext = cmd;
    }
    
    pCommandBuffer->current = cmd;
    return cmd;
}

#ifdef __cplusplus
}
#endif