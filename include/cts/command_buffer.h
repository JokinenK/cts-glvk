#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/command_types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateCommandBuffers(
    CtsDevice pDevice,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
);

void ctsFreeCommandBuffers(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

CtsResult ctsBeginCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
);

CtsResult ctsResetCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandBufferResetFlags pFlags
);

CtsResult ctsEndCommandBuffer(
    CtsCommandBuffer pCommandBuffer
);

CtsResult ctsQueueSubmit(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
);

void ctsCmdBeginQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
);

void ctsCmdBeginRenderPass(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* renderPassBegin,
    CtsSubpassContents contents
);

void ctsCmdBindDescriptorSets(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipelineLayout pPipelineLayout,
    uint32_t pFirstSet,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
);

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    CtsIndexType pIndexType
);

void ctsCmdBindPipeline(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipeline pPipeline
);

void ctsCmdBindVertexBuffers(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstBinding,
    uint32_t pBindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
);

void ctsCmdBlitImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageBlit* pRegions,
    CtsFilter pFilter
);

void ctsCmdClearAttachments(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pAttachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t pRectCount,
    const CtsClearRect* pRects
);

void ctsCmdClearColorImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearColorValue* pColor,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdCopyBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferCopy* pRegions
);

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageCopy* pRegions
);

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyQueryPoolResults(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pStride,
    CtsQueryResultFlags pFlags
);

void ctsCmdDispatch(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pGroupCountX,
    uint32_t pGroupCountY,
    uint32_t pGroupCountZ
);

void ctsCmdDispatchIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
);

void ctsCmdDraw(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pVertexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstVertex,
    uint32_t pFirstInstance
);

void ctsCmdDrawIndexed(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pIndexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstIndex,
    int32_t pVertexOffset,
    uint32_t pFirstInstance
);

void ctsCmdDrawIndexedIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
);

void ctsCmdDrawIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
);

void ctsCmdEndQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
);

void ctsCmdEndRenderPass(
    CtsCommandBuffer pCommandBuffer
);

void ctsCmdExecuteCommands(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

void ctsCmdFillBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pSize,
    uint32_t pData
);

void ctsCmdNextSubpass(
    CtsCommandBuffer pCommandBuffer,
    CtsSubpassContents pContents
);

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
);

void ctsCmdPushConstants(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineLayout pLayout,
    CtsShaderStageFlags pStageFlags,
    uint32_t pOffset,
    uint32_t pSize,
    const void* pValues
);

void ctsCmdResetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
);

void ctsCmdResetQueryPool(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount
);

void ctsCmdResolveImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageResolve* pRegions
);

void ctsCmdSetBlendConstants(
    CtsCommandBuffer pCommandBuffer,
    const float pBlendConstants[4]
);

void ctsCmdSetDepthBias(
    CtsCommandBuffer pCommandBuffer,
    float pDepthBiasConstantFactor,
    float pDepthBiasClamp,
    float pDepthBiasSlopeFactor
);

void ctsCmdSetDepthBounds(
    CtsCommandBuffer pCommandBuffer,
    float pMinDepthBounds,
    float pMaxDepthBounds
);

void ctsCmdSetDeviceMask(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pDeviceMask
);

void ctsCmdSetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
);

void ctsCmdSetLineWidth(
    CtsCommandBuffer pCommandBuffer,
    float pLineWidth
);

void ctsCmdSetScissor(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstScissor,
    uint32_t pScissorCount,
    const CtsRect2D* pScissors
);

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pCompareMask
);

void ctsCmdSetStencilReference(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pReference
);

void ctsCmdSetStencilWriteMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pWriteMask
);

void ctsCmdSetViewport(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstViewport,
    uint32_t pViewportCount,
    const CtsViewport* pViewports
);

void ctsCmdUpdateBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pDataSize,
    const void* pData
);

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
);

void ctsCmdWriteTimestamp(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlagBits pPipelineStage,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
);

void* ctsCommandBufferAllocateCommand(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandType pCommandType
);

#ifdef __cplusplus
}
#endif