#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/command_types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateCommandBuffers(
    CtsDevice device,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
);

void ctsFreeCommandBuffers(
    CtsDevice device,
    CtsCommandPool commandPool,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

CtsResult ctsBeginCommandBuffer(
    CtsCommandBuffer commandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
);

CtsResult ctsResetCommandBuffer(
    CtsCommandBuffer commandBuffer,
    CtsCommandBufferResetFlags flags
);

CtsResult ctsEndCommandBuffer(
    CtsCommandBuffer commandBuffer
);

CtsResult ctsQueueSubmit(
    CtsQueue queue,
    uint32_t submitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence fence
);

void ctsCmdBeginQuery(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query,
    CtsQueryControlFlags flags
);

void ctsCmdBeginRenderPass(
    CtsCommandBuffer commandBuffer,
    const CtsRenderPassBeginInfo* renderPassBegin,
    CtsSubpassContents contents
);

void ctsCmdBindDescriptorSets(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
);

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    CtsIndexType indexType
);

void ctsCmdBindPipeline(
    CtsCommandBuffer commandBuffer,
    CtsPipelineBindPoint pipelineBindPoint,
    CtsPipeline pipeline
);

void ctsCmdBindVertexBuffers(
    CtsCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
);

void ctsCmdBlitImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageBlit* pRegions,
    CtsFilter filter
);

void ctsCmdClearAttachments(
    CtsCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t rectCount,
    const CtsClearRect* pRects
);

void ctsCmdClearColorImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearColorValue* pColor,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer commandBuffer,
    CtsImage image,
    CtsImageLayout imageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdCopyBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferCopy* pRegions
);

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer commandBuffer,
    CtsBuffer srcBuffer,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageCopy* pRegions
);

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsBuffer dstBuffer,
    uint32_t regionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyQueryPoolResults(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize stride,
    CtsQueryResultFlags flags
);

void ctsCmdDispatch(
    CtsCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
);

void ctsCmdDispatchIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset
);

void ctsCmdDraw(
    CtsCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
);

void ctsCmdDrawIndexed(
    CtsCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
);

void ctsCmdDrawIndexedIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdDrawIndirect(
    CtsCommandBuffer commandBuffer,
    CtsBuffer buffer,
    CtsDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdEndQuery(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t query
);

void ctsCmdEndRenderPass(
    CtsCommandBuffer commandBuffer
);

void ctsCmdExecuteCommands(
    CtsCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

void ctsCmdFillBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize size,
    uint32_t data
);

void ctsCmdNextSubpass(
    CtsCommandBuffer commandBuffer,
    CtsSubpassContents contents
);

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
);

void ctsCmdPushConstants(
    CtsCommandBuffer commandBuffer,
    CtsPipelineLayout layout,
    CtsShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* values
);

void ctsCmdResetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
);

void ctsCmdResetQueryPool(
    CtsCommandBuffer commandBuffer,
    CtsQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
);

void ctsCmdResolveImage(
    CtsCommandBuffer commandBuffer,
    CtsImage srcImage,
    CtsImageLayout srcImageLayout,
    CtsImage dstImage,
    CtsImageLayout dstImageLayout,
    uint32_t regionCount,
    const CtsImageResolve* pRegions
);

void ctsCmdSetBlendConstants(
    CtsCommandBuffer commandBuffer,
    const float blendConstants[4]
);

void ctsCmdSetDepthBias(
    CtsCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
);

void ctsCmdSetDepthBounds(
    CtsCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds
);

void ctsCmdSetDeviceMask(
    CtsCommandBuffer commandBuffer,
    uint32_t deviceMask
);

void ctsCmdSetEvent(
    CtsCommandBuffer commandBuffer,
    CtsEvent event,
    CtsPipelineStageFlags stageMask
);

void ctsCmdSetLineWidth(
    CtsCommandBuffer commandBuffer,
    float lineWidth
);

void ctsCmdSetScissor(
    CtsCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const CtsRect2D* pScissors
);

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t compareMask
);

void ctsCmdSetStencilReference(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t reference
);

void ctsCmdSetStencilWriteMask(
    CtsCommandBuffer commandBuffer,
    CtsStencilFaceFlags faceMask,
    uint32_t writeMask
);

void ctsCmdSetViewport(
    CtsCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const CtsViewport* pViewports
);

void ctsCmdUpdateBuffer(
    CtsCommandBuffer commandBuffer,
    CtsBuffer dstBuffer,
    CtsDeviceSize dstOffset,
    CtsDeviceSize dataSize,
    const void* pData
);

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
);

void ctsCmdWriteTimestamp(
    CtsCommandBuffer commandBuffer,
    CtsPipelineStageFlagBits pipelineStage,
    CtsQueryPool queryPool,
    uint32_t query
);

void* ctsCommandBufferAllocateCommand(
    CtsCommandBuffer commandBuffer,
    CtsCommandType commandType
);

#ifdef __cplusplus
}
#endif