#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include "cts/command_dispatcher.h"
#include "cts/commands.h"
#include "cts/private.h"
#include "cts/command_buffer_private.h"
#include "cts/descriptor_set_private.h"
#include "cts/device_memory_private.h"
#include "cts/fence_private.h"
#include "cts/framebuffer_private.h"
#include "cts/image_private.h"
#include "cts/image_view_private.h"
#include "cts/pipeline_private.h"
#include "cts/sampler_private.h"
#include "cts/swapchain_private.h"
#include "cts/surface_private.h"


#pragma region StaticVariableDefinitions

static CtsCommandMetadata* gCommandMetadata = NULL;
static size_t gMaxCommandSize = 0;
static size_t gMaxCommandAlign = 0;

#pragma endregion

#pragma region StaticMethodDeclarations

static void initCommandMetadata();
static CtsCommandMetadata* createCommandMetadata();

static void CtsAllocateMemoryTrampoline(const CtsCmdBase* pCmd);
static void CtsMapMemoryTrampoline(const CtsCmdBase* pCmd);
static void CtsUnmapMemoryTrampoline(const CtsCmdBase* pCmd);
static void CtsFlushMappedMemoryRangesTrampoline(const CtsCmdBase* pCmd);
static void CtsFreeMemoryTrampoline(const CtsCmdBase* pCmd);

static void CtsQueuePresentTrampoline(const CtsCmdBase* pCmd);

static void CtsAllocateDescriptorSetsTrampoline(const CtsCmdBase* pCmd);
static void CtsUpdateDescriptorSetsTrampoline(const CtsCmdBase* pCmd);
static void CtsFreeDescriptorSetsTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateGraphicsPipelinesTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroyPipelineTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateImageViewTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroyImageViewTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateImageTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroyImageTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateSamplerTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroySamplerTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateSwapchainTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroySwapchainTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateFramebufferTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroyFramebufferTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateFenceTrampoline(const CtsCmdBase* pCmd);
static void CtsResetFencesTrampoline(const CtsCmdBase* pCmd);
static void CtsGetFenceStatusTrampoline(const CtsCmdBase* pCmd);
static void CtsSignalFenceTrampoline(const CtsCmdBase* pCmd);
static void CtsWaitForFencesTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroyFenceTrampoline(const CtsCmdBase* pCmd);

static void CtsCmdBeginQueryTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBeginRenderPassTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBindDescriptorSetsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBindIndexBufferTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBindPipelineTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBindVertexBuffersTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdBlitImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdClearAttachmentsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdClearColorImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdClearDepthStencilImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdCopyBufferTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdCopyBufferToImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdCopyImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdCopyImageToBufferTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdCopyQueryPoolResultsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDispatchTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDispatchIndirectTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDrawTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDrawIndexedTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDrawIndexedIndirectTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdDrawIndirectTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdEndQueryTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdEndRenderPassTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdExecuteCommandsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdFillBufferTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdNextSubpassTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdPipelineBarrierTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdPushConstantsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdResetEventTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdResetQueryPoolTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdResolveImageTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetBlendConstantsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetDepthBiasTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetDepthBoundsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetDeviceMaskTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetEventTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetLineWidthTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetScissorTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetStencilCompareMaskTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetStencilReferenceTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetStencilWriteMaskTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdSetViewportTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdUpdateBufferTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdWaitEventsTrampoline(const CtsCmdBase* pCmd);
static void CtsCmdWriteTimestampTrampoline(const CtsCmdBase* pCmd);

static void CtsCreateWin32SurfaceKHRTrampoline(const CtsCmdBase* pCmd);
static void CtsDestroySurfaceKHRTrampoline(const CtsCmdBase* pCmd);

#pragma endregion
#pragma region PublicMethodBodies

const CtsCommandMetadata* ctsGetCommandMetadata(CtsCommandType commandType) {
    initCommandMetadata();

    CtsCommandMetadata* result = &gCommandMetadata[commandType];
    if (result->handler == NULL) {
        fprintf(stderr, "No handler for command: %d", commandType);
    }

    assert(result->handler != NULL);
    return result;
}

const size_t ctsGetMaxCommandSize() {
    initCommandMetadata();

    assert(gMaxCommandSize > 0);
    return gMaxCommandSize;
}

const size_t ctsGetMaxCommandAlign() {
    initCommandMetadata();

    assert(gMaxCommandAlign > 0);
    return gMaxCommandAlign;
}

#pragma endregion
#pragma region StaticMethodDeclarations

static void initCommandMetadata()
{
    if (gCommandMetadata == NULL) {
        CtsCommandMetadata* commandMetadata = createCommandMetadata();
        size_t maxCommandSize = 0;
        size_t maxCommandAlign = 0;

        for (uint32_t i = 0; i < NUM_CTS_COMMANDS; ++i) {
            if (commandMetadata[i].size > maxCommandSize) {
                maxCommandSize = commandMetadata[i].size;
            } 

            if (commandMetadata[i].align > maxCommandAlign) {
                maxCommandAlign = commandMetadata[i].align;
            }
        }

        gCommandMetadata = commandMetadata;
        gMaxCommandSize = maxCommandSize;
        gMaxCommandAlign = maxCommandAlign;
    }
}

static CtsCommandMetadata* createCommandMetadata()
{
    static CtsCommandMetadata lookup[NUM_CTS_COMMANDS];

    lookup[CTS_COMMAND_CREATE_GRAPHICS_PIPELINES]     = (CtsCommandMetadata) { .handler = CtsCreateGraphicsPipelinesTrampoline,    .size = sizeof(CtsCreateGraphicsPipelines),    .align = alignof(CtsCreateGraphicsPipelines)    };
    lookup[CTS_COMMAND_DESTROY_PIPELINE]              = (CtsCommandMetadata) { .handler = CtsDestroyPipelineTrampoline,            .size = sizeof(CtsDestroyPipeline),            .align = alignof(CtsDestroyPipeline)            };

    lookup[CTS_COMMAND_CREATE_IMAGE_VIEW]             = (CtsCommandMetadata) { .handler = CtsCreateImageViewTrampoline,            .size = sizeof(CtsCreateImageView),            .align = alignof(CtsCreateImageView)            };
    lookup[CTS_COMMAND_DESTROY_IMAGE_VIEW]            = (CtsCommandMetadata) { .handler = CtsDestroyImageViewTrampoline,           .size = sizeof(CtsDestroyImageView),           .align = alignof(CtsDestroyImageView)           };

    lookup[CTS_COMMAND_CREATE_IMAGE]                  = (CtsCommandMetadata) { .handler = CtsCreateImageTrampoline,                .size = sizeof(CtsCreateImage),                .align = alignof(CtsCreateImage)                };
    lookup[CTS_COMMAND_DESTROY_IMAGE]                 = (CtsCommandMetadata) { .handler = CtsDestroyImageTrampoline,               .size = sizeof(CtsDestroyImage),               .align = alignof(CtsDestroyImage)               };

    lookup[CTS_COMMAND_CREATE_SAMPLER]                = (CtsCommandMetadata) { .handler = CtsCreateSamplerTrampoline,              .size = sizeof(CtsCreateSampler),              .align = alignof(CtsCreateSampler)              };
    lookup[CTS_COMMAND_DESTROY_SAMPLER]               = (CtsCommandMetadata) { .handler = CtsDestroySamplerTrampoline,             .size = sizeof(CtsDestroySampler),             .align = alignof(CtsDestroySampler)             };

    lookup[CTS_COMMAND_CREATE_SWAPCHAIN]              = (CtsCommandMetadata) { .handler = CtsCreateSwapchainTrampoline,            .size = sizeof(CtsCreateSwapchain),            .align = alignof(CtsCreateSwapchain)            };
    lookup[CTS_COMMAND_DESTROY_SWAPCHAIN]             = (CtsCommandMetadata) { .handler = CtsDestroySwapchainTrampoline,           .size = sizeof(CtsDestroySwapchain),           .align = alignof(CtsDestroySwapchain)           };

    lookup[CTS_COMMAND_CREATE_FRAMEBUFFER]            = (CtsCommandMetadata) { .handler = CtsCreateFramebufferTrampoline,          .size = sizeof(CtsCreateFramebuffer),          .align = alignof(CtsCreateFramebuffer)          };
    lookup[CTS_COMMAND_DESTROY_FRAMEBUFFER]           = (CtsCommandMetadata) { .handler = CtsDestroyFramebufferTrampoline,         .size = sizeof(CtsDestroyFramebuffer),         .align = alignof(CtsDestroyFramebuffer)         };

    lookup[CTS_COMMAND_CREATE_FENCE]                  = (CtsCommandMetadata) { .handler = CtsCreateFenceTrampoline,                .size = sizeof(CtsCreateFence),                .align = alignof(CtsCreateFence)                };
    lookup[CTS_COMMAND_RESET_FENCES]                  = (CtsCommandMetadata) { .handler = CtsResetFencesTrampoline,                .size = sizeof(CtsResetFences),                .align = alignof(CtsResetFences)                };
    lookup[CTS_COMMAND_GET_FENCE_STATUS]              = (CtsCommandMetadata) { .handler = CtsGetFenceStatusTrampoline,             .size = sizeof(CtsGetFenceStatus),             .align = alignof(CtsGetFenceStatus)             };
    lookup[CTS_COMMAND_SIGNAL_FENCE]                  = (CtsCommandMetadata) { .handler = CtsSignalFenceTrampoline,                .size = sizeof(CtsSignalFence),                .align = alignof(CtsSignalFence)                };
    lookup[CTS_COMMAND_WAIT_FOR_FENCES]               = (CtsCommandMetadata) { .handler = CtsWaitForFencesTrampoline,              .size = sizeof(CtsWaitForFences),              .align = alignof(CtsWaitForFences)              };
    lookup[CTS_COMMAND_DESTROY_FENCE]                 = (CtsCommandMetadata) { .handler = CtsDestroyFenceTrampoline,               .size = sizeof(CtsDestroyFence),               .align = alignof(CtsDestroyFence)               };

    lookup[CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = CtsAllocateDescriptorSetsTrampoline,     .size = sizeof(CtsAllocateDescriptorSets),     .align = alignof(CtsAllocateDescriptorSets)     };
    lookup[CTS_COMMAND_UPDATE_DESCRIPTOR_SETS]        = (CtsCommandMetadata) { .handler = CtsUpdateDescriptorSetsTrampoline,       .size = sizeof(CtsUpdateDescriptorSets),       .align = alignof(CtsUpdateDescriptorSets)       };
    lookup[CTS_COMMAND_FREE_DESCRIPTOR_SETS]          = (CtsCommandMetadata) { .handler = CtsFreeDescriptorSetsTrampoline,         .size = sizeof(CtsFreeDescriptorSets),         .align = alignof(CtsFreeDescriptorSets)         };

    lookup[CTS_COMMAND_ALLOCATE_MEMORY]               = (CtsCommandMetadata) { .handler = CtsAllocateMemoryTrampoline,             .size = sizeof(CtsAllocateMemory),             .align = alignof(CtsAllocateMemory)             };
    lookup[CTS_COMMAND_MAP_MEMORY]                    = (CtsCommandMetadata) { .handler = CtsMapMemoryTrampoline,                  .size = sizeof(CtsMapMemory),                  .align = alignof(CtsMapMemory)                  };
    lookup[CTS_COMMAND_UNMAP_MEMORY]                  = (CtsCommandMetadata) { .handler = CtsUnmapMemoryTrampoline,                .size = sizeof(CtsUnmapMemory),                .align = alignof(CtsUnmapMemory)                };
    lookup[CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES]    = (CtsCommandMetadata) { .handler = CtsFlushMappedMemoryRangesTrampoline,    .size = sizeof(CtsFlushMappedMemoryRanges),    .align = alignof(CtsFlushMappedMemoryRanges)    };
    lookup[CTS_COMMAND_FREE_MEMORY]                   = (CtsCommandMetadata) { .handler = CtsFreeMemoryTrampoline,                 .size = sizeof(CtsFreeMemory),                 .align = alignof(CtsFreeMemory)                 };

    lookup[CTS_COMMAND_QUEUE_PRESENT]                 = (CtsCommandMetadata) { .handler = CtsQueuePresentTrampoline,               .size = sizeof(CtsQueuePresent),               .align = alignof(CtsQueuePresent)               };

    lookup[CTS_COMMAND_CMD_BEGIN_QUERY]               = (CtsCommandMetadata) { .handler = CtsCmdBeginQueryTrampoline,              .size = sizeof(CtsCmdBeginQuery),              .align = alignof(CtsCmdBeginQuery)              };
    lookup[CTS_COMMAND_CMD_BEGIN_RENDER_PASS]         = (CtsCommandMetadata) { .handler = CtsCmdBeginRenderPassTrampoline,         .size = sizeof(CtsCmdBeginRenderPass),         .align = alignof(CtsCmdBeginRenderPass)         };
    lookup[CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = CtsCmdBindDescriptorSetsTrampoline,      .size = sizeof(CtsCmdBindDescriptorSets),      .align = alignof(CtsCmdBindDescriptorSets)      };
    lookup[CTS_COMMAND_CMD_BIND_INDEX_BUFFER]         = (CtsCommandMetadata) { .handler = CtsCmdBindIndexBufferTrampoline,         .size = sizeof(CtsCmdBindIndexBuffer),         .align = alignof(CtsCmdBindIndexBuffer)         };
    lookup[CTS_COMMAND_CMD_BIND_PIPELINE]             = (CtsCommandMetadata) { .handler = CtsCmdBindPipelineTrampoline,            .size = sizeof(CtsCmdBindPipeline),            .align = alignof(CtsCmdBindPipeline)            };
    lookup[CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS]       = (CtsCommandMetadata) { .handler = CtsCmdBindVertexBuffersTrampoline,       .size = sizeof(CtsCmdBindVertexBuffers),       .align = alignof(CtsCmdBindVertexBuffers)       };
    lookup[CTS_COMMAND_CMD_BLIT_IMAGE]                = (CtsCommandMetadata) { .handler = CtsCmdBlitImageTrampoline,               .size = sizeof(CtsCmdBlitImage),               .align = alignof(CtsCmdBlitImage)               };
    lookup[CTS_COMMAND_CMD_CLEAR_ATTACHMENTS]         = (CtsCommandMetadata) { .handler = CtsCmdClearAttachmentsTrampoline,        .size = sizeof(CtsCmdClearAttachments),        .align = alignof(CtsCmdClearAttachments)        };
    lookup[CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE]         = (CtsCommandMetadata) { .handler = CtsCmdClearColorImageTrampoline,         .size = sizeof(CtsCmdClearColorImage),         .align = alignof(CtsCmdClearColorImage)         };
    lookup[CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE] = (CtsCommandMetadata) { .handler = CtsCmdClearDepthStencilImageTrampoline,  .size = sizeof(CtsCmdClearDepthStencilImage),  .align = alignof(CtsCmdClearDepthStencilImage)  };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER]               = (CtsCommandMetadata) { .handler = CtsCmdCopyBufferTrampoline,              .size = sizeof(CtsCmdCopyBuffer),              .align = alignof(CtsCmdCopyBuffer)              };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE]      = (CtsCommandMetadata) { .handler = CtsCmdCopyBufferToImageTrampoline,       .size = sizeof(CtsCmdCopyBufferToImage),       .align = alignof(CtsCmdCopyBufferToImage)       };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE]                = (CtsCommandMetadata) { .handler = CtsCmdCopyImageTrampoline,               .size = sizeof(CtsCmdCopyImage),               .align = alignof(CtsCmdCopyImage)               };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER]      = (CtsCommandMetadata) { .handler = CtsCmdCopyImageToBufferTrampoline,       .size = sizeof(CtsCmdCopyImageToBuffer),       .align = alignof(CtsCmdCopyImageToBuffer)       };
    lookup[CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS]   = (CtsCommandMetadata) { .handler = CtsCmdCopyQueryPoolResultsTrampoline,    .size = sizeof(CtsCmdCopyQueryPoolResults),    .align = alignof(CtsCmdCopyQueryPoolResults)    };
    lookup[CTS_COMMAND_CMD_DISPATCH]                  = (CtsCommandMetadata) { .handler = CtsCmdDispatchTrampoline,                .size = sizeof(CtsCmdDispatch),                .align = alignof(CtsCmdDispatch)                };
    lookup[CTS_COMMAND_CMD_DISPATCH_INDIRECT]         = (CtsCommandMetadata) { .handler = CtsCmdDispatchIndirectTrampoline,        .size = sizeof(CtsCmdDispatchIndirect),        .align = alignof(CtsCmdDispatchIndirect)        };
    lookup[CTS_COMMAND_CMD_DRAW]                      = (CtsCommandMetadata) { .handler = CtsCmdDrawTrampoline,                    .size = sizeof(CtsCmdDraw),                    .align = alignof(CtsCmdDraw)                    };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED]              = (CtsCommandMetadata) { .handler = CtsCmdDrawIndexedTrampoline,             .size = sizeof(CtsCmdDrawIndexed),             .align = alignof(CtsCmdDrawIndexed)             };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT]     = (CtsCommandMetadata) { .handler = CtsCmdDrawIndexedIndirectTrampoline,     .size = sizeof(CtsCmdDrawIndexedIndirect),     .align = alignof(CtsCmdDrawIndexedIndirect)     };
    lookup[CTS_COMMAND_CMD_DRAW_INDIRECT]             = (CtsCommandMetadata) { .handler = CtsCmdDrawIndirectTrampoline,            .size = sizeof(CtsCmdDrawIndirect),            .align = alignof(CtsCmdDrawIndirect)            };
    lookup[CTS_COMMAND_CMD_END_QUERY]                 = (CtsCommandMetadata) { .handler = CtsCmdEndQueryTrampoline,                .size = sizeof(CtsCmdEndQuery),                .align = alignof(CtsCmdEndQuery)                };
    lookup[CTS_COMMAND_CMD_END_RENDER_PASS]           = (CtsCommandMetadata) { .handler = CtsCmdEndRenderPassTrampoline,           .size = sizeof(CtsCmdEndRenderPass),           .align = alignof(CtsCmdEndRenderPass)           };
    lookup[CTS_COMMAND_CMD_EXECUTE_COMMANDS]          = (CtsCommandMetadata) { .handler = CtsCmdExecuteCommandsTrampoline,         .size = sizeof(CtsCmdExecuteCommands),         .align = alignof(CtsCmdExecuteCommands)         };
    lookup[CTS_COMMAND_CMD_FILL_BUFFER]               = (CtsCommandMetadata) { .handler = CtsCmdFillBufferTrampoline,              .size = sizeof(CtsCmdFillBuffer),              .align = alignof(CtsCmdFillBuffer)              };
    lookup[CTS_COMMAND_CMD_NEXT_SUBPASS]              = (CtsCommandMetadata) { .handler = CtsCmdNextSubpassTrampoline,             .size = sizeof(CtsCmdNextSubpass),             .align = alignof(CtsCmdNextSubpass)             };
    lookup[CTS_COMMAND_CMD_PIPELINE_BARRIER]          = (CtsCommandMetadata) { .handler = CtsCmdPipelineBarrierTrampoline,         .size = sizeof(CtsCmdPipelineBarrier),         .align = alignof(CtsCmdPipelineBarrier)         };
    lookup[CTS_COMMAND_CMD_PUSH_CONSTANTS]            = (CtsCommandMetadata) { .handler = CtsCmdPushConstantsTrampoline,           .size = sizeof(CtsCmdPushConstants),           .align = alignof(CtsCmdPushConstants)           };
    lookup[CTS_COMMAND_CMD_RESET_EVENT]               = (CtsCommandMetadata) { .handler = CtsCmdResetEventTrampoline,              .size = sizeof(CtsCmdResetEvent),              .align = alignof(CtsCmdResetEvent)              };
    lookup[CTS_COMMAND_CMD_RESET_QUERY_POOL]          = (CtsCommandMetadata) { .handler = CtsCmdResetQueryPoolTrampoline,          .size = sizeof(CtsCmdResetQueryPool),          .align = alignof(CtsCmdResetQueryPool)          };
    lookup[CTS_COMMAND_CMD_RESOLVE_IMAGE]             = (CtsCommandMetadata) { .handler = CtsCmdResolveImageTrampoline,            .size = sizeof(CtsCmdResolveImage),            .align = alignof(CtsCmdResolveImage)            };
    lookup[CTS_COMMAND_CMD_SET_BLEND_CONSTANTS]       = (CtsCommandMetadata) { .handler = CtsCmdSetBlendConstantsTrampoline,       .size = sizeof(CtsCmdSetBlendConstants),       .align = alignof(CtsCmdSetBlendConstants)       };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BIAS]            = (CtsCommandMetadata) { .handler = CtsCmdSetDepthBiasTrampoline,            .size = sizeof(CtsCmdSetDepthBias),            .align = alignof(CtsCmdSetDepthBias)            };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BOUNDS]          = (CtsCommandMetadata) { .handler = CtsCmdSetDepthBoundsTrampoline,          .size = sizeof(CtsCmdSetDepthBounds),          .align = alignof(CtsCmdSetDepthBounds)          };
    lookup[CTS_COMMAND_CMD_SET_DEVICE_MASK]           = (CtsCommandMetadata) { .handler = CtsCmdSetDeviceMaskTrampoline,           .size = sizeof(CtsCmdSetDeviceMask),           .align = alignof(CtsCmdSetDeviceMask)           };
    lookup[CTS_COMMAND_CMD_SET_EVENT]                 = (CtsCommandMetadata) { .handler = CtsCmdSetEventTrampoline,                .size = sizeof(CtsCmdSetEvent),                .align = alignof(CtsCmdSetEvent)                };
    lookup[CTS_COMMAND_CMD_SET_LINE_WIDTH]            = (CtsCommandMetadata) { .handler = CtsCmdSetLineWidthTrampoline,            .size = sizeof(CtsCmdSetLineWidth),            .align = alignof(CtsCmdSetLineWidth)            };
    lookup[CTS_COMMAND_CMD_SET_SCISSOR]               = (CtsCommandMetadata) { .handler = CtsCmdSetScissorTrampoline,              .size = sizeof(CtsCmdSetScissor),              .align = alignof(CtsCmdSetScissor)              };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK]  = (CtsCommandMetadata) { .handler = CtsCmdSetStencilCompareMaskTrampoline,   .size = sizeof(CtsCmdSetStencilCompareMask),   .align = alignof(CtsCmdSetStencilCompareMask)   };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_REFERENCE]     = (CtsCommandMetadata) { .handler = CtsCmdSetStencilReferenceTrampoline,     .size = sizeof(CtsCmdSetStencilReference),     .align = alignof(CtsCmdSetStencilReference)     };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK]    = (CtsCommandMetadata) { .handler = CtsCmdSetStencilWriteMaskTrampoline,     .size = sizeof(CtsCmdSetStencilWriteMask),     .align = alignof(CtsCmdSetStencilWriteMask)     };
    lookup[CTS_COMMAND_CMD_SET_VIEWPORT]              = (CtsCommandMetadata) { .handler = CtsCmdSetViewportTrampoline,             .size = sizeof(CtsCmdSetViewport),             .align = alignof(CtsCmdSetViewport)             };
    lookup[CTS_COMMAND_CMD_UPDATE_BUFFER]             = (CtsCommandMetadata) { .handler = CtsCmdUpdateBufferTrampoline,            .size = sizeof(CtsCmdUpdateBuffer),            .align = alignof(CtsCmdUpdateBuffer)            };
    lookup[CTS_COMMAND_CMD_WAIT_EVENTS]               = (CtsCommandMetadata) { .handler = CtsCmdWaitEventsTrampoline,              .size = sizeof(CtsCmdWaitEvents),              .align = alignof(CtsCmdWaitEvents)              };
    lookup[CTS_COMMAND_CMD_WRITE_TIMESTAMP]           = (CtsCommandMetadata) { .handler = CtsCmdWriteTimestampTrampoline,          .size = sizeof(CtsCmdWriteTimestamp),          .align = alignof(CtsCmdWriteTimestamp)          };

    lookup[CTS_COMMAND_CREATE_WIN32_SURFACE_KHR]      = (CtsCommandMetadata) { .handler = CtsCreateWin32SurfaceKHRTrampoline,      .size = sizeof(CtsCreateWin32SurfaceKHR),      .align = alignof(CtsCreateWin32SurfaceKHR)      };
    lookup[CTS_COMMAND_DESTROY_SURFACE_KHR]           = (CtsCommandMetadata) { .handler = CtsDestroySurfaceKHRTrampoline,          .size = sizeof(CtsDestroySurfaceKHR),          .align = alignof(CtsDestroySurfaceKHR)          };

    return lookup;
}

static void CtsAllocateMemoryTrampoline(const CtsCmdBase* pCmd) {
    const CtsAllocateMemory* cmd = (const CtsAllocateMemory*) pCmd;
    *cmd->pResult = ctsAllocateMemoryImpl(cmd->device, cmd->pAllocateInfo, cmd->pAllocator, cmd->pMemory);
}

static void CtsMapMemoryTrampoline(const CtsCmdBase* pCmd) {
    const CtsMapMemory* cmd = (const CtsMapMemory*) pCmd;
    *cmd->pResult = ctsMapMemoryImpl(cmd->device, cmd->memory, cmd->offset, cmd->size, cmd->flags, cmd->ppData);
}

static void CtsUnmapMemoryTrampoline(const CtsCmdBase* pCmd) {
    const CtsUnmapMemory* cmd = (const CtsUnmapMemory*) pCmd;
    ctsUnmapMemoryImpl(cmd->device, cmd->memory);
}

static void CtsFlushMappedMemoryRangesTrampoline(const CtsCmdBase* pCmd) {
    const CtsFlushMappedMemoryRanges* cmd = (const CtsFlushMappedMemoryRanges*) pCmd;
    *cmd->pResult = ctsFlushMappedMemoryRangesImpl(cmd->device, cmd->memoryRangeCount, cmd->pMemoryRanges);
}

static void CtsFreeMemoryTrampoline(const CtsCmdBase* pCmd) {
    const CtsFreeMemory* cmd = (const CtsFreeMemory*) pCmd;
    ctsFreeMemoryImpl(cmd->device, cmd->memory, cmd->pAllocator);
}

static void CtsQueuePresentTrampoline(const CtsCmdBase* pCmd) {
    const CtsQueuePresent* cmd = (const CtsQueuePresent*) pCmd;
    *cmd->pResult = ctsQueuePresentKHRImpl(cmd->queue, cmd->pPresentInfo);
}

static void CtsAllocateDescriptorSetsTrampoline(const CtsCmdBase* pCmd) {
    const CtsAllocateDescriptorSets* cmd = (const CtsAllocateDescriptorSets*) pCmd;
    *cmd->pResult = ctsAllocateDescriptorSetsImpl(cmd->device, cmd->pAllocateInfo, cmd->pDescriptorSets);
}

static void CtsUpdateDescriptorSetsTrampoline(const CtsCmdBase* pCmd) {
    const CtsUpdateDescriptorSets* cmd = (const CtsUpdateDescriptorSets*) pCmd;
    ctsUpdateDescriptorSetsImpl(cmd->device, cmd->descriptorWriteCount, cmd->pDescriptorWrites, cmd->descriptorCopyCount, cmd->pDescriptorCopies);
}

static void CtsFreeDescriptorSetsTrampoline(const CtsCmdBase* pCmd) {
    const CtsFreeDescriptorSets* cmd = (const CtsFreeDescriptorSets*) pCmd;
    ctsFreeDescriptorSetsImpl(cmd->device,  cmd->descriptorPool, cmd->descriptorSetCount, cmd->pDescriptorSets);
}

static void CtsCreateGraphicsPipelinesTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateGraphicsPipelines* cmd = (const CtsCreateGraphicsPipelines*) pCmd;
    *cmd->pResult = ctsCreateGraphicsPipelinesImpl(cmd->device, cmd->pipelineCache, cmd->createInfoCount, cmd->pCreateInfos, cmd->pAllocator, cmd->pPipelines);
}

static void CtsDestroyPipelineTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroyPipeline* cmd = (const CtsDestroyPipeline*) pCmd;
    ctsDestroyPipelineImpl(cmd->device, cmd->pipeline, cmd->pAllocator);
}

static void CtsCreateImageViewTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateImageView* cmd = (const CtsCreateImageView*) pCmd;
    *cmd->pResult = ctsCreateImageViewImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pImageView);
}

static void CtsDestroyImageViewTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroyImageView* cmd = (const CtsDestroyImageView*) pCmd;
    ctsDestroyImageViewImpl(cmd->device, cmd->imageView, cmd->pAllocator);
}

static void CtsCreateImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateImage* cmd = (const CtsCreateImage*) pCmd;
    *cmd->pResult = ctsCreateImageImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pImage);
}

static void CtsDestroyImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroyImage* cmd = (const CtsDestroyImage*) pCmd;
    ctsDestroyImageImpl(cmd->device, cmd->image, cmd->pAllocator);
}

static void CtsCreateSamplerTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateSampler* cmd = (const CtsCreateSampler*) pCmd;
    *cmd->pResult = ctsCreateSamplerImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pSampler);
}

static void CtsDestroySamplerTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroySampler* cmd = (const CtsDestroySampler*) pCmd;
    ctsDestroySamplerImpl(cmd->device, cmd->sampler, cmd->pAllocator);
}

static void CtsCreateSwapchainTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateSwapchain* cmd = (const CtsCreateSwapchain*) pCmd;
    *cmd->pResult = ctsCreateSwapchainKHRImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pSwapchain);
}

static void CtsDestroySwapchainTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroySwapchain* cmd = (const CtsDestroySwapchain*) pCmd;
    ctsDestroySwapchainKHRImpl(cmd->device, cmd->swapchain, cmd->pAllocator);
}

static void CtsCreateFramebufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateFramebuffer* cmd = (const CtsCreateFramebuffer*) pCmd;
    *cmd->pResult = ctsCreateFramebufferImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pFramebuffer);
}

static void CtsDestroyFramebufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroyFramebuffer* cmd = (const CtsDestroyFramebuffer*) pCmd;
    ctsDestroyFramebufferImpl(cmd->device, cmd->framebuffer, cmd->pAllocator);
}

static void CtsCreateFenceTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateFence* cmd = (const CtsCreateFence*) pCmd;
    *cmd->pResult = ctsCreateFenceImpl(cmd->device, cmd->pCreateInfo, cmd->pAllocator, cmd->pFence);
}

static void CtsResetFencesTrampoline(const CtsCmdBase* pCmd) {
    const CtsResetFences* cmd = (const CtsResetFences*) pCmd;
    *cmd->pResult = ctsResetFencesImpl(cmd->device, cmd->fenceCount, cmd->pFences);
}

static void CtsGetFenceStatusTrampoline(const CtsCmdBase* pCmd) {
    const CtsGetFenceStatus* cmd = (const CtsGetFenceStatus*) pCmd;
    *cmd->pResult = ctsGetFenceStatusImpl(cmd->device, cmd->fence);
}

static void CtsSignalFenceTrampoline(const CtsCmdBase* pCmd) {
    const CtsSignalFence* cmd = (const CtsSignalFence*) pCmd;
    ctsSignalFenceFenceImpl(cmd->device, cmd->fence);
}

static void CtsWaitForFencesTrampoline(const CtsCmdBase* pCmd) {
    const CtsWaitForFences* cmd = (const CtsWaitForFences*) pCmd;
    *cmd->pResult = ctsWaitForFencesImpl(cmd->device, cmd->fenceCount, cmd->pFences, cmd->waitAll, cmd->timeout);
}

static void CtsDestroyFenceTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroyFence* cmd = (const CtsDestroyFence*) pCmd;
    ctsDestroyFenceImpl(cmd->device, cmd->fence, cmd->pAllocator);
}

static void CtsCmdBeginQueryTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBeginQuery* cmd = (const CtsCmdBeginQuery*) pCmd;
    ctsCmdBeginQueryImpl(cmd->commandBuffer, cmd->queryPool, cmd->query, cmd->flags);
}

static void CtsCmdBeginRenderPassTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBeginRenderPass* cmd = (const CtsCmdBeginRenderPass*) pCmd;
    ctsCmdBeginRenderPassImpl(cmd->commandBuffer, cmd->pRenderPassBegin, cmd->contents);
}

static void CtsCmdBindDescriptorSetsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBindDescriptorSets* cmd = (const CtsCmdBindDescriptorSets*) pCmd;
    ctsCmdBindDescriptorSetsImpl(cmd->commandBuffer, cmd->pipelineBindPoint, cmd->pipelineLayout, cmd->firstSet, cmd->descriptorSetCount, cmd->pDescriptorSets, cmd->dynamicOffsetCount, cmd->pDynamicOffsets);
}

static void CtsCmdBindIndexBufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBindIndexBuffer* cmd = (const CtsCmdBindIndexBuffer*) pCmd;
    ctsCmdBindIndexBufferImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->indexType);
}

static void CtsCmdBindPipelineTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBindPipeline* cmd = (const CtsCmdBindPipeline*) pCmd;
    ctsCmdBindPipelineImpl(cmd->commandBuffer, cmd->pipelineBindPoint, cmd->pipeline);
}

static void CtsCmdBindVertexBuffersTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBindVertexBuffers* cmd = (const CtsCmdBindVertexBuffers*) pCmd;
    ctsCmdBindVertexBuffersImpl(cmd->commandBuffer, cmd->firstBinding, cmd->bindingCount, cmd->pBuffers, cmd->pOffsets);
}

static void CtsCmdBlitImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdBlitImage* cmd = (const CtsCmdBlitImage*) pCmd;
    ctsCmdBlitImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->pRegions, cmd->filter);
}

static void CtsCmdClearAttachmentsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdClearAttachments* cmd = (const CtsCmdClearAttachments*) pCmd;
    ctsCmdClearAttachmentsImpl(cmd->commandBuffer, cmd->attachmentCount, cmd->pAttachments, cmd->rectCount, cmd->pRects);
}

static void CtsCmdClearColorImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdClearColorImage* cmd = (const CtsCmdClearColorImage*) pCmd;
    ctsCmdClearColorImageImpl(cmd->commandBuffer, cmd->image, cmd->imageLayout, cmd->pColor, cmd->rangeCount, cmd->pRanges);
}

static void CtsCmdClearDepthStencilImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdClearDepthStencilImage* cmd = (const CtsCmdClearDepthStencilImage*) pCmd;
    ctsCmdClearDepthStencilImageImpl(cmd->commandBuffer, cmd->image, cmd->imageLayout, cmd->pDepthStencil, cmd->rangeCount, cmd->pRanges);
}

static void CtsCmdCopyBufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdCopyBuffer* cmd = (const CtsCmdCopyBuffer*) pCmd;
    ctsCmdCopyBufferImpl(cmd->commandBuffer, cmd->srcBuffer, cmd->dstBuffer, cmd->regionCount, cmd->pRegions);
}

static void CtsCmdCopyBufferToImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdCopyBufferToImage* cmd = (const CtsCmdCopyBufferToImage*) pCmd;
    ctsCmdCopyBufferToImageImpl(cmd->commandBuffer, cmd->srcBuffer, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->pRegions);
}

static void CtsCmdCopyImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdCopyImage* cmd = (const CtsCmdCopyImage*) pCmd;
    ctsCmdCopyImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->pRegions);
}

static void CtsCmdCopyImageToBufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdCopyImageToBuffer* cmd = (const CtsCmdCopyImageToBuffer*) pCmd;
    ctsCmdCopyImageToBufferImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstBuffer, cmd->regionCount, cmd->pRegions);
}

static void CtsCmdCopyQueryPoolResultsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdCopyQueryPoolResults* cmd = (const CtsCmdCopyQueryPoolResults*) pCmd;
    ctsCmdCopyQueryPoolResultsImpl(cmd->commandBuffer, cmd->queryPool, cmd->firstQuery, cmd->queryCount, cmd->dstBuffer, cmd->dstOffset, cmd->stride, cmd->flags);
}

static void CtsCmdDispatchTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDispatch* cmd = (const CtsCmdDispatch*) pCmd;
    ctsCmdDispatchImpl(cmd->commandBuffer, cmd->groupCountX, cmd->groupCountY, cmd->groupCountZ);
}

static void CtsCmdDispatchIndirectTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDispatchIndirect* cmd = (const CtsCmdDispatchIndirect*) pCmd;
    ctsCmdDispatchIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset);
}

static void CtsCmdDrawTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDraw* cmd = (const CtsCmdDraw*) pCmd;
    ctsCmdDrawImpl(cmd->commandBuffer, cmd->vertexCount, cmd->instanceCount, cmd->firstVertex, cmd->firstInstance);
}

static void CtsCmdDrawIndexedTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndexed* cmd = (const CtsCmdDrawIndexed*) pCmd;
    ctsCmdDrawIndexedImpl(cmd->commandBuffer, cmd->indexCount, cmd->instanceCount, cmd->firstIndex, cmd->vertexOffset, cmd->firstInstance);
}

static void CtsCmdDrawIndexedIndirectTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndexedIndirect* cmd = (const CtsCmdDrawIndexedIndirect*) pCmd;
    ctsCmdDrawIndexedIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->drawCount, cmd->stride);
}

static void CtsCmdDrawIndirectTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndirect* cmd = (const CtsCmdDrawIndirect*) pCmd;
    ctsCmdDrawIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->drawCount, cmd->stride);
}

static void CtsCmdEndQueryTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdEndQuery* cmd = (const CtsCmdEndQuery*) pCmd;
    ctsCmdEndQueryImpl(cmd->commandBuffer, cmd->queryPool, cmd->query);
}

static void CtsCmdEndRenderPassTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdEndRenderPass* cmd = (const CtsCmdEndRenderPass*) pCmd;
    ctsCmdEndRenderPassImpl(cmd->commandBuffer);
}

static void CtsCmdExecuteCommandsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdExecuteCommands* cmd = (const CtsCmdExecuteCommands*) pCmd;
    ctsCmdExecuteCommandsImpl(cmd->commandBuffer, cmd->commandBufferCount, cmd->pCommandBuffers);
}

static void CtsCmdFillBufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdFillBuffer* cmd = (const CtsCmdFillBuffer*) pCmd;
    ctsCmdFillBufferImpl(cmd->commandBuffer, cmd->dstBuffer, cmd->dstOffset, cmd->size, cmd->data);
}

static void CtsCmdNextSubpassTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdNextSubpass* cmd = (const CtsCmdNextSubpass*) pCmd;
    ctsCmdNextSubpassImpl(cmd->commandBuffer, cmd->contents);
}

static void CtsCmdPipelineBarrierTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdPipelineBarrier* cmd = (const CtsCmdPipelineBarrier*) pCmd;
    ctsCmdPipelineBarrierImpl(cmd->commandBuffer, cmd->srcStageMask, cmd->dstStageMask, cmd->dependencyFlags, cmd->memoryBarrierCount, cmd->pMemoryBarriers, cmd->bufferMemoryBarrierCount, cmd->pBufferMemoryBarriers, cmd->imageMemoryBarrierCount, cmd->pImageMemoryBarriers);
}

static void CtsCmdPushConstantsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdPushConstants* cmd = (const CtsCmdPushConstants*) pCmd;
    ctsCmdPushConstantsImpl(cmd->commandBuffer, cmd->layout, cmd->stageFlags, cmd->offset, cmd->size, cmd->pValues);
}

static void CtsCmdResetEventTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdResetEvent* cmd = (const CtsCmdResetEvent*) pCmd;
    ctsCmdResetEventImpl(cmd->commandBuffer, cmd->event, cmd->stageMask);
}

static void CtsCmdResetQueryPoolTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdResetQueryPool* cmd = (const CtsCmdResetQueryPool*) pCmd;
    ctsCmdResetQueryPoolImpl(cmd->commandBuffer, cmd->queryPool, cmd->firstQuery, cmd->queryCount);
}

static void CtsCmdResolveImageTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdResolveImage* cmd = (const CtsCmdResolveImage*) pCmd;
    ctsCmdResolveImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->pRegions);
}

static void CtsCmdSetBlendConstantsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetBlendConstants* cmd = (const CtsCmdSetBlendConstants*) pCmd;
    ctsCmdSetBlendConstantsImpl(cmd->commandBuffer, cmd->blendConstants);
}

static void CtsCmdSetDepthBiasTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetDepthBias* cmd = (const CtsCmdSetDepthBias*) pCmd;
    ctsCmdSetDepthBiasImpl(cmd->commandBuffer, cmd->depthBiasConstantFactor, cmd->depthBiasClamp, cmd->depthBiasSlopeFactor);
}

static void CtsCmdSetDepthBoundsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetDepthBounds* cmd = (const CtsCmdSetDepthBounds*) pCmd;
    ctsCmdSetDepthBoundsImpl(cmd->commandBuffer, cmd->minDepthBounds, cmd->maxDepthBounds);
}

static void CtsCmdSetDeviceMaskTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetDeviceMask* cmd = (const CtsCmdSetDeviceMask*) pCmd;
    ctsCmdSetDeviceMaskImpl(cmd->commandBuffer, cmd->deviceMask);
}

static void CtsCmdSetEventTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetEvent* cmd = (const CtsCmdSetEvent*) pCmd;
    ctsCmdSetEventImpl(cmd->commandBuffer, cmd->event, cmd->stageMask);
}

static void CtsCmdSetLineWidthTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetLineWidth* cmd = (const CtsCmdSetLineWidth*) pCmd;
    ctsCmdSetLineWidthImpl(cmd->commandBuffer, cmd->lineWidth);
}

static void CtsCmdSetScissorTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetScissor* cmd = (const CtsCmdSetScissor*) pCmd;
    ctsCmdSetScissorImpl(cmd->commandBuffer, cmd->firstScissor, cmd->scissorCount, cmd->pScissors);
}

static void CtsCmdSetStencilCompareMaskTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilCompareMask* cmd = (const CtsCmdSetStencilCompareMask*) pCmd;
    ctsCmdSetStencilCompareMaskImpl(cmd->commandBuffer, cmd->faceMask, cmd->compareMask);
}

static void CtsCmdSetStencilReferenceTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilReference* cmd = (const CtsCmdSetStencilReference*) pCmd;
    ctsCmdSetStencilReferenceImpl(cmd->commandBuffer, cmd->faceMask, cmd->reference);
}

static void CtsCmdSetStencilWriteMaskTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilWriteMask* cmd = (const CtsCmdSetStencilWriteMask*) pCmd;
    ctsCmdSetStencilWriteMaskImpl(cmd->commandBuffer, cmd->faceMask, cmd->writeMask);
}

static void CtsCmdSetViewportTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdSetViewport* cmd = (const CtsCmdSetViewport*) pCmd;
    ctsCmdSetViewportImpl(cmd->commandBuffer, cmd->firstViewport, cmd->viewportCount, cmd->pViewports);
}

static void CtsCmdUpdateBufferTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdUpdateBuffer* cmd = (const CtsCmdUpdateBuffer*) pCmd;
    ctsCmdUpdateBufferImpl(cmd->commandBuffer, cmd->dstBuffer, cmd->dstOffset, cmd->dataSize, cmd->pData);
}

static void CtsCmdWaitEventsTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdWaitEvents* cmd = (const CtsCmdWaitEvents*) pCmd;
    ctsCmdWaitEventsImpl(cmd->commandBuffer, cmd->eventCount, cmd->events, cmd->srcStageMask, cmd->dstStageMask, cmd->memoryBarrierCount, cmd->pMemoryBarriers, cmd->bufferMemoryBarrierCount, cmd->pBufferMemoryBarriers, cmd->imageMemoryBarrierCount, cmd->pImageMemoryBarriers);
}

static void CtsCmdWriteTimestampTrampoline(const CtsCmdBase* pCmd) {
    const CtsCmdWriteTimestamp* cmd = (const CtsCmdWriteTimestamp*) pCmd;
    ctsCmdWriteTimestampImpl(cmd->commandBuffer, cmd->pipelineStage, cmd->queryPool, cmd->query);
}

static void CtsCreateWin32SurfaceKHRTrampoline(const CtsCmdBase* pCmd) {
    const CtsCreateWin32SurfaceKHR* cmd = (const CtsCreateWin32SurfaceKHR*) pCmd;
    *cmd->pResult = ctsCreateWin32SurfaceKHRImpl(cmd->instance, cmd->pCreateInfo, cmd->pAllocator, cmd->pSurface);
}

static void CtsDestroySurfaceKHRTrampoline(const CtsCmdBase* pCmd) {
    const CtsDestroySurfaceKHR* cmd = (const CtsDestroySurfaceKHR*) pCmd;
    ctsDestroySurfaceKHRImpl(cmd->instance,  cmd->surface, cmd->pAllocator);
}

#pragma endregion
