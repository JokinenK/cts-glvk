#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/util/linear_allocator.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CtsCommandBufferState {
    CTS_COMMAND_BUFFER_STATE_INITIAL = 0,
    CTS_COMMAND_BUFFER_STATE_RECORDING = 1,
    CTS_COMMAND_BUFFER_STATE_EXECUTABLE = 2,
    CTS_COMMAND_BUFFER_STATE_PENDING = 3,
    CTS_COMMAND_BUFFER_STATE_INVALID = 4,

    NUM_CTS_COMMAND_BUFFER_STATES,
} CtsCommandBufferState;

struct CtsCommandBuffer {
    struct CtsObjectBase base;
    struct CtsDevice* device;
    struct CtsCommandPool* pool;

    CtsLinearAllocator linearAllocator;
    VkAllocationCallbacks allocator;

    CtsCmdBase* root;
    CtsCmdBase* current;
    VkCommandBufferLevel level;
    VkCommandBufferUsageFlags flags;
    CtsCommandBufferState state;
};

void ctsCmdBeginQueryImpl(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags
);

void ctsCmdBeginRenderPassImpl(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents
);

void ctsCmdBindDescriptorSetsImpl(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout pipelineLayout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets
);

void ctsCmdBindIndexBufferImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkIndexType indexType
);

void ctsCmdBindPipelineImpl(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline
);

void ctsCmdBindVertexBuffersImpl(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets
);

void ctsCmdBlitImageImpl(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
);

void ctsCmdClearAttachmentsImpl(
    VkCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const VkClearAttachment* pAttachments,
    uint32_t rectCount,
    const VkClearRect* pRects
);

void ctsCmdClearColorImageImpl(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearColorValue* pColor,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
);

void ctsCmdClearDepthStencilImageImpl(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges
);

void ctsCmdCopyBufferImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferCopy* pRegions
);

void ctsCmdCopyBufferToImageImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
);

void ctsCmdCopyImageImpl(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageCopy* pRegions
);

void ctsCmdCopyImageToBufferImpl(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions
);

void ctsCmdCopyQueryPoolResultsImpl(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize stride,
    VkQueryResultFlags flags
);

void ctsCmdDispatchImpl(
    VkCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
);

void ctsCmdDispatchBaseImpl(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ
);

void ctsCmdDispatchIndirectImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset
);

void ctsCmdDrawImpl(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
);

void ctsCmdDrawIndexedImpl(
    VkCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance
);

void ctsCmdDrawIndexedIndirectImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdDrawIndirectImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride
);

void ctsCmdEndQueryImpl(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query
);

void ctsCmdEndRenderPassImpl(
    VkCommandBuffer commandBuffer
);

void ctsCmdExecuteCommandsImpl(
    VkCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers
);

void ctsCmdFillBufferImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize size,
    uint32_t data
);

void ctsCmdNextSubpassImpl(
    VkCommandBuffer commandBuffer,
    VkSubpassContents contents
);

void ctsCmdPipelineBarrierImpl(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers
);

void ctsCmdPushConstantsImpl(
    VkCommandBuffer commandBuffer,
    VkPipelineLayout layout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* values
);

void ctsCmdResetEventImpl(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask
);

void ctsCmdResetQueryPoolImpl(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount
);

void ctsCmdResolveImageImpl(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageResolve* pRegions
);

void ctsCmdSetBlendConstantsImpl(
    VkCommandBuffer commandBuffer,
    const float blendConstants[4]
);

void ctsCmdSetDepthBiasImpl(
    VkCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor
);

void ctsCmdSetDepthBoundsImpl(
    VkCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds
);

void ctsCmdSetDeviceMaskImpl(
    VkCommandBuffer commandBuffer,
    uint32_t deviceMask
);

void ctsCmdSetEventImpl(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask
);

void ctsCmdSetLineWidthImpl(
    VkCommandBuffer commandBuffer,
    float lineWidth
);

void ctsCmdSetScissorImpl(
    VkCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors
);

void ctsCmdSetStencilCompareMaskImpl(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t compareMask
);

void ctsCmdSetStencilReferenceImpl(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t reference
);

void ctsCmdSetStencilWriteMaskImpl(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t writeMask
);

void ctsCmdSetViewportImpl(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports
);

void ctsCmdUpdateBufferImpl(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize dataSize,
    const void* pData
);

void ctsCmdWaitEventsImpl(
    VkCommandBuffer commandBuffer,
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
);

void ctsCmdWriteTimestampImpl(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlagBits pipelineStage,
    VkQueryPool queryPool,
    uint32_t query
);

#ifdef __cplusplus
}
#endif