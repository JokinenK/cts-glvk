#include <assert.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/command_buffer.h>
#include <cts/commands.h>
#include <cts/command_dispatcher.h>
#include <cts/type_mapper.h>
#include <cts/typedefs/gl_enums.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_shader.h>
#include <cts/typedefs/gl_texture.h>
#include <cts/typedefs/gl_descriptor.h>
#include <private/command_buffer_private.h>
#include <private/command_pool_private.h>
#include <private/buffer_private.h>
#include <private/buffer_view_private.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/descriptor_set_private.h>
#include <private/descriptor_set_layout_private.h>
#include <private/fence_private.h>
#include <private/framebuffer_private.h>
#include <private/pipeline_private.h>
#include <private/queue_private.h>
#include <private/render_pass_private.h>
#include <private/image_private.h>
#include <private/image_view_private.h>
#include <private/sampler_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static void* advance(void* pPtr, size_t amount);
static bool hasFlag(CtsFlags flags, CtsFlags flag);

static bool shouldUpdateBool(bool enable, bool* pCurrent, bool* pPrevious);
static bool shouldUpdateHandle(GLuint handle, GLuint* pCurrent, GLuint* pPrevious);
static void enableFeature(GLenum feature, bool enable);
static void enableFeatureIndexed(GLenum feature, uint32_t index, bool enable);
static void enableDepthClamp(CtsDevice device, bool enable, bool* pPrevious);
static void enableRasterizerDiscard(CtsDevice device, bool enable, bool* pPrevious);
static void enableCullFace(CtsDevice device, bool enable, bool* pPrevious);
static void enableDepthTest(CtsDevice device, bool enable, bool* pPrevious);
static void enableStencilTest(CtsDevice device, bool enable, bool* pPrevious);
static void enableBlend(CtsDevice device, uint32_t index, bool enable, bool* pPrevious);
static void useProgram(CtsDevice device, GLuint program, GLuint* pPrevious);

static void bindTexture(CtsDevice device, uint32_t pSlot, GLenum pTarget, uint32_t pHandle, CtsGlTextureBinding* pPrevious);
static void bindSampler(CtsDevice device, uint32_t unit, uint32_t sampler, uint32_t* pPrevious);
static void bindRenderPass(CtsDevice device, CtsRenderPass renderPass, uint32_t subpassNumber);
static void bindDynamicState(CtsDevice device, CtsFlags pState);
static void bindVertexInputState(CtsDevice device, CtsGlPipelineVertexInputState* pState);
static void bindInputAssemblyState(CtsDevice device, CtsGlPipelineInputAssemblyState* pState);
static void bindTessellationState(CtsDevice device, CtsGlPipelineTessellationState* pState);
static void bindViewportState(CtsDevice device, CtsGlPipelineViewportState* pState);
static void bindRasterizationState(CtsDevice device, CtsGlPipelineRasterizationState* pState);
static void bindMultisampleState(CtsDevice device, CtsGlPipelineMultisampleState* pState);
static void bindDepthStencilState(CtsDevice device, CtsGlPipelineDepthStencilState* pState);
static void bindColorBlendState(CtsDevice device, CtsGlPipelineColorBlendState* pState);

static void* allocateCommand(CtsCommandBuffer commandBuffer, CtsCommandType commandType, size_t extraDataLen);

CtsResult ctsBeginCommandBuffer(
    CtsCommandBuffer commandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
) {
    (void) pBeginInfo->pInheritanceInfo;

    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_INITIAL) {
        commandBuffer->flags = pBeginInfo->flags;
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    return CTS_SUCCESS;
}

CtsResult ctsResetCommandBuffer(
    CtsCommandBuffer commandBuffer,
    CtsCommandBufferResetFlags flags
) {
    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING || commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    ctsLinearAllocatorReset(commandBuffer->linearAllocator);

    if (flags & CTS_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) {
        ctsLinearAllocatorTrim(commandBuffer->linearAllocator);    
    }

    return CTS_SUCCESS;
}

CtsResult ctsEndCommandBuffer(
    CtsCommandBuffer commandBuffer
) {
    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING) {
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
    }

    return CTS_SUCCESS;
}


CtsResult ctsQueueSubmit(
    CtsQueue queue,
    uint32_t submitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence fence
) {
    for (uint32_t i = 0; i < submitCount; ++i) {
        const CtsSubmitInfo* submit = &pSubmits[i];

        // TODO: Handle waitDstStageMask
        ctsWaitSemaphores(submit->waitSemaphoreCount, submit->pWaitSemaphores);

        for (uint32_t j = 0; j < submit->commandBufferCount; ++j) {
            CtsCommandBuffer commandBuffer = submit->pCommandBuffers[j];

            if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
                commandBuffer->state = CTS_COMMAND_BUFFER_STATE_PENDING;

                ctsQueueDispatch(queue, commandBuffer->root);
                ctsSignalSemaphores(submit->signalSemaphoreCount, submit->pSignalSemaphores);

                if (fence != NULL) {
                    ctsSignalFence(commandBuffer->device, fence);
                }

                if (commandBuffer->flags & CTS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
                    commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
                } else {
                    commandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
                }
            }
        }
    }
   
    return CTS_SUCCESS;
}

void ctsCmdBeginQuery(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query,
    CtsQueryControlFlags flags
) {
    CtsCmdBeginQuery* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_QUERY,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->queryPool = queryPool;
    cmd->query = query;
    cmd->flags = flags;
}

void ctsCmdEndQuery(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query
) {
    CtsCmdEndQuery* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_QUERY,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->queryPool = queryPool;
    cmd->query = query;
}

void ctsCmdBeginRenderPass(
    CtsCommandBuffer commandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents contents
) {
    size_t renderPassBeginSize = sizeof(CtsRenderPassBeginInfo);
    size_t clearValueSize = pRenderPassBegin->clearValueCount * sizeof(CtsClearValue);

    CtsCmdBeginRenderPass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_RENDER_PASS,
        renderPassBeginSize + clearValueSize
    );

    void* pRenderPassBeginCopy = advance(cmd, sizeof(CtsCmdBeginRenderPass));
    void* pClearValuesCopy = advance(pRenderPassBeginCopy, renderPassBeginSize);

    memcpy(pRenderPassBeginCopy, pRenderPassBegin, renderPassBeginSize);
    memcpy(pClearValuesCopy, pRenderPassBegin->pClearValues, clearValueSize);

    cmd->commandBuffer = commandBuffer;
    cmd->pRenderPassBegin = pRenderPassBeginCopy;
    cmd->pRenderPassBegin->pClearValues = pClearValuesCopy;
    cmd->contents = contents;
}

void ctsCmdEndRenderPass(
    CtsCommandBuffer commandBuffer
) {
    CtsCmdEndRenderPass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_RENDER_PASS,
        0
    );

    cmd->commandBuffer = commandBuffer;
}

void ctsCmdBindDescriptorSets(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    size_t descriptorSetSize = descriptorSetCount * sizeof(CtsDescriptorSet);
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

    cmd->commandBuffer = commandBuffer;
    cmd->pipelineBindPoint = pipelineBindPoint;
    cmd->firstSet = firstSet;
    cmd->descriptorSetCount = descriptorSetCount;
    cmd->pDescriptorSets = pDescriptorSetsCopy;
    cmd->dynamicOffsetCount = dynamicOffsetCount;
    cmd->pDynamicOffsets = pDynamicOffsetsCopy;
}

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    CtsIndexType indexType
) {
    CtsCmdBindIndexBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_INDEX_BUFFER,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->indexType = indexType;
}

void ctsCmdBindPipeline(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipeline pipeline
) {
    CtsCmdBindPipeline* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_PIPELINE,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->pipelineBindPoint = pipelineBindPoint;
    cmd->pipeline = pipeline;
}

void ctsCmdBindVertexBuffers(
    CtsCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
) {
    size_t buffersSize = bindingCount * sizeof(CtsBuffer);
    size_t offsetsSize = bindingCount * sizeof(CtsDeviceSize);

    CtsCmdBindVertexBuffers* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS,
        buffersSize + offsetsSize
    );

    void* pBuffersCopy = advance(cmd, sizeof(CtsCmdBindVertexBuffers));
    void* pOffsetsCopy = advance(pBuffersCopy, buffersSize);

    memcpy(pBuffersCopy, pBuffers, buffersSize);
    memcpy(pOffsetsCopy, pOffsets, offsetsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->firstBinding = firstBinding;
    cmd->bindingCount = bindingCount;
    cmd->pBuffers = pBuffersCopy;
    cmd->pOffsets = pOffsetsCopy;
}

void ctsCmdBlitImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageBlit* pRegions,
    CtsFilter filter
) {
    size_t regionsSize = regionCount * sizeof(CtsImageBlit);

    CtsCmdBlitImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BLIT_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdBlitImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
    cmd->filter = filter;
}

void ctsCmdClearAttachments(
    CtsCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t rectCount,
    const CtsClearRect* pRects
) {
    size_t attachmentsSize = attachmentCount * sizeof(CtsClearAttachment);
    size_t rectsSize = rectCount * sizeof(CtsClearRect);

    CtsCmdClearAttachments* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_ATTACHMENTS,
        attachmentsSize + rectsSize
    );

    void* pAttachmentsCopy = advance(cmd, sizeof(CtsCmdClearAttachments));
    void* pRectsCopy = advance(pAttachmentsCopy, attachmentsSize);
    
    memcpy(pAttachmentsCopy, pAttachments, attachmentsSize);
    memcpy(pRectsCopy, pRects, rectsSize);
    
    cmd->commandBuffer = commandBuffer;
    cmd->attachmentCount = attachmentCount;
    cmd->pAttachments = pAttachmentsCopy;
    cmd->rectCount = rectCount;
    cmd->pRects = pRectsCopy;
}

void ctsCmdClearColorImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearColorValue* pColor,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    size_t colorSize = sizeof(CtsClearColorValue);
    size_t rangesSize = rangeCount * sizeof(CtsImageSubresourceRange);

    CtsCmdClearColorImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE,
        colorSize + rangesSize
    );

    void* pColorCopy = advance(cmd, sizeof(CtsCmdClearColorImage));
    void* pRangesCopy = advance(pColorCopy, colorSize);

    memcpy(pColorCopy, pColor, colorSize);
    memcpy(pRangesCopy, pRanges, rangesSize);

    cmd->commandBuffer = commandBuffer;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pColor = pColorCopy;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRangesCopy;
}

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    size_t depthStencilSize = sizeof(CtsClearDepthStencilValue);
    size_t rangesSize = rangeCount * sizeof(CtsImageSubresourceRange);

    CtsCmdClearDepthStencilImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
        depthStencilSize + rangesSize
    );

    void* pDepthStencilCopy = advance(cmd, sizeof(CtsCmdClearDepthStencilImage));
    void* pRangesCopy = advance(pDepthStencilCopy, depthStencilSize);
    
    memcpy(pDepthStencilCopy, pDepthStencil, depthStencilSize);
    memcpy(pRangesCopy, pRanges, rangesSize);

    cmd->commandBuffer = commandBuffer;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pDepthStencil = pDepthStencilCopy;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRangesCopy;
}

void ctsCmdCopyBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferCopy* pRegions
) {
    size_t regionSize = regionCount * sizeof(CtsBufferCopy);

    CtsCmdCopyBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER,
        regionSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyBuffer));
    memcpy(pRegionsCopy, pRegions, regionSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcBuffer = srcBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    size_t regionSize = regionCount * sizeof(CtsBufferImageCopy);

    CtsCmdCopyBufferToImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE,
        regionSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyBufferToImage));
    memcpy(pRegionsCopy, pRegions, regionSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcBuffer = srcBuffer;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void ctsCmdCopyImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageCopy* pRegions
) {
    size_t regionsSize = regionCount * sizeof(CtsImageCopy);

    CtsCmdCopyImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    size_t regionsSize = regionCount * sizeof(CtsBufferImageCopy);

    CtsCmdCopyImageToBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdCopyImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void ctsCmdCopyQueryPoolResults(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize stride,
    CtsQueryResultFlags flags
) {
    CtsCmdCopyQueryPoolResults* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->queryPool = queryPool;
    cmd->firstQuery = firstQuery;
    cmd->queryCount = queryCount;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->stride = stride;
    cmd->flags = flags;
}

void ctsCmdDispatch(
    CtsCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    CtsCmdDispatch* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->groupCountX = groupCountX;
    cmd->groupCountY = groupCountY;
    cmd->groupCountZ = groupCountZ;
}

void ctsCmdDispatchIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset
) {
    CtsCmdDispatchIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->buffer = buffer;
    cmd->offset = offset;
}

void ctsCmdDraw(
    CtsCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    CtsCmdDraw* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->vertexCount = vertexCount;
    cmd->instanceCount = instanceCount;
    cmd->firstVertex = firstVertex;
    cmd->firstInstance = firstInstance;
}

void ctsCmdDrawIndexed(
    CtsCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
) {
    CtsCmdDrawIndexed* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->indexCount = indexCount;
    cmd->instanceCount = instanceCount;
    cmd->firstIndex = firstIndex;
    cmd->vertexOffset = vertexOffset;
    cmd->firstInstance = firstInstance;
}

void ctsCmdDrawIndexedIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    CtsCmdDrawIndexedIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->drawCount = drawCount;
    cmd->stride = stride;
}

void ctsCmdDrawIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    CtsCmdDrawIndirect* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDIRECT,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->drawCount = drawCount;
    cmd->stride = stride;
}

void ctsCmdExecuteCommands(
    CtsCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    size_t commandBuffersSize = commandBufferCount * sizeof(CtsCommandBuffer);

    CtsCmdExecuteCommands* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_EXECUTE_COMMANDS,
        commandBuffersSize
    );

    void* pCommandBuffersCopy = advance(cmd, sizeof(CtsCmdExecuteCommands));
    memcpy(pCommandBuffersCopy, pCommandBuffers, commandBuffersSize);

    cmd->commandBuffer = commandBuffer;
    cmd->commandBufferCount = commandBufferCount;
    cmd->pCommandBuffers = pCommandBuffersCopy;
}

void ctsCmdFillBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize size,
    uint32_t data
) {
    CtsCmdFillBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_FILL_BUFFER,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->size = size;
    cmd->data = data;
}

void ctsCmdNextSubpass(
    CtsCommandBuffer commandBuffer,
    CtsSubpassContents contents
) {
    CtsCmdNextSubpass* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_NEXT_SUBPASS,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->contents = contents;
}

void ctsCmdPipelineBarrier(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlags srcStageMask,
    CtsPipelineStageFlags dstStageMask,
    CtsDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
    size_t memoryBarrierSize = memoryBarrierCount * sizeof(CtsMemoryBarrier);
    size_t bufferMemoryBarrierSize = bufferMemoryBarrierCount * sizeof(CtsBufferMemoryBarrier);
    size_t imageMemoryBarrierSize = imageMemoryBarrierCount * sizeof(CtsImageMemoryBarrier);

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

    cmd->commandBuffer = commandBuffer;
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

void ctsCmdPushConstants(
    CtsCommandBuffer commandBuffer,
    CtsPipelineLayout layout,
    CtsShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues
) {
    CtsCmdPushConstants* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_PUSH_CONSTANTS,
        size
    );

    void* pValuesCopy = advance(cmd, sizeof(CtsCmdPushConstants));
    memcpy(pValuesCopy, pValues, size);

    cmd->commandBuffer = commandBuffer;
    cmd->layout = layout;
    cmd->stageFlags = stageFlags;
    cmd->offset = offset;
    cmd->size = size;
    cmd->pValues = pValuesCopy;
}

void ctsCmdResetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
    CtsCmdResetEvent* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_EVENT,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->event = event;
    cmd->stageMask = stageMask;
}

void ctsCmdResetQueryPool(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
) {
    CtsCmdResetQueryPool* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_QUERY_POOL,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->queryPool = queryPool;
    cmd->firstQuery = firstQuery;
    cmd->queryCount = queryCount;
}

void ctsCmdResolveImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageResolve* pRegions
) {
    size_t regionsSize = regionCount * sizeof(CtsImageBlit);

    CtsCmdResolveImage* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESOLVE_IMAGE,
        regionsSize
    );

    void* pRegionsCopy = advance(cmd, sizeof(CtsCmdBlitImage));
    memcpy(pRegionsCopy, pRegions, regionsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegionsCopy;
}

void ctsCmdSetBlendConstants(
    CtsCommandBuffer commandBuffer,
    const float blendConstants[4]
) {
    // TODO: This might need attention
    CtsCmdSetBlendConstants* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_BLEND_CONSTANTS,
        0
    );

    cmd->commandBuffer = commandBuffer;
    memcpy(cmd->blendConstants, blendConstants, sizeof(float[4]));
}

void ctsCmdSetDepthBias(
    CtsCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
) {
    CtsCmdSetDepthBias* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BIAS,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->depthBiasConstantFactor = depthBiasConstantFactor;
    cmd->depthBiasClamp = depthBiasClamp;
    cmd->depthBiasSlopeFactor = depthBiasSlopeFactor;
}

void ctsCmdSetDepthBounds(
    CtsCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds
) {
    CtsCmdSetDepthBounds* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BOUNDS,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->minDepthBounds = minDepthBounds;
    cmd->maxDepthBounds = maxDepthBounds;
}

void ctsCmdSetDeviceMask(
    CtsCommandBuffer commandBuffer,
    uint32_t deviceMask
) {
    CtsCmdSetDeviceMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEVICE_MASK,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->deviceMask = deviceMask;
}

void ctsCmdSetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
    CtsCmdSetEvent* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_EVENT,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->event = event;
    cmd->stageMask = stageMask;
}

void ctsCmdSetLineWidth(
    CtsCommandBuffer commandBuffer,
    float lineWidth
) {
    CtsCmdSetLineWidth* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_LINE_WIDTH,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->lineWidth = lineWidth;
}

void ctsCmdSetScissor(
    CtsCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const CtsRect2D* pScissors
) {
    size_t scissorsSize = scissorCount * sizeof(CtsRect2D);

    CtsCmdSetScissor* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_SCISSOR,
        scissorsSize
    );

    void* pScissorsCopy = advance(cmd, sizeof(CtsCmdSetScissor));
    memcpy(pScissorsCopy, pScissors, scissorsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->firstScissor = firstScissor;
    cmd->scissorCount = scissorCount;
    cmd->pScissors = pScissorsCopy;
}

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t compareMask
) {
    CtsCmdSetStencilCompareMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->faceMask = faceMask;
    cmd->compareMask = compareMask;
}

void ctsCmdSetStencilReference(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t reference
) {
    CtsCmdSetStencilReference* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_REFERENCE,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->faceMask = faceMask;
    cmd->reference = reference;
}

void ctsCmdSetStencilWriteMask(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t writeMask
) {
    CtsCmdSetStencilWriteMask* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->faceMask = faceMask;
    cmd->writeMask = writeMask;
}

void ctsCmdSetViewport(
    CtsCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const CtsViewport* pViewports
) {
    size_t viewportsSize = viewportCount * sizeof(CtsViewport);

    CtsCmdSetViewport* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_VIEWPORT,
        viewportsSize
    );

    void* pViewportsCopy = advance(cmd, sizeof(CtsCmdSetViewport));
    memcpy(pViewportsCopy, pViewports, viewportsSize);

    cmd->commandBuffer = commandBuffer;
    cmd->firstViewport = firstViewport;
    cmd->viewportCount = viewportCount;
    cmd->pViewports = pViewportsCopy;
}

void ctsCmdUpdateBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize dataSize,
    const void* pData
) {
    CtsCmdUpdateBuffer* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_UPDATE_BUFFER,
        dataSize
    );

    void* pDataCopy = advance(cmd, sizeof(CtsCmdUpdateBuffer));
    memcpy(pDataCopy, pData, dataSize); 

    cmd->commandBuffer = commandBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->dataSize = dataSize;
    cmd->pData = pDataCopy;
}

void ctsCmdWaitEvents(
    CtsCommandBuffer commandBuffer,
    uint32_t eventCount,
    const CtsEvent* events,
    CtsPipelineStageFlags srcStageMask,
    CtsPipelineStageFlags dstStageMask,
    uint32_t memoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
    size_t memoryBarriersSize = memoryBarrierCount * sizeof(CtsMemoryBarrier);
    size_t bufferMemoryBarriersSize = bufferMemoryBarrierCount * sizeof(CtsBufferMemoryBarrier);
    size_t imageMemoryBarriersSize = imageMemoryBarrierCount * sizeof(CtsImageMemoryBarrier);

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

    cmd->commandBuffer = commandBuffer;
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

void ctsCmdWriteTimestamp(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlagBits pipelineStage,
    CtsQueryPool queryPool,
    uint32_t query
) {
    CtsCmdWriteTimestamp* cmd = allocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_WRITE_TIMESTAMP,
        0
    );

    cmd->commandBuffer = commandBuffer;
    cmd->pipelineStage = pipelineStage;
    cmd->queryPool = queryPool;
    cmd->query = query;
}

void ctsCmdBeginQueryImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query,
    CtsQueryControlFlags flags
) {
    // TODO: Implement this
    //glBeginQuery()
}

void ctsCmdEndQueryImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query
) {
    //glEndQuery()
}

void ctsCmdBeginRenderPassImpl(
    CtsCommandBuffer commandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents contents
) {
    (void) pRenderPassBegin->renderArea;
    (void) contents;

    CtsDevice device = commandBuffer->device;
    CtsFramebuffer framebuffer = pRenderPassBegin->framebuffer;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->handle);
    bindRenderPass(device, pRenderPassBegin->renderPass, 0);

    uint32_t lastAttachment = (pRenderPassBegin->clearValueCount < framebuffer->attachmentCount)
        ? pRenderPassBegin->clearValueCount
        : framebuffer->attachmentCount;

    for (uint32_t i = 0; i < lastAttachment; ++i) {
        const CtsClearValue* clearValue = &pRenderPassBegin->pClearValues[i];
        const CtsAttachmentDescription* description = &pRenderPassBegin->renderPass->pAttachments[i];

        if (description->format == CTS_FORMAT_D16_UNORM ||
            description->format == CTS_FORMAT_D32_SFLOAT
        ) {
            glClearBufferfv(GL_DEPTH, 0, &clearValue->depthStencil.depth);
        } else if (
            description->format == CTS_FORMAT_D24_UNORM_S8_UINT ||
            description->format == CTS_FORMAT_D32_SFLOAT_S8_UINT
        ) {
            glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue->depthStencil.depth,  clearValue->depthStencil.stencil);
        } else {
            glClearBufferfv(GL_COLOR, i, clearValue->color.float32);
        }
    }

    device->activeFramebuffer = framebuffer;
}

void ctsCmdEndRenderPassImpl(
    CtsCommandBuffer commandBuffer
) {
    CtsDevice device = commandBuffer->device;
    device->activeFramebuffer = NULL;
    device->activeSubpass = 0;
}

void ctsCmdBindDescriptorSetsImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    (void) pipelineBindPoint;
    (void) pipelineLayout;
    (void) dynamicOffsetCount;
    (void) pDynamicOffsets;

    CtsDevice device = commandBuffer->device;
    const CtsGlShader* shader = &device->activeGraphicsPipeline->shader;

    for (uint32_t i = firstSet; i < descriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = pDescriptorSets[i];
        CtsDescriptorSetLayout layout = descriptorSet->layout;

        for (uint32_t j = 0; j < layout->bindingCount; ++j) {
            const CtsGlDescriptorSetLayoutBinding* binding = &layout->bindings[j];
            CtsGlDescriptor* descriptor = &descriptorSet->descriptors[binding->binding];

            switch (descriptor->type) {
                case CTS_DESCRIPTOR_TYPE_SAMPLER:
                case CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                case CTS_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                case CTS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                case CTS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                    CtsGlDescriptorImageView* imageViewContainer = &descriptor->imageViewContainer;
                    CtsImageView imageView = imageViewContainer->imageView;
                    CtsSampler sampler = imageViewContainer->sampler;

                    if (imageView != NULL) {
                        bindTexture(device, binding->binding, imageView->target, imageView->handle, NULL);
                    }

                    if (sampler != NULL) {
                        bindSampler(device, binding->binding, sampler->handle, NULL);
                    }
                } break;

                case CTS_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                case CTS_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
                    CtsGlDescriptorBufferView* bufferViewContainer = &descriptor->bufferViewContainer;
                    CtsBufferView bufferView = bufferViewContainer->bufferView;

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

                case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
                    CtsGlDescriptorBuffer* bufferContainer = &descriptor->bufferContainer;
                    CtsBuffer buffer = bufferContainer->buffer;

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
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    CtsIndexType indexType
) {
    CtsDevice device = commandBuffer->device;

    device->activeIndexBuffer.buffer = buffer;
    device->activeIndexBuffer.offset = 0;
    device->activeIndexBuffer.indexType = (indexType == CTS_INDEX_TYPE_UINT16)
        ? GL_UNSIGNED_SHORT
        : GL_UNSIGNED_INT;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->memory->handle);
}

void ctsCmdBindPipelineImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipeline pipeline
) {
    CtsDevice device = commandBuffer->device;

    if (pipeline->bindPoint == CTS_PIPELINE_BIND_POINT_GRAPHICS) {
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
    CtsCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
) {
    CtsDevice device = commandBuffer->device;

    for (uint32_t i = 0; i < bindingCount; ++i) {
        CtsBuffer buffer = pBuffers[i];
        CtsDeviceSize offset = pOffsets[i];

        const CtsGlPipelineVertexInputState* pVertexInputState = &device->activeGraphicsPipeline->vertexInputState;
        const CtsVertexInputBindingDescription* vertexBindingDescription = &pVertexInputState->pVertexBindingDescriptions[i + firstBinding];

        glBindVertexBuffer(
            vertexBindingDescription->binding,
            buffer->memory->handle,
            buffer->offset + offset,
            vertexBindingDescription->stride
        );
    }
}

void ctsCmdBlitImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageBlit* pRegions,
    CtsFilter filter
) {
}

void ctsCmdClearAttachmentsImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t rectCount,
    const CtsClearRect* pRects
) {
}

void ctsCmdClearColorImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearColorValue* pColor,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
}

void ctsCmdClearDepthStencilImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
}

void ctsCmdCopyBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferCopy* pRegions
) {
    glBindBuffer(GL_COPY_READ_BUFFER, srcBuffer->memory->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer->memory->handle);

    for (uint32_t i = 0; i < regionCount; ++i) {
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
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    (void) dstImageLayout;

    CtsDevice device = commandBuffer->device;
    GLenum target = dstImage->target;
 
    CtsGlTextureBinding previous;
    bindTexture(device, 0, target, dstImage->handle, &previous);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, srcBuffer->memory->handle);
    
    for (uint32_t i = 0; i < regionCount; ++i) {
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
                dstImage->format,
                dstImage->type,
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
                dstImage->format,
                dstImage->type,
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
                dstImage->format,
                dstImage->type,
                (const void*)region->bufferOffset
            );
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    bindTexture(device, 0, previous.target, previous.texture, NULL);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void ctsCmdCopyImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageCopy* pRegions
) {
    (void) srcImageLayout;
    (void) dstImageLayout;
    
    for (uint32_t i = 0; i < regionCount; ++i) {
        const CtsImageCopy* region = &pRegions[i];

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
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    (void) srcImageLayout;

    GLenum target = srcImage->target;
    CtsDevice device = commandBuffer->device;

    CtsGlTextureBinding previous;
    bindTexture(device, 0, target, srcImage->handle, &previous);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, dstBuffer->memory->handle);
    
    for (uint32_t i = 0; i < regionCount; ++i) {
        const CtsBufferImageCopy* region = &pRegions[i];

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
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize stride,
    CtsQueryResultFlags flags
) {
}

void ctsCmdDispatchImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
) {
    glDispatchCompute(groupCountX, groupCountY, groupCountZ);
}

void ctsCmdDispatchIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset
) {
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer->memory->handle);
    glDispatchComputeIndirect((GLintptr) buffer->offset);
}

void ctsCmdDrawImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    CtsDevice device = commandBuffer->device;

    glDrawArraysInstancedBaseInstance(
        device->activeGraphicsPipeline->inputAssemblyState.topology,
        firstVertex,
        vertexCount,
        instanceCount,
        firstInstance
    );
}

void ctsCmdDrawIndexedImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
) {
    CtsDevice device = commandBuffer->device;
    CtsGlIndexBuffer* activeIndexBuffer = &device->activeIndexBuffer;

    size_t indexSize = (activeIndexBuffer->indexType == GL_UNSIGNED_SHORT)
        ? sizeof(GLushort)
        : sizeof(GLuint);

    glDrawElementsInstancedBaseVertexBaseInstance(
        device->activeGraphicsPipeline->inputAssemblyState.topology,
        indexCount,
        activeIndexBuffer->indexType,
        (GLvoid*)(activeIndexBuffer->offset + firstIndex * indexSize),
        instanceCount,
        vertexOffset,
        firstInstance
    );
}

void ctsCmdDrawIndexedIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    CtsDevice device = commandBuffer->device;
    CtsGlIndexBuffer* activeIndexBuffer = &device->activeIndexBuffer;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->memory->handle);
    glMultiDrawElementsIndirect(
        device->activeGraphicsPipeline->inputAssemblyState.topology,
        activeIndexBuffer->indexType,
        (GLvoid*)offset,
        drawCount,
        stride
    );
}

void ctsCmdDrawIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
) {
    CtsDevice device = commandBuffer->device;
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->memory->handle);
    glMultiDrawArraysIndirect(
        device->activeGraphicsPipeline->inputAssemblyState.topology,
        (GLvoid*)offset,
        drawCount,
        stride
    );
}

void ctsCmdExecuteCommandsImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    const CtsCommandMetadata* commandMetadata;
    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        const CtsCommandBuffer commandBuffer = pCommandBuffers[i];

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
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize size,
    uint32_t data
) {
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
    CtsCommandBuffer commandBuffer,
    CtsSubpassContents contents
) {
    CtsDevice device = commandBuffer->device;
    bindRenderPass(device, device->activeRenderPass, device->activeSubpassNumber + 1);
}

void ctsCmdPipelineBarrierImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlags srcStageMask,
    CtsPipelineStageFlags dstStageMask,
    CtsDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
}

void ctsCmdPushConstantsImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineLayout layout,
    CtsShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues
) {
}

void ctsCmdResetEventImpl(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
}

void ctsCmdResetQueryPoolImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
) {
}

void ctsCmdResolveImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageResolve* pRegions
) {
}

void ctsCmdSetBlendConstantsImpl(
    CtsCommandBuffer commandBuffer,
    const float blendConstants[4]
) {
    CtsDevice device = commandBuffer->device;

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
    CtsCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
) {
}

void ctsCmdSetDepthBoundsImpl(
    CtsCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds
) {
}

void ctsCmdSetDeviceMaskImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t deviceMask
) {
}

void ctsCmdSetEventImpl(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
}

void ctsCmdSetLineWidthImpl(
    CtsCommandBuffer commandBuffer,
    float lineWidth
) {
    CtsDevice device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT)) {
        glLineWidth(lineWidth);
    }
}

void ctsCmdSetScissorImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const CtsRect2D* pScissors
) {
    CtsDevice device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = firstScissor; i < scissorCount; ++i) {
            const CtsRect2D* scissor = &pScissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height); 
        }
    }
}

void ctsCmdSetStencilCompareMaskImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t compareMask
) {
}

void ctsCmdSetStencilReferenceImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t reference
) {
}

void ctsCmdSetStencilWriteMaskImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t writeMask
) {
}

void ctsCmdSetViewportImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const CtsViewport* pViewports
) {
    CtsDevice device = commandBuffer->device;

    if (hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = firstViewport; i < viewportCount; ++i) {
            const CtsViewport* viewport = &pViewports[i];

            float y = viewport->y + viewport->height;
            float height = -viewport->height;

            glViewportIndexedf(i, viewport->x, y, viewport->width, height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }
}

void ctsCmdUpdateBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize dataSize,
    const void* pData
) {
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
    CtsCommandBuffer commandBuffer,
    uint32_t eventCount,
    const CtsEvent* events,
    CtsPipelineStageFlags srcStageMask,
    CtsPipelineStageFlags dstStageMask,
    uint32_t memoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
}

void ctsCmdWriteTimestampImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlagBits pipelineStage,
    CtsQueryPool queryPool,
    uint32_t query
) {
}

static void* advance(void* pPtr, size_t amount) {
    return (char*)pPtr + amount;
}

static bool hasFlag(CtsFlags flags, CtsFlagBit flag) {
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

static void enableDepthClamp(CtsDevice device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.depthClamp, pPrevious)) {
        enableFeature(GL_DEPTH_CLAMP, enable);
    }
}

static void enableRasterizerDiscard(CtsDevice device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.rasterizerDiscard, pPrevious)) {
        enableFeature(GL_RASTERIZER_DISCARD, enable);
    }
}

static void enableCullFace(CtsDevice device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.cullFace, pPrevious)) {
        enableFeature(GL_CULL_FACE, enable);
    }
}

static void enableDepthTest(CtsDevice device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.depthTest, pPrevious)) {
        enableFeature(GL_DEPTH_TEST, enable);
    }
}

static void enableStencilTest(CtsDevice device, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.stencilTest, pPrevious)) {
        enableFeature(GL_STENCIL_TEST, enable);
    }
}

static void enableBlend(CtsDevice device, uint32_t index, bool enable, bool* pPrevious) {
    if (shouldUpdateBool(enable, &device->state.blend[index], pPrevious)) {
        enableFeatureIndexed(GL_BLEND, index, enable);
    }
}

static void useProgram(CtsDevice device, GLuint program, GLuint* pPrevious) {
    if (shouldUpdateHandle(program, &device->state.program, pPrevious)) {
        glUseProgram(program);
    }
}

static void bindTexture(CtsDevice device, uint32_t unit, GLenum target, uint32_t texture, CtsGlTextureBinding* pPrevious) {
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

static void bindSampler(CtsDevice device, uint32_t unit, uint32_t sampler, uint32_t* pPrevious) {
    glBindSampler(unit, sampler);
    uint32_t* data = &device->state.sampler[unit];

    if (pPrevious != NULL) {
        *pPrevious = *data;
    }

    *data = sampler;
}

static void bindRenderPass(CtsDevice device, CtsRenderPass renderPass, uint32_t subpassNumber) {
    const CtsGlSubpassDescription* subpassDescription = &renderPass->pSubpasses[subpassNumber];

    (void) subpassDescription->flags;
    (void) subpassDescription->pipelineBindPoint;
    (void) subpassDescription->pResolveAttachments;
    (void) subpassDescription->preserveAttachmentCount;
    (void) subpassDescription->pPreserveAttachments;

    for (uint32_t i = 0; i < subpassDescription->inputAttachmentCount; ++i) {
        const CtsAttachmentReference* inputAttachment = &subpassDescription->pInputAttachments[i];
        //bindTexture(device, inputAttachment->attachment, imageView->target, imageView->handle, NULL);
    }

    for (uint32_t i = 0; i < subpassDescription->colorAttachmentCount; ++i) {
        renderPass->pDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + subpassDescription->pColorAttachments[i].attachment;
    }

    glDrawBuffers(subpassDescription->colorAttachmentCount, renderPass->pDrawBuffers);

    device->activeRenderPass = renderPass;
    device->activeSubpassNumber = subpassNumber;
    device->activeSubpass = subpassDescription;
}

static void bindDynamicState(
    CtsDevice device,
    CtsFlags state
) {
    device->dynamicStateFlags = state;
}

static void bindVertexInputState(
    CtsDevice device,
    CtsGlPipelineVertexInputState* pState
) {
    glBindVertexArray(pState->vao);
}

static void bindInputAssemblyState(
    CtsDevice device,
    CtsGlPipelineInputAssemblyState* pState
) {
}

static void bindTessellationState(
    CtsDevice device,
    CtsGlPipelineTessellationState* pState
) {
}

static void bindViewportState(
    CtsDevice device,
    CtsGlPipelineViewportState* pState
) {
    uint32_t viewportCount = pState->viewportCount;
    uint32_t scissorCount = pState->scissorCount;

    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = 0; i < viewportCount; ++i) {
            CtsViewport* viewport = &pState->pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }

    if (!hasFlag(device->dynamicStateFlags, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
        for (uint32_t i = 0; i < scissorCount; ++i) {
            CtsRect2D* scissor = &pState->pScissors[i];
            glScissorIndexed(i, scissor->offset.x, scissor->offset.y, scissor->extent.width, scissor->extent.height);    
        }
    }
}

static void bindRasterizationState(
    CtsDevice device,
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
    CtsDevice device,
    CtsGlPipelineMultisampleState* pState)
{
}

static void bindDepthStencilState(
    CtsDevice device,
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

static void bindColorBlendState(
    CtsDevice device,
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

    for (uint32_t i = 0; i < pState->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* attachmentState = &pState->pAttachments[i];
        enableBlend(device, i, attachmentState->blendEnable, NULL);

        glColorMaski(
            i,
            (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_R_BIT) == CTS_COLOR_COMPONENT_R_BIT,
            (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_G_BIT) == CTS_COLOR_COMPONENT_G_BIT,
            (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_B_BIT) == CTS_COLOR_COMPONENT_B_BIT,
            (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_A_BIT) == CTS_COLOR_COMPONENT_A_BIT
        );

        glBlendFuncSeparateiARB(
            i,
            attachmentState->srcColorBlendFactor,
            attachmentState->dstColorBlendFactor,
            attachmentState->srcAlphaBlendFactor,
            attachmentState->dstAlphaBlendFactor
        );

        glBlendEquationSeparateiARB(
            i,
            attachmentState->colorBlendOp,
            attachmentState->alphaBlendOp
        );
    }
}


static void* allocateCommand(
    CtsCommandBuffer commandBuffer,
    CtsCommandType commandType,
    size_t extraDataLen
) {
    const CtsCommandMetadata* metadata = ctsGetCommandMetadata(commandType);
    CtsCmdBase* cmd = ctsAllocation(
        &commandBuffer->allocator,
        metadata->size + extraDataLen,
        metadata->align,
        CTS_SYSTEM_ALLOCATION_SCOPE_COMMAND
    );

    cmd->type = commandType;
    cmd->pNext = NULL;

    if (commandBuffer->root == NULL) {
        commandBuffer->root = cmd;
    } else {
        commandBuffer->current->pNext = cmd;
    }
    
    commandBuffer->current = cmd;
    return cmd;
}

#ifdef __cplusplus
}
#endif