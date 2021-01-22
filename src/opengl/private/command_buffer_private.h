#pragma once

#include <cts/typedefs/bool.h>
#include <cts/typedefs/command_buffer.h>
#include <cts/typedefs/command_pool.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/enums.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/linear_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandBufferImpl {
    CtsDevice device;
    CtsCommandPool pool;

    CtsLinearAllocator linearAllocator;
    CtsAllocationCallbacks allocator;

    CtsCmdBase* root;
    CtsCmdBase* current;
    CtsCommandBufferLevel level;
    CtsCommandBufferUsageFlags flags;
    CtsCommandBufferState state;
};

void ctsCmdBeginQueryImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query,
    CtsQueryControlFlags flags
);

void ctsCmdBeginRenderPassImpl(
    CtsCommandBuffer commandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents contents
);

void ctsCmdBindDescriptorSetsImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
);

void ctsCmdBindIndexBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    CtsIndexType indexType
);

void ctsCmdBindPipelineImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipeline pipeline
);

void ctsCmdBindVertexBuffersImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
);

void ctsCmdBlitImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageBlit* pRegions,
    CtsFilter filter
);

void ctsCmdClearAttachmentsImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t rectCount,
    const CtsClearRect* pRects
);

void ctsCmdClearColorImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearColorValue* pColor,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdClearDepthStencilImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdCopyBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferCopy* pRegions
);

void ctsCmdCopyBufferToImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageCopy* pRegions
);

void ctsCmdCopyImageToBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyQueryPoolResultsImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize stride,
    CtsQueryResultFlags flags
);

void ctsCmdDispatchImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
);

void ctsCmdDispatchIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset
);

void ctsCmdDrawImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
);

void ctsCmdDrawIndexedImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
);

void ctsCmdDrawIndexedIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdDrawIndirectImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdEndQueryImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query
);

void ctsCmdEndRenderPassImpl(
    CtsCommandBuffer commandBuffer
);

void ctsCmdExecuteCommandsImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

void ctsCmdFillBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize size,
    uint32_t data
);

void ctsCmdNextSubpassImpl(
    CtsCommandBuffer commandBuffer,
    CtsSubpassContents contents
);

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
);

void ctsCmdPushConstantsImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineLayout layout,
    CtsShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* values
);

void ctsCmdResetEventImpl(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
);

void ctsCmdResetQueryPoolImpl(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
);

void ctsCmdResolveImageImpl(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageResolve* pRegions
);

void ctsCmdSetBlendConstantsImpl(
    CtsCommandBuffer commandBuffer,
    const float blendConstants[4]
);

void ctsCmdSetDepthBiasImpl(
    CtsCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
);

void ctsCmdSetDepthBoundsImpl(
    CtsCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds
);

void ctsCmdSetDeviceMaskImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t deviceMask
);

void ctsCmdSetEventImpl(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
);

void ctsCmdSetLineWidthImpl(
    CtsCommandBuffer commandBuffer,
    float lineWidth
);

void ctsCmdSetScissorImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const CtsRect2D* pScissors
);

void ctsCmdSetStencilCompareMaskImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t compareMask
);

void ctsCmdSetStencilReferenceImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t reference
);

void ctsCmdSetStencilWriteMaskImpl(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t writeMask
);

void ctsCmdSetViewportImpl(
    CtsCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const CtsViewport* pViewports
);

void ctsCmdUpdateBufferImpl(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize dataSize,
    const void* pData
);

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
);

void ctsCmdWriteTimestampImpl(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlagBits pipelineStage,
    CtsQueryPool queryPool,
    uint32_t query
);

#ifdef __cplusplus
}
#endif