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

static bool hasFlag(CtsFlags flags, CtsFlags flag);
static void bindFramebuffer(CtsDevice pDevice, CtsFramebuffer pFramebuffer, uint32_t pSubpassNumber);
static void bindDynamicState(CtsDevice pDevice, CtsFlags pState);
static void bindVertexInputState(CtsDevice pDevice, CtsGlPipelineVertexInputState* pState);
static void bindInputAssemblyState(CtsDevice pDevice, CtsGlPipelineInputAssemblyState* pState);
static void bindTessellationState(CtsDevice pDevice, CtsGlPipelineTessellationState* pState);
static void bindViewportState(CtsDevice pDevice, CtsGlPipelineViewportState* pState);
static void bindRasterizationState(CtsDevice pDevice, CtsGlPipelineRasterizationState* pState);
static void bindMultisampleState(CtsDevice pDevice, CtsGlPipelineMultisampleState* pState);
static void bindDepthStencilState(CtsDevice pDevice, CtsGlPipelineDepthStencilState* pState);
static void bindColorBlendState(CtsDevice pDevice, CtsGlPipelineColorBlendState* pState);

CtsResult ctsAllocateCommandBuffers(
    CtsDevice pDevice,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
) {
    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    const CtsAllocationCallbacks* allocator = &pAllocateInfo->commandPool->bumpAllocator;

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

        commandBuffer->device = pDevice;
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
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    const CtsAllocationCallbacks* allocator = &pCommandPool->bumpAllocator;

    for (uint32_t i = 0; i < pCommandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = pCommandBuffers[i];
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
        ctsFree(allocator, commandBuffer);
    }
}

CtsResult ctsBeginCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
) {
    (void) pBeginInfo->inheritanceInfo;

    if (pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_INITIAL) {
        pCommandBuffer->flags = pBeginInfo->flags;
        pCommandBuffer->root = NULL;
        pCommandBuffer->current = NULL;
        pCommandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    return CTS_SUCCESS;
}

CtsResult ctsResetCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandBufferResetFlags pFlags
) {
    if (pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING || pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
        pCommandBuffer->root = NULL;
        pCommandBuffer->current = NULL;
        pCommandBuffer->state = CTS_COMMAND_BUFFER_STATE_RECORDING;
    }

    return CTS_SUCCESS;
}

CtsResult ctsEndCommandBuffer(
    CtsCommandBuffer pCommandBuffer
) {
    if (pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING) {
        pCommandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
    }

    return CTS_SUCCESS;
}

CtsResult ctsQueueSubmit(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
) {
    CtsResult result;
    CtsQueueSubmit cmd;
    cmd.base.type = CTS_COMMAND_QUEUE_SUBMIT;
    cmd.base.next = NULL;

    cmd.queue = pQueue,
    cmd.submitCount = pSubmitCount;
    cmd.submits = pSubmits;
    cmd.fence = pFence;
    cmd.result = &result;

    ctsQueueDispatch(pQueue, &cmd.base, pQueue->device->dispatchSemaphore);
    ctsWaitSemaphores(1, &pQueue->device->dispatchSemaphore);

    return result;
}

void ctsCmdBeginQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
) {
    CtsCmdBeginQuery* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BEGIN_QUERY
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
    cmd->flags = pFlags;
}

void ctsCmdEndQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
    CtsCmdEndQuery* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_END_QUERY
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
}

void ctsCmdBeginRenderPass(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents pContents
) {
    CtsCmdBeginRenderPass* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BEGIN_RENDER_PASS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->renderPassBegin = pRenderPassBegin;
    cmd->contents = pContents;
}

void ctsCmdEndRenderPass(
    CtsCommandBuffer pCommandBuffer
) {
    CtsCmdEndRenderPass* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_END_RENDER_PASS
    );

    cmd->commandBuffer = pCommandBuffer;
}

void ctsCmdBindDescriptorSets(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipelineLayout pPipelineLayout,
    uint32_t pFirstSet,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t pDynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    CtsCmdBindDescriptorSets* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineBindPoint = pPipelineBindPoint;
    cmd->firstSet = pFirstSet;
    cmd->descriptorSetCount = pDescriptorSetCount;
    cmd->descriptorSets = pDescriptorSets;
    cmd->dynamicOffsetCount = pDynamicOffsetCount;
    cmd->dynamicOffsets = pDynamicOffsets;
}

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    CtsIndexType pIndexType
) {
    CtsCmdBindIndexBuffer* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_INDEX_BUFFER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->indexType = pIndexType;
}

void ctsCmdBindPipeline(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipeline pPipeline
) {
    CtsCmdBindPipeline* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_PIPELINE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineBindPoint = pPipelineBindPoint;
    cmd->pipeline = pPipeline;
}

void ctsCmdBindVertexBuffers(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstBinding,
    uint32_t pBindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
) {
    CtsCmdBindVertexBuffers* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstBinding = pFirstBinding;
    cmd->bindingCount = pBindingCount;
    cmd->buffers = pBuffers;
    cmd->offsets = pOffsets;
}

void ctsCmdBlitImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageBlit* pRegions,
    CtsFilter pFilter
) {
    CtsCmdBlitImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BLIT_IMAGE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
    cmd->filter = pFilter;
}

void ctsCmdClearAttachments(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pAttachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t pRectCount,
    const CtsClearRect* pRects
) {
    CtsCmdClearAttachments* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_ATTACHMENTS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->attachmentCount = pAttachmentCount;
    cmd->attachments = pAttachments;
    cmd->rectCount = pRectCount;
    cmd->rects = pRects;
}

void ctsCmdClearColorImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearColorValue* pColor,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearColorImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->image = pImage;
    cmd->imageLayout = pImageLayout;
    cmd->color = pColor;
    cmd->rangeCount = pRangeCount;
    cmd->ranges = pRanges;
}

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearDepthStencilImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->image = pImage;
    cmd->imageLayout = pImageLayout;
    cmd->depthStencil = pDepthStencil;
    cmd->rangeCount = pRangeCount;
    cmd->ranges = pRanges;
}

void ctsCmdCopyBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferCopy* pRegions
) {
    CtsCmdCopyBuffer* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcBuffer = pSrcBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyBufferToImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE
    );
    cmd->commandBuffer = pCommandBuffer;
    cmd->srcBuffer = pSrcBuffer;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageCopy* pRegions
) {
    CtsCmdCopyImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyImageToBuffer* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstBuffer = pDstBuffer;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyQueryPoolResults(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pStride,
    CtsQueryResultFlags pFlags
) {
    CtsCmdCopyQueryPoolResults* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->firstQuery = pFirstQuery;
    cmd->queryCount = pQueryCount;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->stride = pStride;
    cmd->flags = pFlags;
}

void ctsCmdDispatch(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pGroupCountX,
    uint32_t pGroupCountY,
    uint32_t pGroupCountZ
) {
    CtsCmdDispatch* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DISPATCH
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->groupCountX = pGroupCountX;
    cmd->groupCountY = pGroupCountY;
    cmd->groupCountZ = pGroupCountZ;
}

void ctsCmdDispatchIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
) {
    CtsCmdDispatchIndirect* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DISPATCH_INDIRECT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
}

void ctsCmdDraw(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pVertexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstVertex,
    uint32_t pFirstInstance
) {
    CtsCmdDraw* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->vertexCount = pVertexCount;
    cmd->instanceCount = pInstanceCount;
    cmd->firstVertex = pFirstVertex;
    cmd->firstInstance = pFirstInstance;
}

void ctsCmdDrawIndexed(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pIndexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstIndex,
    int32_t pVertexOffset,
    uint32_t pFirstInstance
) {
    CtsCmdDrawIndexed* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->indexCount = pIndexCount;
    cmd->instanceCount = pInstanceCount;
    cmd->firstIndex = pFirstIndex;
    cmd->vertexOffset = pVertexOffset;
    cmd->firstInstance = pFirstInstance;
}

void ctsCmdDrawIndexedIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsCmdDrawIndexedIndirect* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->drawCount = pDrawCount;
    cmd->stride = pStride;
}

void ctsCmdDrawIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsCmdDrawIndirect* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDIRECT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->drawCount = pDrawCount;
    cmd->stride = pStride;
}

void ctsCmdExecuteCommands(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    CtsCmdExecuteCommands* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_EXECUTE_COMMANDS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->commandBufferCount = pCommandBufferCount;
    cmd->commandBuffers = pCommandBuffers;
}

void ctsCmdFillBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pSize,
    uint32_t pData
) {
    CtsCmdFillBuffer* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_FILL_BUFFER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->size = pSize;
    cmd->data = pData;
}

void ctsCmdNextSubpass(
    CtsCommandBuffer pCommandBuffer,
    CtsSubpassContents pContents
) {
    CtsCmdNextSubpass* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_NEXT_SUBPASS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->contents = pContents;
}

void ctsCmdPipelineBarrier(
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
    CtsCmdPipelineBarrier* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_PIPELINE_BARRIER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcStageMask = pSrcStageMask;
    cmd->dstStageMask = pDstStageMask;
    cmd->dependencyFlags = pDependencyFlags;
    cmd->memoryBarrierCount = pMemoryBarrierCount;
    cmd->memoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = pBufferMemoryBarrierCount;
    cmd->bufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = pImageMemoryBarrierCount;
    cmd->imageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdPushConstants(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineLayout pLayout,
    CtsShaderStageFlags pStageFlags,
    uint32_t pOffset,
    uint32_t pSize,
    const void* pValues
) {
    CtsCmdPushConstants* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_PUSH_CONSTANTS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->layout = pLayout;
    cmd->stageFlags = pStageFlags;
    cmd->offset = pOffset;
    cmd->size = pSize;
    cmd->values = pValues;
}

void ctsCmdResetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
    CtsCmdResetEvent* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESET_EVENT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->event = pEvent;
    cmd->stageMask = pStageMask;
}

void ctsCmdResetQueryPool(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount
) {
    CtsCmdResetQueryPool* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESET_QUERY_POOL
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->firstQuery = pFirstQuery;
    cmd->queryCount = pQueryCount;
}

void ctsCmdResolveImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageResolve* pRegions
) {
    CtsCmdResolveImage* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESOLVE_IMAGE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdSetBlendConstants(
    CtsCommandBuffer pCommandBuffer,
    const float pBlendConstants[4]
) {
    CtsCmdSetBlendConstants* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_BLEND_CONSTANTS
    );

    cmd->commandBuffer = pCommandBuffer;
    memcpy(cmd->blendConstants, pBlendConstants, sizeof(float[4]));
}

void ctsCmdSetDepthBias(
    CtsCommandBuffer pCommandBuffer,
    float pDepthBiasConstantFactor,
    float pDepthBiasClamp,
    float pDepthBiasSlopeFactor
) {
    CtsCmdSetDepthBias* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BIAS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->depthBiasConstantFactor = pDepthBiasConstantFactor;
    cmd->depthBiasClamp = pDepthBiasClamp;
    cmd->depthBiasSlopeFactor = pDepthBiasSlopeFactor;
}

void ctsCmdSetDepthBounds(
    CtsCommandBuffer pCommandBuffer,
    float pMinDepthBounds,
    float pMaxDepthBounds
) {
    CtsCmdSetDepthBounds* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BOUNDS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->minDepthBounds = pMinDepthBounds;
    cmd->maxDepthBounds = pMaxDepthBounds;
}

void ctsCmdSetDeviceMask(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pDeviceMask
) {
    CtsCmdSetDeviceMask* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEVICE_MASK
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->deviceMask = pDeviceMask;
}

void ctsCmdSetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
    CtsCmdSetEvent* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_EVENT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->event = pEvent;
    cmd->stageMask = pStageMask;
}

void ctsCmdSetLineWidth(
    CtsCommandBuffer pCommandBuffer,
    float pLineWidth
) {
    CtsCmdSetLineWidth* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_LINE_WIDTH
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->lineWidth = pLineWidth;
}

void ctsCmdSetScissor(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstScissor,
    uint32_t pScissorCount,
    const CtsRect2D* pScissors
) {
    CtsCmdSetScissor* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_SCISSOR
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstScissor = pFirstScissor;
    cmd->scissorCount = pScissorCount;
    cmd->scissors = pScissors;
}

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pCompareMask
) {
    CtsCmdSetStencilCompareMask* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->compareMask = pCompareMask;
}

void ctsCmdSetStencilReference(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pReference
) {
    CtsCmdSetStencilReference* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_REFERENCE
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->reference = pReference;
}

void ctsCmdSetStencilWriteMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pWriteMask
) {
    CtsCmdSetStencilWriteMask* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->writeMask = pWriteMask;
}

void ctsCmdSetViewport(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstViewport,
    uint32_t pViewportCount,
    const CtsViewport* pViewports
) {
    CtsCmdSetViewport* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_VIEWPORT
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstViewport = pFirstViewport;
    cmd->viewportCount = pViewportCount;
    cmd->viewports = pViewports;
}

void ctsCmdUpdateBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pDataSize,
    const void* pData
) {
    CtsCmdUpdateBuffer* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_UPDATE_BUFFER
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->dataSize = pDataSize;
    cmd->data = pData;
}

void ctsCmdWaitEvents(
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
    CtsCmdWaitEvents* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_WAIT_EVENTS
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->eventCount = pEventCount;
    cmd->events = pEvents;
    cmd->srcStageMask = pSrcStageMask;
    cmd->dstStageMask = pDstStageMask;
    cmd->memoryBarrierCount = pMemoryBarrierCount;
    cmd->memoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = pBufferMemoryBarrierCount;
    cmd->bufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = pImageMemoryBarrierCount;
    cmd->imageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdWriteTimestamp(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlagBits pPipelineStage,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
    CtsCmdWriteTimestamp* cmd = ctsCommandBufferAllocateCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_WRITE_TIMESTAMP
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineStage = pPipelineStage;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
}

void* ctsCommandBufferAllocateCommand(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandType pCommandType
) {
    assert(pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_RECORDING);

    const CtsCommandMetadata* metadata = ctsGetCommandMetadata(pCommandType);
    CtsCmdBase* cmd = ctsAllocation(
        &pCommandBuffer->pool->bumpAllocator,
        metadata->size,
        metadata->align,
        CTS_SYSTEM_ALLOCATION_SCOPE_COMMAND
    );

    cmd->type = pCommandType;
    cmd->next = NULL;

    if (pCommandBuffer->root == NULL) {
        pCommandBuffer->root = cmd;
    } else {
        pCommandBuffer->current->next = cmd;
    }
    
    pCommandBuffer->current = cmd;
    return cmd;
}

CtsResult ctsQueueSubmitImpl(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
) {
    if (pFence != NULL) {
        ctsWaitForFencesImpl(pQueue->device, 1, &pFence, true, UINT64_MAX);
    }

    CtsQueueItem queueItem;
    for (uint32_t i = 0; i < pSubmitCount; ++i) {
        const CtsSubmitInfo* submit = &pSubmits[i];

        // TODO: Handle waitDstStageMask
        ctsWaitSemaphores(submit->waitSemaphoreCount, submit->waitSemaphores);

        for (uint32_t j = 0; j < submit->commandBufferCount; ++j) {
            CtsCommandBuffer commandBuffer = submit->commandBuffers[j];

            if (commandBuffer->state == CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
                commandBuffer->state = CTS_COMMAND_BUFFER_STATE_PENDING;

                CtsQueueFinish* queueFinishCmd = ctsCommandBufferAllocateCommand(
                    commandBuffer,
                    CTS_COMMAND_QUEUE_FINISH
                );

                queueFinishCmd->semaphoreCount = submit->signalSemaphoreCount;
                queueFinishCmd->semaphores = submit->signalSemaphores;
                queueFinishCmd->commandBuffer = commandBuffer;

                queueItem.cmd = commandBuffer->root;
                queueItem.semaphore = NULL;
                ctsQueuePush(pQueue, &queueItem);
            }
        }
    }
   
    return CTS_SUCCESS;
}

void ctsQueueFinishImpl(
    uint32_t pSemaphoreCount,
    const CtsSemaphore* pSemaphores,
    CtsCommandBuffer pCommandBuffer
) {
    if (pCommandBuffer->state == CTS_COMMAND_BUFFER_STATE_PENDING) {
        if (pCommandBuffer->flags & CTS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
            pCommandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;
        } else {
            pCommandBuffer->state = CTS_COMMAND_BUFFER_STATE_EXECUTABLE;
        }

        ctsSignalSemaphores(pSemaphoreCount, pSemaphores);
    }
}

void ctsCmdBeginQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
) {
    // TODO: Implement this
    //glBeginQuery()
}

void ctsCmdEndQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
    //glEndQuery()
}

void ctsCmdBeginRenderPassImpl(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents pContents
) {
    (void) pRenderPassBegin->renderArea;
    (void) pContents;

    CtsDevice device = pCommandBuffer->device;
    CtsFramebuffer framebuffer = pRenderPassBegin->framebuffer;

    bindFramebuffer(device, framebuffer, 0);

    uint32_t lastAttachment = (pRenderPassBegin->clearValueCount < framebuffer->attachmentCount)
        ? pRenderPassBegin->clearValueCount
        : framebuffer->attachmentCount;

    for (uint32_t i = 0; i < lastAttachment; ++i) {
        const CtsClearValue* clearValue = &pRenderPassBegin->clearValues[i];
        const CtsAttachmentDescription* description = &pRenderPassBegin->renderPass->attachments[i];

        if (description->format == CTS_FORMAT_DEPTH ||
            description->format == CTS_FORMAT_DEPTHSTENCIL ||
            description->format == CTS_FORMAT_DEPTH24STENCIL8
        ) {
            glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue->depthStencil.depth,  clearValue->depthStencil.stencil);
        } else {
            glClearBufferfv(GL_COLOR, i, clearValue->color.float32);
        }
    }
}

void ctsCmdEndRenderPassImpl(
    CtsCommandBuffer pCommandBuffer
) {
    CtsDevice device = pCommandBuffer->device;
    device->activeFramebuffer = NULL;
    device->activeSubpass = 0;
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
                    glBindSampler(binding->binding, imageInfo->sampler->handle);
                } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                    CtsDescriptorBufferInfo* bufferInfo = &binding->bufferInfo[k];
                    CtsBuffer buffer = bufferInfo->buffer;

                    size_t offset = buffer->offset + bufferInfo->offset;
                    size_t size = (bufferInfo->range > buffer->size)
                        ? buffer->size
                        : bufferInfo->range;

                    glBindBuffer(buffer->type, buffer->memory->handle);
                    glBindBufferRange(buffer->type, binding->binding + k, buffer->memory->handle, offset, size);
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
    glBindBuffer(pBuffer->type, pBuffer->memory->handle);
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
    glBindBuffer(GL_COPY_READ_BUFFER, pSrcBuffer->memory->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->memory->handle);

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
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pSrcBuffer->memory->handle);
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
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pDstBuffer->memory->handle);
    glBindTexture(target, pSrcImage->handle);
    
    for (uint32_t i = 0; i < pRegionCount; ++i) {
        const CtsBufferImageCopy* region = &pRegions[i];

        (void) region->bufferImageHeight;
        (void) region->imageSubresource.aspectMask;

        glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->memory->handle);

        void* bufferData = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_WRITE_ONLY);
        size_t bufferOffset = (pDstBuffer->offset + region->bufferOffset);
        void* bufferPos = (char*)bufferData + pDstBuffer->offset;
        size_t bufferSize = (pDstBuffer->size - bufferOffset);

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
            (GLsizei)bufferSize,
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
    glDispatchCompute(pGroupCountX, pGroupCountY, pGroupCountZ);
}

void ctsCmdDispatchIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
) {
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, pBuffer->memory->handle);
    glDispatchComputeIndirect((GLintptr) pBuffer->offset);
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

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pBuffer->memory->handle);
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

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pBuffer->memory->handle);
    glMultiDrawElementsIndirect(
        device->activeInputAssemblyState->polygonMode,
        device->activeIndexBuffer->type,
        (GLvoid*)pOffset,
        pDrawCount,
        pStride
    );
}

void ctsCmdExecuteCommandsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    const CtsCommandMetadata* commandMetadata;
    for (uint32_t i = 0; i < pCommandBufferCount; ++i) {
        const CtsCommandBuffer commandBuffer = pCommandBuffers[i];

        if (commandBuffer->state != CTS_COMMAND_BUFFER_STATE_EXECUTABLE) {
            continue;
        }

        const CtsCmdBase* next = commandBuffer->root;
        while (next != NULL) {
            commandMetadata = ctsGetCommandMetadata(next->type);
            commandMetadata->handler(next);
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
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->memory->handle);
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
    CtsDevice device = pCommandBuffer->device;
    bindFramebuffer(device, device->activeFramebuffer, device->activeSubpass + 1);
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
    glBindBuffer(GL_COPY_WRITE_BUFFER, pDstBuffer->memory->handle);
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

static bool hasFlag(CtsFlags flags, CtsFlagBit flag) {
    return ((flags & flag) == flag);
}

static void bindFramebuffer(CtsDevice pDevice, CtsFramebuffer pFramebuffer, uint32_t pSubpassNumber) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pFramebuffer->handle);
    const CtsSubpassDescription* subpassDescription = &pFramebuffer->renderPass->subpasses[pSubpassNumber];

    (void) subpassDescription->flags;
    (void) subpassDescription->pipelineBindPoint;
    (void) subpassDescription->resolveAttachments;
    (void) subpassDescription->preserveAttachmentCount;
    (void) subpassDescription->preserveAttachments;

    for (uint32_t i = 0; i < pFramebuffer->attachmentCount; ++i) {
        pFramebuffer->drawBuffers[i] = GL_NONE;
    }

    for (uint32_t i = 0; i < subpassDescription->inputAttachmentCount; ++i) {
        const CtsAttachmentReference* inputAttachment = &subpassDescription->inputAttachments[i];
        const CtsImageView imageView = pFramebuffer->attachments[inputAttachment->attachment];

        glActiveTexture(GL_TEXTURE0 + inputAttachment->attachment);
        glBindTexture(imageView->target, imageView->handle);
    }

    for (uint32_t i = 0; i < subpassDescription->colorAttachmentCount; ++i) {
        const CtsAttachmentReference* colorAttachment = &subpassDescription->colorAttachments[i];
        const CtsImageView imageView = pFramebuffer->attachments[colorAttachment->attachment];
        GLenum buffer = GL_COLOR_ATTACHMENT0 + colorAttachment->attachment;

        pFramebuffer->drawBuffers[colorAttachment->attachment] = buffer;
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER,
            buffer,
            imageView->target,
            imageView->handle,
            0
        );
    }

    if (subpassDescription->depthStencilAttachment != NULL) {
        const CtsAttachmentReference* depthStencilAttachment = subpassDescription->depthStencilAttachment;
        const CtsImageView imageView = pFramebuffer->attachments[depthStencilAttachment->attachment];
        GLenum buffer = GL_DEPTH_STENCIL_ATTACHMENT;

        pFramebuffer->drawBuffers[depthStencilAttachment->attachment] = buffer;
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER,
            buffer,
            imageView->target,
            imageView->handle,
            0
        );
    }

    glDrawBuffers(pFramebuffer->attachmentCount, pFramebuffer->drawBuffers);
    pDevice->activeSubpass = pSubpassNumber;
    pDevice->activeFramebuffer = pFramebuffer;
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

    if (parseColorBlendStateBlendConstantChanged(pState, pDevice->activeColorBlendState) && 
        !hasFlag(pDevice->activeDynamicState, CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT)
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
        CtsGlPipelineColorBlendStateAttachment* attachmentState = &pState->attachments[i];

        parseColorBlendStateChanges(attachmentState, &pDevice->activeColorBlendState->attachments[i], &changes);
        pDevice->activeColorBlendState = pState;

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