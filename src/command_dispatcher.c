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

    lookup[CTS_COMMAND_CREATE_GRAPHICS_PIPELINES]     = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateGraphicsPipelines),    .size = sizeof(CtsCreateGraphicsPipelines),    .align = alignof(CtsCreateGraphicsPipelines)    };
    lookup[CTS_COMMAND_DESTROY_PIPELINE]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroyPipeline),            .size = sizeof(CtsDestroyPipeline),            .align = alignof(CtsDestroyPipeline)            };

    lookup[CTS_COMMAND_CREATE_IMAGE_VIEW]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateImageView),            .size = sizeof(CtsCreateImageView),            .align = alignof(CtsCreateImageView)            };
    lookup[CTS_COMMAND_DESTROY_IMAGE_VIEW]            = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroyImageView),           .size = sizeof(CtsDestroyImageView),           .align = alignof(CtsDestroyImageView)           };

    lookup[CTS_COMMAND_CREATE_IMAGE]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateImage),                .size = sizeof(CtsCreateImage),                .align = alignof(CtsCreateImage)                };
    lookup[CTS_COMMAND_DESTROY_IMAGE]                 = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroyImage),               .size = sizeof(CtsDestroyImage),               .align = alignof(CtsDestroyImage)               };

    lookup[CTS_COMMAND_CREATE_SAMPLER]                = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateSampler),              .size = sizeof(CtsCreateSampler),              .align = alignof(CtsCreateSampler)              };
    lookup[CTS_COMMAND_DESTROY_SAMPLER]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroySampler),             .size = sizeof(CtsDestroySampler),             .align = alignof(CtsDestroySampler)             };

    lookup[CTS_COMMAND_CREATE_SWAPCHAIN]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateSwapchainKHR),         .size = sizeof(CtsCreateSwapchainKHR),         .align = alignof(CtsCreateSwapchainKHR)         };
    lookup[CTS_COMMAND_DESTROY_SWAPCHAIN]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroySwapchainKHR),        .size = sizeof(CtsDestroySwapchainKHR),        .align = alignof(CtsDestroySwapchainKHR)        };

    lookup[CTS_COMMAND_CREATE_FRAMEBUFFER]            = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateFramebuffer),          .size = sizeof(CtsCreateFramebuffer),          .align = alignof(CtsCreateFramebuffer)          };
    lookup[CTS_COMMAND_DESTROY_FRAMEBUFFER]           = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroyFramebuffer),         .size = sizeof(CtsDestroyFramebuffer),         .align = alignof(CtsDestroyFramebuffer)         };

    lookup[CTS_COMMAND_CREATE_FENCE]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateFence),                .size = sizeof(CtsCreateFence),                .align = alignof(CtsCreateFence)                };
    lookup[CTS_COMMAND_RESET_FENCES]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsResetFences),                .size = sizeof(CtsResetFences),                .align = alignof(CtsResetFences)                };
    lookup[CTS_COMMAND_GET_FENCE_STATUS]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsGetFenceStatus),             .size = sizeof(CtsGetFenceStatus),             .align = alignof(CtsGetFenceStatus)             };
    lookup[CTS_COMMAND_SIGNAL_FENCE]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsSignalFence),                .size = sizeof(CtsSignalFence),                .align = alignof(CtsSignalFence)                };
    lookup[CTS_COMMAND_WAIT_FOR_FENCES]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsWaitForFences),              .size = sizeof(CtsWaitForFences),              .align = alignof(CtsWaitForFences)              };
    lookup[CTS_COMMAND_DESTROY_FENCE]                 = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroyFence),               .size = sizeof(CtsDestroyFence),               .align = alignof(CtsDestroyFence)               };

    lookup[CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsAllocateDescriptorSets),     .size = sizeof(CtsAllocateDescriptorSets),     .align = alignof(CtsAllocateDescriptorSets)     };
    lookup[CTS_COMMAND_UPDATE_DESCRIPTOR_SETS]        = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsUpdateDescriptorSets),       .size = sizeof(CtsUpdateDescriptorSets),       .align = alignof(CtsUpdateDescriptorSets)       };
    lookup[CTS_COMMAND_FREE_DESCRIPTOR_SETS]          = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsFreeDescriptorSets),         .size = sizeof(CtsFreeDescriptorSets),         .align = alignof(CtsFreeDescriptorSets)         };

    lookup[CTS_COMMAND_ALLOCATE_MEMORY]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsAllocateMemory),             .size = sizeof(CtsAllocateMemory),             .align = alignof(CtsAllocateMemory)             };
    lookup[CTS_COMMAND_MAP_MEMORY]                    = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsMapMemory),                  .size = sizeof(CtsMapMemory),                  .align = alignof(CtsMapMemory)                  };
    lookup[CTS_COMMAND_UNMAP_MEMORY]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsUnmapMemory),                .size = sizeof(CtsUnmapMemory),                .align = alignof(CtsUnmapMemory)                };
    lookup[CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES]    = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsFlushMappedMemoryRanges),    .size = sizeof(CtsFlushMappedMemoryRanges),    .align = alignof(CtsFlushMappedMemoryRanges)    };
    lookup[CTS_COMMAND_FREE_MEMORY]                   = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsFreeMemory),                 .size = sizeof(CtsFreeMemory),                 .align = alignof(CtsFreeMemory)                 };

    lookup[CTS_COMMAND_QUEUE_PRESENT]                 = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsQueuePresentKHR),            .size = sizeof(CtsQueuePresentKHR),            .align = alignof(CtsQueuePresentKHR)            };

    lookup[CTS_COMMAND_CMD_BEGIN_QUERY]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBeginQuery),              .size = sizeof(CtsCmdBeginQuery),              .align = alignof(CtsCmdBeginQuery)              };
    lookup[CTS_COMMAND_CMD_BEGIN_RENDER_PASS]         = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBeginRenderPass),         .size = sizeof(CtsCmdBeginRenderPass),         .align = alignof(CtsCmdBeginRenderPass)         };
    lookup[CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBindDescriptorSets),      .size = sizeof(CtsCmdBindDescriptorSets),      .align = alignof(CtsCmdBindDescriptorSets)      };
    lookup[CTS_COMMAND_CMD_BIND_INDEX_BUFFER]         = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBindIndexBuffer),         .size = sizeof(CtsCmdBindIndexBuffer),         .align = alignof(CtsCmdBindIndexBuffer)         };
    lookup[CTS_COMMAND_CMD_BIND_PIPELINE]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBindPipeline),            .size = sizeof(CtsCmdBindPipeline),            .align = alignof(CtsCmdBindPipeline)            };
    lookup[CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS]       = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBindVertexBuffers),       .size = sizeof(CtsCmdBindVertexBuffers),       .align = alignof(CtsCmdBindVertexBuffers)       };
    lookup[CTS_COMMAND_CMD_BLIT_IMAGE]                = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdBlitImage),               .size = sizeof(CtsCmdBlitImage),               .align = alignof(CtsCmdBlitImage)               };
    lookup[CTS_COMMAND_CMD_CLEAR_ATTACHMENTS]         = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdClearAttachments),        .size = sizeof(CtsCmdClearAttachments),        .align = alignof(CtsCmdClearAttachments)        };
    lookup[CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE]         = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdClearColorImage),         .size = sizeof(CtsCmdClearColorImage),         .align = alignof(CtsCmdClearColorImage)         };
    lookup[CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE] = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdClearDepthStencilImage),  .size = sizeof(CtsCmdClearDepthStencilImage),  .align = alignof(CtsCmdClearDepthStencilImage)  };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdCopyBuffer),              .size = sizeof(CtsCmdCopyBuffer),              .align = alignof(CtsCmdCopyBuffer)              };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE]      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdCopyBufferToImage),       .size = sizeof(CtsCmdCopyBufferToImage),       .align = alignof(CtsCmdCopyBufferToImage)       };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE]                = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdCopyImage),               .size = sizeof(CtsCmdCopyImage),               .align = alignof(CtsCmdCopyImage)               };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER]      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdCopyImageToBuffer),       .size = sizeof(CtsCmdCopyImageToBuffer),       .align = alignof(CtsCmdCopyImageToBuffer)       };
    lookup[CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS]   = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdCopyQueryPoolResults),    .size = sizeof(CtsCmdCopyQueryPoolResults),    .align = alignof(CtsCmdCopyQueryPoolResults)    };
    lookup[CTS_COMMAND_CMD_DISPATCH]                  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDispatch),                .size = sizeof(CtsCmdDispatch),                .align = alignof(CtsCmdDispatch)                };
    lookup[CTS_COMMAND_CMD_DISPATCH_INDIRECT]         = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDispatchIndirect),        .size = sizeof(CtsCmdDispatchIndirect),        .align = alignof(CtsCmdDispatchIndirect)        };
    lookup[CTS_COMMAND_CMD_DRAW]                      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDraw),                    .size = sizeof(CtsCmdDraw),                    .align = alignof(CtsCmdDraw)                    };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDrawIndexed),             .size = sizeof(CtsCmdDrawIndexed),             .align = alignof(CtsCmdDrawIndexed)             };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT]     = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDrawIndexedIndirect),     .size = sizeof(CtsCmdDrawIndexedIndirect),     .align = alignof(CtsCmdDrawIndexedIndirect)     };
    lookup[CTS_COMMAND_CMD_DRAW_INDIRECT]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdDrawIndirect),            .size = sizeof(CtsCmdDrawIndirect),            .align = alignof(CtsCmdDrawIndirect)            };
    lookup[CTS_COMMAND_CMD_END_QUERY]                 = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdEndQuery),                .size = sizeof(CtsCmdEndQuery),                .align = alignof(CtsCmdEndQuery)                };
    lookup[CTS_COMMAND_CMD_END_RENDER_PASS]           = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdEndRenderPass),           .size = sizeof(CtsCmdEndRenderPass),           .align = alignof(CtsCmdEndRenderPass)           };
    lookup[CTS_COMMAND_CMD_EXECUTE_COMMANDS]          = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdExecuteCommands),         .size = sizeof(CtsCmdExecuteCommands),         .align = alignof(CtsCmdExecuteCommands)         };
    lookup[CTS_COMMAND_CMD_FILL_BUFFER]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdFillBuffer),              .size = sizeof(CtsCmdFillBuffer),              .align = alignof(CtsCmdFillBuffer)              };
    lookup[CTS_COMMAND_CMD_NEXT_SUBPASS]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdNextSubpass),             .size = sizeof(CtsCmdNextSubpass),             .align = alignof(CtsCmdNextSubpass)             };
    lookup[CTS_COMMAND_CMD_PIPELINE_BARRIER]          = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdPipelineBarrier),         .size = sizeof(CtsCmdPipelineBarrier),         .align = alignof(CtsCmdPipelineBarrier)         };
    lookup[CTS_COMMAND_CMD_PUSH_CONSTANTS]            = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdPushConstants),           .size = sizeof(CtsCmdPushConstants),           .align = alignof(CtsCmdPushConstants)           };
    lookup[CTS_COMMAND_CMD_RESET_EVENT]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdResetEvent),              .size = sizeof(CtsCmdResetEvent),              .align = alignof(CtsCmdResetEvent)              };
    lookup[CTS_COMMAND_CMD_RESET_QUERY_POOL]          = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdResetQueryPool),          .size = sizeof(CtsCmdResetQueryPool),          .align = alignof(CtsCmdResetQueryPool)          };
    lookup[CTS_COMMAND_CMD_RESOLVE_IMAGE]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdResolveImage),            .size = sizeof(CtsCmdResolveImage),            .align = alignof(CtsCmdResolveImage)            };
    lookup[CTS_COMMAND_CMD_SET_BLEND_CONSTANTS]       = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetBlendConstants),       .size = sizeof(CtsCmdSetBlendConstants),       .align = alignof(CtsCmdSetBlendConstants)       };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BIAS]            = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetDepthBias),            .size = sizeof(CtsCmdSetDepthBias),            .align = alignof(CtsCmdSetDepthBias)            };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BOUNDS]          = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetDepthBounds),          .size = sizeof(CtsCmdSetDepthBounds),          .align = alignof(CtsCmdSetDepthBounds)          };
    lookup[CTS_COMMAND_CMD_SET_DEVICE_MASK]           = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetDeviceMask),           .size = sizeof(CtsCmdSetDeviceMask),           .align = alignof(CtsCmdSetDeviceMask)           };
    lookup[CTS_COMMAND_CMD_SET_EVENT]                 = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetEvent),                .size = sizeof(CtsCmdSetEvent),                .align = alignof(CtsCmdSetEvent)                };
    lookup[CTS_COMMAND_CMD_SET_LINE_WIDTH]            = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetLineWidth),            .size = sizeof(CtsCmdSetLineWidth),            .align = alignof(CtsCmdSetLineWidth)            };
    lookup[CTS_COMMAND_CMD_SET_SCISSOR]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetScissor),              .size = sizeof(CtsCmdSetScissor),              .align = alignof(CtsCmdSetScissor)              };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK]  = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetStencilCompareMask),   .size = sizeof(CtsCmdSetStencilCompareMask),   .align = alignof(CtsCmdSetStencilCompareMask)   };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_REFERENCE]     = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetStencilReference),     .size = sizeof(CtsCmdSetStencilReference),     .align = alignof(CtsCmdSetStencilReference)     };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK]    = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetStencilWriteMask),     .size = sizeof(CtsCmdSetStencilWriteMask),     .align = alignof(CtsCmdSetStencilWriteMask)     };
    lookup[CTS_COMMAND_CMD_SET_VIEWPORT]              = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdSetViewport),             .size = sizeof(CtsCmdSetViewport),             .align = alignof(CtsCmdSetViewport)             };
    lookup[CTS_COMMAND_CMD_UPDATE_BUFFER]             = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdUpdateBuffer),            .size = sizeof(CtsCmdUpdateBuffer),            .align = alignof(CtsCmdUpdateBuffer)            };
    lookup[CTS_COMMAND_CMD_WAIT_EVENTS]               = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdWaitEvents),              .size = sizeof(CtsCmdWaitEvents),              .align = alignof(CtsCmdWaitEvents)              };
    lookup[CTS_COMMAND_CMD_WRITE_TIMESTAMP]           = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCmdWriteTimestamp),          .size = sizeof(CtsCmdWriteTimestamp),          .align = alignof(CtsCmdWriteTimestamp)          };

    lookup[CTS_COMMAND_CREATE_WIN32_SURFACE_KHR]      = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsCreateWin32SurfaceKHR),      .size = sizeof(CtsCreateWin32SurfaceKHR),      .align = alignof(CtsCreateWin32SurfaceKHR)      };
    lookup[CTS_COMMAND_DESTROY_SURFACE_KHR]           = (CtsCommandMetadata) { .handler = CTS_TRAMPOLINE_FN(CtsDestroySurfaceKHR),          .size = sizeof(CtsDestroySurfaceKHR),          .align = alignof(CtsDestroySurfaceKHR)          };

    return lookup;
}
