#pragma once

#include <cts/typedefs/bool.h>
#include <cts/typedefs/command_pool.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/enums.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandBuffer {
    CtsDevice device;
    CtsCommandPool pool;
    CtsCmdBase* root;
    CtsCmdBase* current;
    CtsCommandBufferLevel level;
    CtsCommandBufferUsageFlags flags;
    CtsCommandBufferState state;
};

CtsResult ctsQueueSubmitImpl(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
);

void ctsQueueFinishImpl(
    uint32_t pSemaphoreCount,
    const CtsSemaphore* pSemaphores,
    CtsCommandBuffer pCommandBuffer
);

void ctsCmdBeginQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
);

void ctsCmdBeginRenderPassImpl(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* renderPassBegin,
    CtsSubpassContents contents
);

void ctsCmdBindDescriptorSetsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipelineLayout pPipelineLayout,
    uint32_t pFirstSet,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
);

void ctsCmdBindIndexBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    CtsIndexType pIndexType
);

void ctsCmdBindPipelineImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipeline pPipeline
);

void ctsCmdBindVertexBuffersImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstBinding,
    uint32_t pBindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
);

void ctsCmdBlitImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageBlit* pRegions,
    CtsFilter pFilter
);

void ctsCmdClearAttachmentsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pAttachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t pRectCount,
    const CtsClearRect* pRects
);

void ctsCmdClearColorImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearColorValue* pColor,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdClearDepthStencilImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
);

void ctsCmdCopyBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferCopy* pRegions
);

void ctsCmdCopyBufferToImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageCopy* pRegions
);

void ctsCmdCopyImageToBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
);

void ctsCmdCopyQueryPoolResultsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pStride,
    CtsQueryResultFlags pFlags
);

void ctsCmdDispatchImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pGroupCountX,
    uint32_t pGroupCountY,
    uint32_t pGroupCountZ
);

void ctsCmdDispatchIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
);

void ctsCmdDrawImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pVertexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstVertex,
    uint32_t pFirstInstance
);

void ctsCmdDrawIndexedImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pIndexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstIndex,
    int32_t pVertexOffset,
    uint32_t pFirstInstance
);

void ctsCmdDrawIndexedIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
);

void ctsCmdDrawIndirectImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
);

void ctsCmdEndQueryImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
);

void ctsCmdEndRenderPassImpl(
    CtsCommandBuffer pCommandBuffer
);

void ctsCmdExecuteCommandsImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

void ctsCmdFillBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pSize,
    uint32_t pData
);

void ctsCmdNextSubpassImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsSubpassContents pContents
);

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
);

void ctsCmdPushConstantsImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineLayout pLayout,
    CtsShaderStageFlags pStageFlags,
    uint32_t pOffset,
    uint32_t pSize,
    const void* pValues
);

void ctsCmdResetEventImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
);

void ctsCmdResetQueryPoolImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount
);

void ctsCmdResolveImageImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageResolve* pRegions
);

void ctsCmdSetBlendConstantsImpl(
    CtsCommandBuffer pCommandBuffer,
    const float pBlendConstants[4]
);

void ctsCmdSetDepthBiasImpl(
    CtsCommandBuffer pCommandBuffer,
    float pDepthBiasConstantFactor,
    float pDepthBiasClamp,
    float pDepthBiasSlopeFactor
);

void ctsCmdSetDepthBoundsImpl(
    CtsCommandBuffer pCommandBuffer,
    float pMinDepthBounds,
    float pMaxDepthBounds
);

void ctsCmdSetDeviceMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pDeviceMask
);

void ctsCmdSetEventImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
);

void ctsCmdSetLineWidthImpl(
    CtsCommandBuffer pCommandBuffer,
    float pLineWidth
);

void ctsCmdSetScissorImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstScissor,
    uint32_t pScissorCount,
    const CtsRect2D* pScissors
);

void ctsCmdSetStencilCompareMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pCompareMask
);

void ctsCmdSetStencilReferenceImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pReference
);

void ctsCmdSetStencilWriteMaskImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pWriteMask
);

void ctsCmdSetViewportImpl(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstViewport,
    uint32_t pViewportCount,
    const CtsViewport* pViewports
);

void ctsCmdUpdateBufferImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pDataSize,
    const void* pData
);

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
);

void ctsCmdWriteTimestampImpl(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlagBits pPipelineStage,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
);

#ifdef __cplusplus
}
#endif