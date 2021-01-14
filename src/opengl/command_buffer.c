#include <assert.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/command_buffer.h>
#include <cts/commands.h>
#include <cts/command_dispatcher.h>
#include <cts/type_mapper.h>
#include <cts/typedefs/gl_color_blend_state_changes.h>
#include <cts/typedefs/gl_depth_stencil_state_changes.h>
#include <cts/typedefs/gl_enums.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_rasterization_state_changes.h>
#include <cts/typedefs/gl_shader.h>
#include <cts/typedefs/gl_texture.h>
#include <private/command_buffer_private.h>
#include <private/command_pool_private.h>
#include <private/buffer_private.h>
#include <private/buffer_view_private.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/descriptor_private.h>
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

static bool hasFlag(CtsFlags flags, CtsFlags flag);
static void bindTexture(CtsDevice device, uint32_t pSlot, GLenum pTarget, uint32_t pHandle, CtsTextureBinding* pPrevious);
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

CtsResult ctsAllocateCommandBuffers(
    CtsDevice device,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
) {
    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    const CtsAllocationCallbacks* allocator = &pAllocateInfo->commandPool->poolAllocator;

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = ctsAllocation(
            allocator,
            sizeof(struct CtsCommandBuffer),
            alignof(struct CtsCommandBuffer),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (commandBuffer == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        commandBuffer->device = device;
        commandBuffer->pool = pAllocateInfo->commandPool;
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->level = pAllocateInfo->level;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INITIAL;

        pCommandBuffers[i] = commandBuffer;
    }

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = NULL;
    }

    return result;
}

void ctsFreeCommandBuffers(
    CtsDevice device,
    CtsCommandPool commandPool,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    const CtsAllocationCallbacks* allocator = &commandPool->poolAllocator;

    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = pCommandBuffers[i];
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
        ctsFree(allocator, commandBuffer);
    }
}

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
    CtsResult result;
    CtsQueueSubmit cmd;
    cmd.base.type = CTS_COMMAND_QUEUE_SUBMIT;
    cmd.base.pNext = NULL;

    cmd.queue = queue,
    cmd.submitCount = submitCount;
    cmd.pSubmits = pSubmits;
    cmd.fence = fence;
    cmd.pResult = &result;

    CtsDevice device = queue->device;
    ctsQueueDispatch(queue, &cmd.base, device->dispatch.mutex, device->dispatch.conditionVariable);

    return result;
}

void ctsCmdBeginQuery(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query,
    CtsQueryControlFlags flags
) {
    CtsCmdBeginQuery* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_QUERY
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
    CtsCmdEndQuery* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_QUERY
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
    CtsCmdBeginRenderPass* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BEGIN_RENDER_PASS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->pRenderPassBegin = pRenderPassBegin;
    cmd->contents = contents;
}

void ctsCmdEndRenderPass(
    CtsCommandBuffer commandBuffer
) {
    CtsCmdEndRenderPass* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_END_RENDER_PASS
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
    uint32_t pDynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    CtsCmdBindDescriptorSets* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->pipelineBindPoint = pipelineBindPoint;
    cmd->firstSet = firstSet;
    cmd->descriptorSetCount = descriptorSetCount;
    cmd->pDescriptorSets = pDescriptorSets;
    cmd->dynamicOffsetCount = pDynamicOffsetCount;
    cmd->pDynamicOffsets = pDynamicOffsets;
}

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    CtsIndexType indexType
) {
    CtsCmdBindIndexBuffer* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_INDEX_BUFFER
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
    CtsCmdBindPipeline* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_PIPELINE
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
    CtsCmdBindVertexBuffers* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->firstBinding = firstBinding;
    cmd->bindingCount = bindingCount;
    cmd->pBuffers = pBuffers;
    cmd->pOffsets = pOffsets;
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
    CtsCmdBlitImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_BLIT_IMAGE
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
    cmd->filter = filter;
}

void ctsCmdClearAttachments(
    CtsCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t rectCount,
    const CtsClearRect* pRects
) {
    CtsCmdClearAttachments* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_ATTACHMENTS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->attachmentCount = attachmentCount;
    cmd->pAttachments = pAttachments;
    cmd->rectCount = rectCount;
    cmd->pRects = pRects;
}

void ctsCmdClearColorImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearColorValue* pColor,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearColorImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE
    );

    cmd->commandBuffer = commandBuffer;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pColor = pColor;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRanges;
}

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearDepthStencilImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE
    );

    cmd->commandBuffer = commandBuffer;
    cmd->image = image;
    cmd->imageLayout = imageLayout;
    cmd->pDepthStencil = pDepthStencil;
    cmd->rangeCount = rangeCount;
    cmd->pRanges = pRanges;
}

void ctsCmdCopyBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferCopy* pRegions
) {
    CtsCmdCopyBuffer* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcBuffer = srcBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
}

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyBufferToImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE
    );
    cmd->commandBuffer = commandBuffer;
    cmd->srcBuffer = srcBuffer;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
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
    CtsCmdCopyImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
}

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyImageToBuffer* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstBuffer = dstBuffer;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
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
    CtsCmdCopyQueryPoolResults* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS
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
    CtsCmdDispatch* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH
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
    CtsCmdDispatchIndirect* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DISPATCH_INDIRECT
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
    CtsCmdDraw* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW
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
    CtsCmdDrawIndexed* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED
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
    CtsCmdDrawIndexedIndirect* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT
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
    CtsCmdDrawIndirect* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_DRAW_INDIRECT
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
    CtsCmdExecuteCommands* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_EXECUTE_COMMANDS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->commandBufferCount = commandBufferCount;
    cmd->pCommandBuffers = pCommandBuffers;
}

void ctsCmdFillBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize size,
    uint32_t data
) {
    CtsCmdFillBuffer* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_FILL_BUFFER
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
    CtsCmdNextSubpass* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_NEXT_SUBPASS
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
    CtsCmdPipelineBarrier* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_PIPELINE_BARRIER
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcStageMask = srcStageMask;
    cmd->dstStageMask = dstStageMask;
    cmd->dependencyFlags = dependencyFlags;
    cmd->memoryBarrierCount = memoryBarrierCount;
    cmd->pMemoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = bufferMemoryBarrierCount;
    cmd->pBufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = imageMemoryBarrierCount;
    cmd->pImageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdPushConstants(
    CtsCommandBuffer commandBuffer,
    CtsPipelineLayout layout,
    CtsShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* values
) {
    CtsCmdPushConstants* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_PUSH_CONSTANTS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->layout = layout;
    cmd->stageFlags = stageFlags;
    cmd->offset = offset;
    cmd->size = size;
    cmd->values = values;
}

void ctsCmdResetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
    CtsCmdResetEvent* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_EVENT
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
    CtsCmdResetQueryPool* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESET_QUERY_POOL
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
    CtsCmdResolveImage* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_RESOLVE_IMAGE
    );

    cmd->commandBuffer = commandBuffer;
    cmd->srcImage = srcImage;
    cmd->srcImageLayout = srcImageLayout;
    cmd->dstImage = dstImage;
    cmd->dstImageLayout = dstImageLayout;
    cmd->regionCount = regionCount;
    cmd->pRegions = pRegions;
}

void ctsCmdSetBlendConstants(
    CtsCommandBuffer commandBuffer,
    const float blendConstants[4]
) {
    CtsCmdSetBlendConstants* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_BLEND_CONSTANTS
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
    CtsCmdSetDepthBias* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BIAS
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
    CtsCmdSetDepthBounds* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BOUNDS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->minDepthBounds = minDepthBounds;
    cmd->maxDepthBounds = maxDepthBounds;
}

void ctsCmdSetDeviceMask(
    CtsCommandBuffer commandBuffer,
    uint32_t deviceMask
) {
    CtsCmdSetDeviceMask* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_DEVICE_MASK
    );

    cmd->commandBuffer = commandBuffer;
    cmd->deviceMask = deviceMask;
}

void ctsCmdSetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
) {
    CtsCmdSetEvent* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_EVENT
    );

    cmd->commandBuffer = commandBuffer;
    cmd->event = event;
    cmd->stageMask = stageMask;
}

void ctsCmdSetLineWidth(
    CtsCommandBuffer commandBuffer,
    float lineWidth
) {
    CtsCmdSetLineWidth* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_LINE_WIDTH
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
    CtsCmdSetScissor* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_SCISSOR
    );

    cmd->commandBuffer = commandBuffer;
    cmd->firstScissor = firstScissor;
    cmd->scissorCount = scissorCount;
    cmd->pScissors = pScissors;
}

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t compareMask
) {
    CtsCmdSetStencilCompareMask* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK
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
    CtsCmdSetStencilReference* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_REFERENCE
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
    CtsCmdSetStencilWriteMask* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK
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
    CtsCmdSetViewport* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_SET_VIEWPORT
    );

    cmd->commandBuffer = commandBuffer;
    cmd->firstViewport = firstViewport;
    cmd->viewportCount = viewportCount;
    cmd->pViewports = pViewports;
}

void ctsCmdUpdateBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize dataSize,
    const void* pData
) {
    CtsCmdUpdateBuffer* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_UPDATE_BUFFER
    );

    cmd->commandBuffer = commandBuffer;
    cmd->dstBuffer = dstBuffer;
    cmd->dstOffset = dstOffset;
    cmd->dataSize = dataSize;
    cmd->pData = pData;
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
    CtsCmdWaitEvents* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_WAIT_EVENTS
    );

    cmd->commandBuffer = commandBuffer;
    cmd->eventCount = eventCount;
    cmd->events = events;
    cmd->srcStageMask = srcStageMask;
    cmd->dstStageMask = dstStageMask;
    cmd->memoryBarrierCount = memoryBarrierCount;
    cmd->pMemoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = bufferMemoryBarrierCount;
    cmd->pBufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = imageMemoryBarrierCount;
    cmd->pImageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdWriteTimestamp(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlagBits pipelineStage,
    CtsQueryPool queryPool,
    uint32_t query
) {
    CtsCmdWriteTimestamp* cmd = ctsCommandBufferAllocateCommand(
        commandBuffer,
        CTS_COMMAND_CMD_WRITE_TIMESTAMP
    );

    cmd->commandBuffer = commandBuffer;
    cmd->pipelineStage = pipelineStage;
    cmd->queryPool = queryPool;
    cmd->query = query;
}

void* ctsCommandBufferAllocateCommand(
    CtsCommandBuffer commandBuffer,
    CtsCommandType commandType
) {
    assert(commandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING);

    const CtsCommandMetadata* metadata = ctsGetCommandMetadata(commandType);
    CtsCmdBase* cmd = ctsAllocation(
        &commandBuffer->pool->poolAllocator,
        metadata->size,
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

CtsResult ctsQueueSubmitImpl(
    CtsQueue queue,
    uint32_t submitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence fence
) {
    if (fence != NULL) {
        ctsWaitForFencesImpl(queue->device, 1, &fence, true, UINT64_MAX);
    }

    CtsQueueItem queueItem;
    for (uint32_t i = 0; i < submitCount; ++i) {
        const CtsSubmitInfo* submit = &pSubmits[i];

        // TODO: Handle waitDstStageMask
        ctsWaitSemaphores(submit->waitSemaphoreCount, submit->pWaitSemaphores);

        for (uint32_t j = 0; j < submit->commandBufferCount; ++j) {
            CtsCommandBuffer commandBuffer = submit->pCommandBuffers[j];

            if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
                commandBuffer->state = CTS_COMMAND_BUFFER_STATE_PENDING;

                CtsQueueFinish* queueFinishCmd = ctsCommandBufferAllocateCommand(
                    commandBuffer,
                    CTS_COMMAND_QUEUE_FINISH
                );

                queueFinishCmd->semaphoreCount = submit->signalSemaphoreCount;
                queueFinishCmd->pSemaphores = submit->pSignalSemaphores;
                queueFinishCmd->commandBuffer = commandBuffer;

                queueItem.cmd = commandBuffer->root;
                queueItem.pFinished = NULL;
                queueItem.conditionVariable = NULL;
                ctsQueuePush(queue, &queueItem);
            }
        }
    }
   
    return CTS_SUCCESS;
}

void ctsQueueFinishImpl(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores,
    CtsCommandBuffer commandBuffer
) {
    if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_PENDING) {
        if (commandBuffer->flags & CTS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
            commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
        } else {
            commandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
        }

        ctsSignalSemaphores(semaphoreCount, pSemaphores);
    }
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

    for (uint32_t i = firstSet; i < descriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = pDescriptorSets[i];
        CtsDescriptorSetLayout layout = descriptorSet->layout;

        for (uint32_t j = 0; j < layout->bindingCount; ++j) {
            const CtsGlDescriptorSetLayoutBinding* binding = &layout->bindings[j];
            CtsDescriptor descriptor = descriptorSet->descriptors[binding->binding];

            switch (descriptor->type) {
                case CTS_DESCRIPTOR_TYPE_SAMPLER:
                case CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                case CTS_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                case CTS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                case CTS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
                    CtsDescriptorImageView* imageViewContainer = &descriptor->imageViewContainer;
                    CtsImageView imageView = imageViewContainer->imageView;
                    CtsSampler sampler = imageViewContainer->sampler;

                    if (imageView != NULL) {
                        bindTexture(device, binding->binding, imageView->target, imageView->handle, NULL);
                    }

                    if (sampler != NULL) {
                        glBindSampler(binding->binding, sampler->handle);
                    }
                } break;

                case CTS_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                case CTS_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
                    CtsDescriptorBufferView* bufferViewContainer = &descriptor->bufferViewContainer;
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
                    CtsDescriptorBuffer* bufferContainer = &descriptor->bufferContainer;
                    CtsBuffer buffer = bufferContainer->buffer;

                    if (buffer != NULL) {
                        glBindBuffer(buffer->type, buffer->memory->handle);
                        glBindBufferRange(buffer->type, binding->binding, buffer->memory->handle, bufferContainer->offset, bufferContainer->range);
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

    device->activeIndexBuffer = buffer;
    glBindBuffer(buffer->type, buffer->memory->handle);
}

void ctsCmdBindPipelineImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipeline pipeline
) {
    CtsDevice device = commandBuffer->device;

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

        device->activeGraphicsPipeline = graphicsPipeline;
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

    for (uint32_t i = firstBinding; i < bindingCount; ++i) {
        const CtsBuffer buffer = pBuffers[i];
        struct CtsGlVertexInputAttributeDescription* description = &device->activeVertexInputState->pVertexAttributeDescriptions[firstBinding + i];

        glBindBuffer(buffer->type, buffer->memory->handle);
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
 
    CtsTextureBinding previous;
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
                dstImage->internalFormat,
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
                dstImage->internalFormat,
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
                dstImage->internalFormat,
                dstImage->type,
                (const void*)region->bufferOffset
            );
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    bindTexture(device, 0, previous.target, previous.handle, NULL);
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

    CtsTextureBinding previous;
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

    bindTexture(device, 0, previous.target, previous.handle, NULL);
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
        device->activeInputAssemblyState->polygonMode,
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

    size_t indexSize = (device->activeIndexBuffer->type == GL_UNSIGNED_SHORT)
        ? sizeof(GLushort)
        : sizeof(GLuint);

    glDrawElementsInstancedBaseVertexBaseInstance(
        device->activeInputAssemblyState->polygonMode,
        indexCount,
        device->activeIndexBuffer->type,
        (GLvoid*)(firstIndex * indexSize),
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

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->memory->handle);
    glMultiDrawArraysIndirect(
        device->activeInputAssemblyState->polygonMode,
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
    glMultiDrawElementsIndirect(
        device->activeInputAssemblyState->polygonMode,
        device->activeIndexBuffer->type,
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
    const void* values
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

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)) {
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

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT)) {
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

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
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

    if (hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = firstViewport; i < viewportCount; ++i) {
            const CtsViewport* viewport = &pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
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

static bool hasFlag(CtsFlags flags, CtsFlagBit flag) {
    return ((flags & flag) == flag);
}

static void bindTexture(CtsDevice device, uint32_t pSlot, GLenum pTarget, uint32_t pHandle, CtsTextureBinding* pPrevious) {
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glBindTexture(pTarget, pHandle);

    CtsTextureBinding* data = &device->activeTextures[pSlot];

    if (pPrevious != NULL) {
        pPrevious->target = data->target;
        pPrevious->handle = data->handle;
    }

    data->handle = pHandle;
    data->target = pTarget;
}

static void bindRenderPass(CtsDevice device, CtsRenderPass renderPass, uint32_t subpassNumber) {
    const CtsSubpassDescription* subpassDescription = &renderPass->pSubpasses[subpassNumber];

    (void) subpassDescription->flags;
    (void) subpassDescription->pipelineBindPoint;
    (void) subpassDescription->pResolveAttachments;
    (void) subpassDescription->preserveAttachmentCount;
    (void) subpassDescription->pPreserveAttachments;

    for (uint32_t i = 0; i < subpassDescription->inputAttachmentCount; ++i) {
        const CtsAttachmentReference* inputAttachment = &subpassDescription->pInputAttachments[i];

        //bindTexture(pDevice, inputAttachment->attachment, imageView->target, imageView->handle, NULL);
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
    CtsFlags pState
) {
    device->activeDynamicState = pState;
}

static void bindVertexInputState(
    CtsDevice device,
    CtsGlPipelineVertexInputState* pState
) {
    if (device->activeVertexInputState == pState) {
        return;
    }

    device->activeVertexInputState = pState;
}

static void bindInputAssemblyState(
    CtsDevice device,
    CtsGlPipelineInputAssemblyState* pState
) {
    if (device->activeInputAssemblyState == pState) {
        return;
    }

    device->activeInputAssemblyState = pState;
}

static void bindTessellationState(
    CtsDevice device,
    CtsGlPipelineTessellationState* pState
) {
    if (device->activeTessellationState == pState) {
        return;
    }

    device->activeTessellationState = pState;
}

static void bindViewportState(
    CtsDevice device,
    CtsGlPipelineViewportState* pState
) {
    if (device->activeViewportState == pState) {
        return;
    }

    uint32_t viewportCount = pState->viewportCount;
    uint32_t scissorCount = pState->scissorCount;

    if (!hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT)) {
        for (uint32_t i = 0; i < viewportCount; ++i) {
            CtsViewport* viewport = &pState->pViewports[i];
            glViewportIndexedf(i, viewport->x, viewport->y, viewport->width, viewport->height);
            glDepthRangeIndexed(i, viewport->minDepth, viewport->maxDepth);
        }
    }

    if (!hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_SCISSOR_BIT)) {
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
    if (device->activeRasterizationState == pState) {
        return;
    }

    CtsGlRasterizationStateChanges changes;
    parseRasterizationStateChanges(pState, device->activeRasterizationState, &changes);
    device->activeRasterizationState = pState;

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

    if (!hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_DEPTH_BIAS_BIT) && changes.depthBiasChanged) {
        if (pState->depthBiasEnable) {
            glPolygonOffset(pState->depthBiasConstantFactor, 0);
        } else {
            glPolygonOffset(0, 0);
        }

        // Not sure how to map depthBiasClamp or depthBiasSlopeFactor
    }

    if (!hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT) && changes.lineWidthChanged) {
        glLineWidth(pState->lineWidth);
    }
}

static void bindMultisampleState(
    CtsDevice device,
    CtsGlPipelineMultisampleState* pState)
{
    if (device->activeMultisampleState == pState) {
        return;
    }

    device->activeMultisampleState = pState;
}

static void bindDepthStencilState(
    CtsDevice device,
    CtsGlPipelineDepthStencilState* pState
) {
    if (device->activeDepthStencilState == pState) {
        return;
    }

    CtsGlDepthStencilStateChanges changes;
    parseDepthStencilStateChanges(pState, device->activeDepthStencilState, &changes);
    device->activeDepthStencilState = pState;

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
    CtsDevice device,
    CtsGlPipelineColorBlendState* pState
) {
    if (device->activeColorBlendState == pState) {
        return;
    }

    if (parseColorBlendStateBlendConstantChanged(pState, device->activeColorBlendState) && 
        !hasFlag(device->activeDynamicState, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)
    ) {
        glBlendColor(
            pState->blendConstants[0],
            pState->blendConstants[1],
            pState->blendConstants[2],
            pState->blendConstants[3]
        );
    }

    CtsGlColorBlendStateChanges changes;
    for (uint32_t i = 0; i < pState->attachmentCount; ++i) {
        CtsGlPipelineColorBlendStateAttachment* attachmentState = &pState->pAttachments[i];

        parseColorBlendStateChanges(attachmentState, &device->activeColorBlendState->pAttachments[i], &changes);
        device->activeColorBlendState = pState;

        if (changes.blendEnableChanged) {
            if (attachmentState->blendEnable) {
                glEnablei(GL_BLEND, i);
            } else {
                glDisablei(GL_BLEND, i);
            }
        }

        if (changes.colorWriteMaskChanged) {
            glColorMaski(
                i,
                (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_R_BIT) == CTS_COLOR_COMPONENT_R_BIT,
                (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_G_BIT) == CTS_COLOR_COMPONENT_G_BIT,
                (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_B_BIT) == CTS_COLOR_COMPONENT_B_BIT,
                (attachmentState->colorWriteMask & CTS_COLOR_COMPONENT_A_BIT) == CTS_COLOR_COMPONENT_A_BIT
            );
        }

        if (changes.blendFactorChanged) {
            glBlendFuncSeparateiARB(
                i,
                attachmentState->srcColorBlendFactor,
                attachmentState->dstColorBlendFactor,
                attachmentState->srcAlphaBlendFactor,
                attachmentState->dstAlphaBlendFactor
            );
        }

        if (changes.blendOpChanged) {
            glBlendEquationSeparateiARB(
                i,
                attachmentState->colorBlendOp,
                attachmentState->alphaBlendOp
            );
        }
    }
}

#ifdef __cplusplus
}
#endif