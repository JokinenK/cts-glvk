#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include "cts/command_dispatcher.h"
#include "cts/commands.h"

static CtsCommandMetadata* gCommandMetadata = NULL;
static size_t gMaxCommandSize = 0;
static size_t gMaxCommandAlign = 0;

static void initCommandMetadata();
static CtsCommandMetadata* createCommandMetadata();
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

    lookup[CTS_COMMAND_CREATE_GRAPHICS_PIPELINES]     = (CtsCommandMetadata) { .handler = ctsCreateGraphicsPipelinesTrampoline,    .size = sizeof(CtsCreateGraphicsPipelines),    .align = alignof(CtsCreateGraphicsPipelines)    };
    lookup[CTS_COMMAND_DESTROY_PIPELINE]              = (CtsCommandMetadata) { .handler = ctsDestroyPipelineTrampoline,            .size = sizeof(CtsDestroyPipeline),            .align = alignof(CtsDestroyPipeline)            };

    lookup[CTS_COMMAND_CREATE_IMAGE_VIEW]             = (CtsCommandMetadata) { .handler = ctsCreateImageViewTrampoline,            .size = sizeof(CtsCreateImageView),            .align = alignof(CtsCreateImageView)            };
    lookup[CTS_COMMAND_DESTROY_IMAGE_VIEW]            = (CtsCommandMetadata) { .handler = ctsDestroyImageViewTrampoline,           .size = sizeof(CtsDestroyImageView),           .align = alignof(CtsDestroyImageView)           };

    lookup[CTS_COMMAND_CREATE_IMAGE]                  = (CtsCommandMetadata) { .handler = ctsCreateImageTrampoline,                .size = sizeof(CtsCreateImage),                .align = alignof(CtsCreateImage)                };
    lookup[CTS_COMMAND_DESTROY_IMAGE]                 = (CtsCommandMetadata) { .handler = ctsDestroyImageTrampoline,               .size = sizeof(CtsDestroyImage),               .align = alignof(CtsDestroyImage)               };

    lookup[CTS_COMMAND_CREATE_SAMPLER]                = (CtsCommandMetadata) { .handler = ctsCreateSamplerTrampoline,              .size = sizeof(CtsCreateSampler),              .align = alignof(CtsCreateSampler)              };
    lookup[CTS_COMMAND_DESTROY_SAMPLER]               = (CtsCommandMetadata) { .handler = ctsDestroySamplerTrampoline,             .size = sizeof(CtsDestroySampler),             .align = alignof(CtsDestroySampler)             };

    lookup[CTS_COMMAND_CREATE_SWAPCHAIN]              = (CtsCommandMetadata) { .handler = ctsCreateSwapchainKHRTrampoline,         .size = sizeof(CtsCreateSwapchainKHR),         .align = alignof(CtsCreateSwapchainKHR)         };
    lookup[CTS_COMMAND_DESTROY_SWAPCHAIN]             = (CtsCommandMetadata) { .handler = ctsDestroySwapchainKHRTrampoline,        .size = sizeof(CtsDestroySwapchainKHR),        .align = alignof(CtsDestroySwapchainKHR)        };

    lookup[CTS_COMMAND_CREATE_FRAMEBUFFER]            = (CtsCommandMetadata) { .handler = ctsCreateFramebufferTrampoline,          .size = sizeof(CtsCreateFramebuffer),          .align = alignof(CtsCreateFramebuffer)          };
    lookup[CTS_COMMAND_DESTROY_FRAMEBUFFER]           = (CtsCommandMetadata) { .handler = ctsDestroyFramebufferTrampoline,         .size = sizeof(CtsDestroyFramebuffer),         .align = alignof(CtsDestroyFramebuffer)         };

    lookup[CTS_COMMAND_CREATE_FENCE]                  = (CtsCommandMetadata) { .handler = ctsCreateFenceTrampoline,                .size = sizeof(CtsCreateFence),                .align = alignof(CtsCreateFence)                };
    lookup[CTS_COMMAND_RESET_FENCES]                  = (CtsCommandMetadata) { .handler = ctsResetFencesTrampoline,                .size = sizeof(CtsResetFences),                .align = alignof(CtsResetFences)                };
    lookup[CTS_COMMAND_GET_FENCE_STATUS]              = (CtsCommandMetadata) { .handler = ctsGetFenceStatusTrampoline,             .size = sizeof(CtsGetFenceStatus),             .align = alignof(CtsGetFenceStatus)             };
    lookup[CTS_COMMAND_SIGNAL_FENCE]                  = (CtsCommandMetadata) { .handler = ctsSignalFenceTrampoline,                .size = sizeof(CtsSignalFence),                .align = alignof(CtsSignalFence)                };
    lookup[CTS_COMMAND_WAIT_FOR_FENCES]               = (CtsCommandMetadata) { .handler = ctsWaitForFencesTrampoline,              .size = sizeof(CtsWaitForFences),              .align = alignof(CtsWaitForFences)              };
    lookup[CTS_COMMAND_DESTROY_FENCE]                 = (CtsCommandMetadata) { .handler = ctsDestroyFenceTrampoline,               .size = sizeof(CtsDestroyFence),               .align = alignof(CtsDestroyFence)               };

    lookup[CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = ctsAllocateDescriptorSetsTrampoline,     .size = sizeof(CtsAllocateDescriptorSets),     .align = alignof(CtsAllocateDescriptorSets)     };
    lookup[CTS_COMMAND_UPDATE_DESCRIPTOR_SETS]        = (CtsCommandMetadata) { .handler = ctsUpdateDescriptorSetsTrampoline,       .size = sizeof(CtsUpdateDescriptorSets),       .align = alignof(CtsUpdateDescriptorSets)       };
    lookup[CTS_COMMAND_FREE_DESCRIPTOR_SETS]          = (CtsCommandMetadata) { .handler = ctsFreeDescriptorSetsTrampoline,         .size = sizeof(CtsFreeDescriptorSets),         .align = alignof(CtsFreeDescriptorSets)         };

    lookup[CTS_COMMAND_ALLOCATE_MEMORY]               = (CtsCommandMetadata) { .handler = ctsAllocateMemoryTrampoline,             .size = sizeof(CtsAllocateMemory),             .align = alignof(CtsAllocateMemory)             };
    lookup[CTS_COMMAND_MAP_MEMORY]                    = (CtsCommandMetadata) { .handler = ctsMapMemoryTrampoline,                  .size = sizeof(CtsMapMemory),                  .align = alignof(CtsMapMemory)                  };
    lookup[CTS_COMMAND_UNMAP_MEMORY]                  = (CtsCommandMetadata) { .handler = ctsUnmapMemoryTrampoline,                .size = sizeof(CtsUnmapMemory),                .align = alignof(CtsUnmapMemory)                };
    lookup[CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES]    = (CtsCommandMetadata) { .handler = ctsFlushMappedMemoryRangesTrampoline,    .size = sizeof(CtsFlushMappedMemoryRanges),    .align = alignof(CtsFlushMappedMemoryRanges)    };
    lookup[CTS_COMMAND_FREE_MEMORY]                   = (CtsCommandMetadata) { .handler = ctsFreeMemoryTrampoline,                 .size = sizeof(CtsFreeMemory),                 .align = alignof(CtsFreeMemory)                 };

    lookup[CTS_COMMAND_QUEUE_PRESENT]                 = (CtsCommandMetadata) { .handler = ctsQueuePresentKHRTrampoline,            .size = sizeof(CtsQueuePresentKHR),            .align = alignof(CtsQueuePresentKHR)            };

    lookup[CTS_COMMAND_CMD_BEGIN_QUERY]               = (CtsCommandMetadata) { .handler = ctsCmdBeginQueryTrampoline,              .size = sizeof(CtsCmdBeginQuery),              .align = alignof(CtsCmdBeginQuery)              };
    lookup[CTS_COMMAND_CMD_BEGIN_RENDER_PASS]         = (CtsCommandMetadata) { .handler = ctsCmdBeginRenderPassTrampoline,         .size = sizeof(CtsCmdBeginRenderPass),         .align = alignof(CtsCmdBeginRenderPass)         };
    lookup[CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = ctsCmdBindDescriptorSetsTrampoline,      .size = sizeof(CtsCmdBindDescriptorSets),      .align = alignof(CtsCmdBindDescriptorSets)      };
    lookup[CTS_COMMAND_CMD_BIND_INDEX_BUFFER]         = (CtsCommandMetadata) { .handler = ctsCmdBindIndexBufferTrampoline,         .size = sizeof(CtsCmdBindIndexBuffer),         .align = alignof(CtsCmdBindIndexBuffer)         };
    lookup[CTS_COMMAND_CMD_BIND_PIPELINE]             = (CtsCommandMetadata) { .handler = ctsCmdBindPipelineTrampoline,            .size = sizeof(CtsCmdBindPipeline),            .align = alignof(CtsCmdBindPipeline)            };
    lookup[CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS]       = (CtsCommandMetadata) { .handler = ctsCmdBindVertexBuffersTrampoline,       .size = sizeof(CtsCmdBindVertexBuffers),       .align = alignof(CtsCmdBindVertexBuffers)       };
    lookup[CTS_COMMAND_CMD_BLIT_IMAGE]                = (CtsCommandMetadata) { .handler = ctsCmdBlitImageTrampoline,               .size = sizeof(CtsCmdBlitImage),               .align = alignof(CtsCmdBlitImage)               };
    lookup[CTS_COMMAND_CMD_CLEAR_ATTACHMENTS]         = (CtsCommandMetadata) { .handler = ctsCmdClearAttachmentsTrampoline,        .size = sizeof(CtsCmdClearAttachments),        .align = alignof(CtsCmdClearAttachments)        };
    lookup[CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE]         = (CtsCommandMetadata) { .handler = ctsCmdClearColorImageTrampoline,         .size = sizeof(CtsCmdClearColorImage),         .align = alignof(CtsCmdClearColorImage)         };
    lookup[CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE] = (CtsCommandMetadata) { .handler = ctsCmdClearDepthStencilImageTrampoline,  .size = sizeof(CtsCmdClearDepthStencilImage),  .align = alignof(CtsCmdClearDepthStencilImage)  };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER]               = (CtsCommandMetadata) { .handler = ctsCmdCopyBufferTrampoline,              .size = sizeof(CtsCmdCopyBuffer),              .align = alignof(CtsCmdCopyBuffer)              };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE]      = (CtsCommandMetadata) { .handler = ctsCmdCopyBufferToImageTrampoline,       .size = sizeof(CtsCmdCopyBufferToImage),       .align = alignof(CtsCmdCopyBufferToImage)       };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE]                = (CtsCommandMetadata) { .handler = ctsCmdCopyImageTrampoline,               .size = sizeof(CtsCmdCopyImage),               .align = alignof(CtsCmdCopyImage)               };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER]      = (CtsCommandMetadata) { .handler = ctsCmdCopyImageToBufferTrampoline,       .size = sizeof(CtsCmdCopyImageToBuffer),       .align = alignof(CtsCmdCopyImageToBuffer)       };
    lookup[CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS]   = (CtsCommandMetadata) { .handler = ctsCmdCopyQueryPoolResultsTrampoline,    .size = sizeof(CtsCmdCopyQueryPoolResults),    .align = alignof(CtsCmdCopyQueryPoolResults)    };
    lookup[CTS_COMMAND_CMD_DISPATCH]                  = (CtsCommandMetadata) { .handler = ctsCmdDispatchTrampoline,                .size = sizeof(CtsCmdDispatch),                .align = alignof(CtsCmdDispatch)                };
    lookup[CTS_COMMAND_CMD_DISPATCH_INDIRECT]         = (CtsCommandMetadata) { .handler = ctsCmdDispatchIndirectTrampoline,        .size = sizeof(CtsCmdDispatchIndirect),        .align = alignof(CtsCmdDispatchIndirect)        };
    lookup[CTS_COMMAND_CMD_DRAW]                      = (CtsCommandMetadata) { .handler = ctsCmdDrawTrampoline,                    .size = sizeof(CtsCmdDraw),                    .align = alignof(CtsCmdDraw)                    };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED]              = (CtsCommandMetadata) { .handler = ctsCmdDrawIndexedTrampoline,             .size = sizeof(CtsCmdDrawIndexed),             .align = alignof(CtsCmdDrawIndexed)             };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT]     = (CtsCommandMetadata) { .handler = ctsCmdDrawIndexedIndirectTrampoline,     .size = sizeof(CtsCmdDrawIndexedIndirect),     .align = alignof(CtsCmdDrawIndexedIndirect)     };
    lookup[CTS_COMMAND_CMD_DRAW_INDIRECT]             = (CtsCommandMetadata) { .handler = ctsCmdDrawIndirectTrampoline,            .size = sizeof(CtsCmdDrawIndirect),            .align = alignof(CtsCmdDrawIndirect)            };
    lookup[CTS_COMMAND_CMD_END_QUERY]                 = (CtsCommandMetadata) { .handler = ctsCmdEndQueryTrampoline,                .size = sizeof(CtsCmdEndQuery),                .align = alignof(CtsCmdEndQuery)                };
    lookup[CTS_COMMAND_CMD_END_RENDER_PASS]           = (CtsCommandMetadata) { .handler = ctsCmdEndRenderPassTrampoline,           .size = sizeof(CtsCmdEndRenderPass),           .align = alignof(CtsCmdEndRenderPass)           };
    lookup[CTS_COMMAND_CMD_EXECUTE_COMMANDS]          = (CtsCommandMetadata) { .handler = ctsCmdExecuteCommandsTrampoline,         .size = sizeof(CtsCmdExecuteCommands),         .align = alignof(CtsCmdExecuteCommands)         };
    lookup[CTS_COMMAND_CMD_FILL_BUFFER]               = (CtsCommandMetadata) { .handler = ctsCmdFillBufferTrampoline,              .size = sizeof(CtsCmdFillBuffer),              .align = alignof(CtsCmdFillBuffer)              };
    lookup[CTS_COMMAND_CMD_NEXT_SUBPASS]              = (CtsCommandMetadata) { .handler = ctsCmdNextSubpassTrampoline,             .size = sizeof(CtsCmdNextSubpass),             .align = alignof(CtsCmdNextSubpass)             };
    lookup[CTS_COMMAND_CMD_PIPELINE_BARRIER]          = (CtsCommandMetadata) { .handler = ctsCmdPipelineBarrierTrampoline,         .size = sizeof(CtsCmdPipelineBarrier),         .align = alignof(CtsCmdPipelineBarrier)         };
    lookup[CTS_COMMAND_CMD_PUSH_CONSTANTS]            = (CtsCommandMetadata) { .handler = ctsCmdPushConstantsTrampoline,           .size = sizeof(CtsCmdPushConstants),           .align = alignof(CtsCmdPushConstants)           };
    lookup[CTS_COMMAND_CMD_RESET_EVENT]               = (CtsCommandMetadata) { .handler = ctsCmdResetEventTrampoline,              .size = sizeof(CtsCmdResetEvent),              .align = alignof(CtsCmdResetEvent)              };
    lookup[CTS_COMMAND_CMD_RESET_QUERY_POOL]          = (CtsCommandMetadata) { .handler = ctsCmdResetQueryPoolTrampoline,          .size = sizeof(CtsCmdResetQueryPool),          .align = alignof(CtsCmdResetQueryPool)          };
    lookup[CTS_COMMAND_CMD_RESOLVE_IMAGE]             = (CtsCommandMetadata) { .handler = ctsCmdResolveImageTrampoline,            .size = sizeof(CtsCmdResolveImage),            .align = alignof(CtsCmdResolveImage)            };
    lookup[CTS_COMMAND_CMD_SET_BLEND_CONSTANTS]       = (CtsCommandMetadata) { .handler = ctsCmdSetBlendConstantsTrampoline,       .size = sizeof(CtsCmdSetBlendConstants),       .align = alignof(CtsCmdSetBlendConstants)       };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BIAS]            = (CtsCommandMetadata) { .handler = ctsCmdSetDepthBiasTrampoline,            .size = sizeof(CtsCmdSetDepthBias),            .align = alignof(CtsCmdSetDepthBias)            };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BOUNDS]          = (CtsCommandMetadata) { .handler = ctsCmdSetDepthBoundsTrampoline,          .size = sizeof(CtsCmdSetDepthBounds),          .align = alignof(CtsCmdSetDepthBounds)          };
    lookup[CTS_COMMAND_CMD_SET_DEVICE_MASK]           = (CtsCommandMetadata) { .handler = ctsCmdSetDeviceMaskTrampoline,           .size = sizeof(CtsCmdSetDeviceMask),           .align = alignof(CtsCmdSetDeviceMask)           };
    lookup[CTS_COMMAND_CMD_SET_EVENT]                 = (CtsCommandMetadata) { .handler = ctsCmdSetEventTrampoline,                .size = sizeof(CtsCmdSetEvent),                .align = alignof(CtsCmdSetEvent)                };
    lookup[CTS_COMMAND_CMD_SET_LINE_WIDTH]            = (CtsCommandMetadata) { .handler = ctsCmdSetLineWidthTrampoline,            .size = sizeof(CtsCmdSetLineWidth),            .align = alignof(CtsCmdSetLineWidth)            };
    lookup[CTS_COMMAND_CMD_SET_SCISSOR]               = (CtsCommandMetadata) { .handler = ctsCmdSetScissorTrampoline,              .size = sizeof(CtsCmdSetScissor),              .align = alignof(CtsCmdSetScissor)              };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK]  = (CtsCommandMetadata) { .handler = ctsCmdSetStencilCompareMaskTrampoline,   .size = sizeof(CtsCmdSetStencilCompareMask),   .align = alignof(CtsCmdSetStencilCompareMask)   };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_REFERENCE]     = (CtsCommandMetadata) { .handler = ctsCmdSetStencilReferenceTrampoline,     .size = sizeof(CtsCmdSetStencilReference),     .align = alignof(CtsCmdSetStencilReference)     };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK]    = (CtsCommandMetadata) { .handler = ctsCmdSetStencilWriteMaskTrampoline,     .size = sizeof(CtsCmdSetStencilWriteMask),     .align = alignof(CtsCmdSetStencilWriteMask)     };
    lookup[CTS_COMMAND_CMD_SET_VIEWPORT]              = (CtsCommandMetadata) { .handler = ctsCmdSetViewportTrampoline,             .size = sizeof(CtsCmdSetViewport),             .align = alignof(CtsCmdSetViewport)             };
    lookup[CTS_COMMAND_CMD_UPDATE_BUFFER]             = (CtsCommandMetadata) { .handler = ctsCmdUpdateBufferTrampoline,            .size = sizeof(CtsCmdUpdateBuffer),            .align = alignof(CtsCmdUpdateBuffer)            };
    lookup[CTS_COMMAND_CMD_WAIT_EVENTS]               = (CtsCommandMetadata) { .handler = ctsCmdWaitEventsTrampoline,              .size = sizeof(CtsCmdWaitEvents),              .align = alignof(CtsCmdWaitEvents)              };
    lookup[CTS_COMMAND_CMD_WRITE_TIMESTAMP]           = (CtsCommandMetadata) { .handler = ctsCmdWriteTimestampTrampoline,          .size = sizeof(CtsCmdWriteTimestamp),          .align = alignof(CtsCmdWriteTimestamp)          };

    lookup[CTS_COMMAND_CREATE_WIN32_SURFACE_KHR]      = (CtsCommandMetadata) { .handler = ctsCreateWin32SurfaceKHRTrampoline,      .size = sizeof(CtsCreateWin32SurfaceKHR),      .align = alignof(CtsCreateWin32SurfaceKHR)      };
    lookup[CTS_COMMAND_DESTROY_SURFACE_KHR]           = (CtsCommandMetadata) { .handler = ctsDestroySurfaceKHRTrampoline,          .size = sizeof(CtsDestroySurfaceKHR),          .align = alignof(CtsDestroySurfaceKHR)          };

    return lookup;
}
