#include <assert.h>
#include <stddef.h>
#include <cts/renderer_impl.h>
#include <cts/renderer_proxy.h>
#include <cts/commands.h>

#pragma region StaticMethodDeclarations

static CtsCommandMetadata* createCommandMetadataLookup();

static void handleAllocateMemory(const CtsCmdBase* pCmd);
static void handleMapMemory(const CtsCmdBase* pCmd);
static void handleUnmapMemory(const CtsCmdBase* pCmd);
static void handleFreeMemory(const CtsCmdBase* pCmd);

static void handleQueueSubmit(const CtsCmdBase* pCmd);
static void handleSignalSemaphores(const CtsCmdBase* pCmd);

static void handleAllocateCommandBuffers(const CtsCmdBase* pCmd);
static void handleFreeCommandBuffers(const CtsCmdBase* pCmd);

static void handleAllocateDescriptorSets(const CtsCmdBase* pCmd);
static void handleUpdateDescriptorSets(const CtsCmdBase* pCmd);
static void handleFreeDescriptorSets(const CtsCmdBase* pCmd);

static void handleCreateBuffer(const CtsCmdBase* pCmd);
static void handleBindBufferMemory(const CtsCmdBase* pCmd);
static void handleDestroyBuffer(const CtsCmdBase* pCmd);

static void handleCreateCommandPool(const CtsCmdBase* pCmd);
static void handleDestroyCommandPool(const CtsCmdBase* pCmd);

static void handleCreateDescriptorPool(const CtsCmdBase* pCmd);
static void handleDestroyDescriptorPool(const CtsCmdBase* pCmd);

static void handleCreateDescriptorSetLayout(const CtsCmdBase* pCmd);
static void handleDestroyDescriptorSetLayout(const CtsCmdBase* pCmd);

static void handleCreateGraphicsPipelines(const CtsCmdBase* pCmd);
static void handleDestroyPipeline(const CtsCmdBase* pCmd);

static void handleCreateImageView(const CtsCmdBase* pCmd);
static void handleDestroyImageView(const CtsCmdBase* pCmd);

static void handleCreateImage(const CtsCmdBase* pCmd);
static void handleDestroyImage(const CtsCmdBase* pCmd);

static void handleCreatePipelineLayout(const CtsCmdBase* pCmd);
static void handleDestroyPipelineLayout(const CtsCmdBase* pCmd);

static void handleCreateShaderModule(const CtsCmdBase* pCmd);
static void handleDestroyShaderModule(const CtsCmdBase* pCmd);

static void handleCreateFence(const CtsCmdBase* pCmd);
static void handleDestroyFence(const CtsCmdBase* pCmd);

static void handleCmdBeginQuery(const CtsCmdBase* pCmd);
static void handleCmdBeginRenderPass(const CtsCmdBase* pCmd);
static void handleCmdBindDescriptorSets(const CtsCmdBase* pCmd);
static void handleCmdBindIndexBuffer(const CtsCmdBase* pCmd);
static void handleCmdBindPipeline(const CtsCmdBase* pCmd);
static void handleCmdBindVertexBuffers(const CtsCmdBase* pCmd);
static void handleCmdBlitImage(const CtsCmdBase* pCmd);
static void handleCmdClearAttachments(const CtsCmdBase* pCmd);
static void handleCmdClearColorImage(const CtsCmdBase* pCmd);
static void handleCmdClearDepthStencilImage(const CtsCmdBase* pCmd);
static void handleCmdCopyBuffer(const CtsCmdBase* pCmd);
static void handleCmdCopyBufferToImage(const CtsCmdBase* pCmd);
static void handleCmdCopyImage(const CtsCmdBase* pCmd);
static void handleCmdCopyImageToBuffer(const CtsCmdBase* pCmd);
static void handleCmdCopyQueryPoolResults(const CtsCmdBase* pCmd);
static void handleCmdDispatch(const CtsCmdBase* pCmd);
static void handleCmdDispatchIndirect(const CtsCmdBase* pCmd);
static void handleCmdDraw(const CtsCmdBase* pCmd);
static void handleCmdDrawIndexed(const CtsCmdBase* pCmd);
static void handleCmdDrawIndexedIndirect(const CtsCmdBase* pCmd);
static void handleCmdDrawIndirect(const CtsCmdBase* pCmd);
static void handleCmdEndQuery(const CtsCmdBase* pCmd);
static void handleCmdEndRenderPass(const CtsCmdBase* pCmd);
static void handleCmdExecuteCommands(const CtsCmdBase* pCmd);
static void handleCmdFillBuffer(const CtsCmdBase* pCmd);
static void handleCmdNextSubpass(const CtsCmdBase* pCmd);
static void handleCmdPipelineBarrier(const CtsCmdBase* pCmd);
static void handleCmdPushConstants(const CtsCmdBase* pCmd);
static void handleCmdResetEvent(const CtsCmdBase* pCmd);
static void handleCmdResetQueryPool(const CtsCmdBase* pCmd);
static void handleCmdResolveImage(const CtsCmdBase* pCmd);
static void handleCmdSetBlendConstants(const CtsCmdBase* pCmd);
static void handleCmdSetDepthBias(const CtsCmdBase* pCmd);
static void handleCmdSetDepthBounds(const CtsCmdBase* pCmd);
static void handleCmdSetDeviceMask(const CtsCmdBase* pCmd);
static void handleCmdSetEvent(const CtsCmdBase* pCmd);
static void handleCmdSetLineWidth(const CtsCmdBase* pCmd);
static void handleCmdSetScissor(const CtsCmdBase* pCmd);
static void handleCmdSetStencilCompareMask(const CtsCmdBase* pCmd);
static void handleCmdSetStencilReference(const CtsCmdBase* pCmd);
static void handleCmdSetStencilWriteMask(const CtsCmdBase* pCmd);
static void handleCmdSetViewport(const CtsCmdBase* pCmd);
static void handleCmdUpdateBuffer(const CtsCmdBase* pCmd);
static void handleCmdWaitEvents(const CtsCmdBase* pCmd);
static void handleCmdWriteTimestamp(const CtsCmdBase* pCmd);

#pragma endregion
#pragma region PublicMethodBodies

const CtsCommandMetadata* ctsGetCommandMetadata(CtsCommandType pCommandType) {
    static CtsCommandMetadata* commandMetadataLookup = NULL;

    if (commandMetadataLookup == NULL) {
        commandMetadataLookup = createCommandMetadataLookup();
    }

    CtsCommandMetadata* result = &commandMetadataLookup[pCommandType];
    assert(result->handler != NULL);

    return result;
}

#pragma endregion
#pragma region StaticMethodDeclarations

static CtsCommandMetadata* createCommandMetadataLookup()
{
    static CtsCommandMetadata lookup[NUM_CTS_COMMANDS];

    lookup[CTS_COMMAND_CREATE_BUFFER]                 = (CtsCommandMetadata) { .handler = handleCreateBuffer,               .size = sizeof(CtsCreateBuffer),               .align = alignof(CtsCreateBuffer)               };
    lookup[CTS_COMMAND_BIND_BUFFER_MEMORY]            = (CtsCommandMetadata) { .handler = handleBindBufferMemory,           .size = sizeof(CtsBindBufferMemory),           .align = alignof(CtsBindBufferMemory)           };
    lookup[CTS_COMMAND_DESTROY_BUFFER]                = (CtsCommandMetadata) { .handler = handleDestroyBuffer,              .size = sizeof(CtsDestroyBuffer),              .align = alignof(CtsDestroyBuffer)              };

    lookup[CTS_COMMAND_CREATE_COMMAND_POOL]           = (CtsCommandMetadata) { .handler = handleCreateCommandPool,          .size = sizeof(CtsCreateCommandPool),          .align = alignof(CtsCreateCommandPool)          };
    lookup[CTS_COMMAND_DESTROY_COMMAND_POOL]          = (CtsCommandMetadata) { .handler = handleDestroyCommandPool,         .size = sizeof(CtsDestroyCommandPool),         .align = alignof(CtsDestroyCommandPool)         };

    lookup[CTS_COMMAND_CREATE_DESCRIPTOR_POOL]        = (CtsCommandMetadata) { .handler = handleCreateDescriptorPool,       .size = sizeof(CtsCreateDescriptorPool),       .align = alignof(CtsCreateDescriptorPool)       };
    lookup[CTS_COMMAND_DESTROY_DESCRIPTOR_POOL]       = (CtsCommandMetadata) { .handler = handleDestroyDescriptorPool,      .size = sizeof(CtsDestroyDescriptorPool),      .align = alignof(CtsDestroyDescriptorPool)      };

    lookup[CTS_COMMAND_CREATE_DESCRIPTOR_SET_LAYOUT]  = (CtsCommandMetadata) { .handler = handleCreateDescriptorSetLayout,  .size = sizeof(CtsCreateDescriptorSetLayout),  .align = alignof(CtsCreateDescriptorSetLayout)  };
    lookup[CTS_COMMAND_DESTROY_DESCRIPTOR_SET_LAYOUT] = (CtsCommandMetadata) { .handler = handleDestroyDescriptorSetLayout, .size = sizeof(CtsDestroyDescriptorSetLayout), .align = alignof(CtsDestroyDescriptorSetLayout) };

    lookup[CTS_COMMAND_CREATE_GRAPHICS_PIPELINES]     = (CtsCommandMetadata) { .handler = handleCreateGraphicsPipelines,    .size = sizeof(CtsCreateGraphicsPipelines),    .align = alignof(CtsCreateGraphicsPipelines)    };
    lookup[CTS_COMMAND_DESTROY_PIPELINE]              = (CtsCommandMetadata) { .handler = handleDestroyPipeline,            .size = sizeof(CtsDestroyPipeline),            .align = alignof(CtsDestroyPipeline)            };

    lookup[CTS_COMMAND_CREATE_IMAGE_VIEW]             = (CtsCommandMetadata) { .handler = handleCreateImageView,            .size = sizeof(CtsCreateImageView),            .align = alignof(CtsCreateImageView)            };
    lookup[CTS_COMMAND_DESTROY_IMAGE_VIEW]            = (CtsCommandMetadata) { .handler = handleDestroyImageView,           .size = sizeof(CtsDestroyImageView),           .align = alignof(CtsDestroyImageView)           };

    lookup[CTS_COMMAND_CREATE_IMAGE]                  = (CtsCommandMetadata) { .handler = handleCreateImage,                .size = sizeof(CtsCreateImage),                .align = alignof(CtsCreateImage)                };
    lookup[CTS_COMMAND_DESTROY_IMAGE]                 = (CtsCommandMetadata) { .handler = handleDestroyImage,               .size = sizeof(CtsDestroyImage),               .align = alignof(CtsDestroyImage)               };

    lookup[CTS_COMMAND_CREATE_PIPELINE_LAYOUT]        = (CtsCommandMetadata) { .handler = handleCreatePipelineLayout,       .size = sizeof(CtsCreatePipelineLayout),       .align = alignof(CtsCreatePipelineLayout)       };
    lookup[CTS_COMMAND_DESTROY_PIPELINE_LAYOUT]       = (CtsCommandMetadata) { .handler = handleDestroyPipelineLayout,      .size = sizeof(CtsDestroyPipelineLayout),      .align = alignof(CtsDestroyPipelineLayout)      };

    lookup[CTS_COMMAND_CREATE_SHADER_MODULE]          = (CtsCommandMetadata) { .handler = handleCreateShaderModule,         .size = sizeof(CtsCreateShaderModule),         .align = alignof(CtsCreateShaderModule)         };
    lookup[CTS_COMMAND_DESTROY_SHADER_MODULE]         = (CtsCommandMetadata) { .handler = handleDestroyShaderModule,        .size = sizeof(CtsDestroyShaderModule),        .align = alignof(CtsDestroyShaderModule)        };

    lookup[CTS_COMMAND_CREATE_FENCE]                  = (CtsCommandMetadata) { .handler = handleCreateFence,                .size = sizeof(CtsCreateFence),                .align = alignof(CtsDestroyFence)               };
    lookup[CTS_COMMAND_DESTROY_FENCE]                 = (CtsCommandMetadata) { .handler = handleDestroyFence,               .size = sizeof(CtsDestroyShaderModule),        .align = alignof(CtsDestroyShaderModule)        };

    lookup[CTS_COMMAND_ALLOCATE_COMMAND_BUFFERS]      = (CtsCommandMetadata) { .handler = handleAllocateCommandBuffers,     .size = sizeof(CtsAllocateCommandBuffers),     .align = alignof(CtsAllocateCommandBuffers)     };
    lookup[CTS_COMMAND_FREE_COMMAND_BUFFERS]          = (CtsCommandMetadata) { .handler = handleFreeCommandBuffers,         .size = sizeof(CtsFreeCommandBuffers),         .align = alignof(CtsFreeCommandBuffers)         };

    lookup[CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = handleAllocateDescriptorSets,     .size = sizeof(CtsAllocateDescriptorSets),     .align = alignof(CtsAllocateDescriptorSets)     };
    lookup[CTS_COMMAND_UPDATE_DESCRIPTOR_SETS]        = (CtsCommandMetadata) { .handler = handleUpdateDescriptorSets,       .size = sizeof(CtsUpdateDescriptorSets),       .align = alignof(CtsUpdateDescriptorSets)       };
    lookup[CTS_COMMAND_FREE_DESCRIPTOR_SETS]          = (CtsCommandMetadata) { .handler = handleFreeDescriptorSets,         .size = sizeof(CtsFreeDescriptorSets),         .align = alignof(CtsFreeDescriptorSets)         };

    lookup[CTS_COMMAND_ALLOCATE_MEMORY]               = (CtsCommandMetadata) { .handler = handleAllocateMemory,             .size = sizeof(CtsAllocateMemory),             .align = alignof(CtsAllocateMemory)             };
    lookup[CTS_COMMAND_MAP_MEMORY]                    = (CtsCommandMetadata) { .handler = handleMapMemory,                  .size = sizeof(CtsMapMemory),                  .align = alignof(CtsMapMemory)                  };
    lookup[CTS_COMMAND_UNMAP_MEMORY]                  = (CtsCommandMetadata) { .handler = handleUnmapMemory,                .size = sizeof(CtsUnmapMemory),                .align = alignof(CtsUnmapMemory)                };
    lookup[CTS_COMMAND_FREE_MEMORY]                   = (CtsCommandMetadata) { .handler = handleFreeMemory,                 .size = sizeof(CtsFreeMemory),                 .align = alignof(CtsFreeMemory)                 };

    lookup[CTS_COMMAND_QUEUE_SUBMIT]                  = (CtsCommandMetadata) { .handler = handleQueueSubmit,                .size = sizeof(CtsQueueSubmit),                .align = alignof(CtsQueueSubmit)                };
    lookup[CTS_COMMAND_SIGNAL_SEMAPHORES]             = (CtsCommandMetadata) { .handler = handleSignalSemaphores,           .size = sizeof(CtsSignalSemaphores),           .align = alignof(CtsSignalSemaphores)           };


    lookup[CTS_COMMAND_CMD_BEGIN_QUERY]               = (CtsCommandMetadata) { .handler = handleCmdBeginQuery,              .size = sizeof(CtsCmdBeginQuery),              .align = alignof(CtsCmdBeginQuery)              };
    lookup[CTS_COMMAND_CMD_BEGIN_RENDER_PASS]         = (CtsCommandMetadata) { .handler = handleCmdBeginRenderPass,         .size = sizeof(CtsCmdBeginRenderPass),         .align = alignof(CtsCmdBeginRenderPass)         };
    lookup[CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS]      = (CtsCommandMetadata) { .handler = handleCmdBindDescriptorSets,      .size = sizeof(CtsCmdBindDescriptorSets),      .align = alignof(CtsCmdBindDescriptorSets)      };
    lookup[CTS_COMMAND_CMD_BIND_INDEX_BUFFER]         = (CtsCommandMetadata) { .handler = handleCmdBindIndexBuffer,         .size = sizeof(CtsCmdBindIndexBuffer),         .align = alignof(CtsCmdBindIndexBuffer)         };
    lookup[CTS_COMMAND_CMD_BIND_PIPELINE]             = (CtsCommandMetadata) { .handler = handleCmdBindPipeline,            .size = sizeof(CtsCmdBindPipeline),            .align = alignof(CtsCmdBindPipeline)            };
    lookup[CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS]       = (CtsCommandMetadata) { .handler = handleCmdBindVertexBuffers,       .size = sizeof(CtsCmdBindVertexBuffers),       .align = alignof(CtsCmdBindVertexBuffers)       };
    lookup[CTS_COMMAND_CMD_BLIT_IMAGE]                = (CtsCommandMetadata) { .handler = handleCmdBlitImage,               .size = sizeof(CtsCmdBlitImage),               .align = alignof(CtsCmdBlitImage)               };
    lookup[CTS_COMMAND_CMD_CLEAR_ATTACHMENTS]         = (CtsCommandMetadata) { .handler = handleCmdClearAttachments,        .size = sizeof(CtsCmdClearAttachments),        .align = alignof(CtsCmdClearAttachments)        };
    lookup[CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE]         = (CtsCommandMetadata) { .handler = handleCmdClearColorImage,         .size = sizeof(CtsCmdClearColorImage),         .align = alignof(CtsCmdClearColorImage)         };
    lookup[CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE] = (CtsCommandMetadata) { .handler = handleCmdClearDepthStencilImage,  .size = sizeof(CtsCmdClearDepthStencilImage),  .align = alignof(CtsCmdClearDepthStencilImage)  };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER]               = (CtsCommandMetadata) { .handler = handleCmdCopyBuffer,              .size = sizeof(CtsCmdCopyBuffer),              .align = alignof(CtsCmdCopyBuffer)              };
    lookup[CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE]      = (CtsCommandMetadata) { .handler = handleCmdCopyBufferToImage,       .size = sizeof(CtsCmdCopyBufferToImage),       .align = alignof(CtsCmdCopyBufferToImage)       };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE]                = (CtsCommandMetadata) { .handler = handleCmdCopyImage,               .size = sizeof(CtsCmdCopyImage),               .align = alignof(CtsCmdCopyImage)               };
    lookup[CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER]      = (CtsCommandMetadata) { .handler = handleCmdCopyImageToBuffer,       .size = sizeof(CtsCmdCopyImageToBuffer),       .align = alignof(CtsCmdCopyImageToBuffer)       };
    lookup[CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS]   = (CtsCommandMetadata) { .handler = handleCmdCopyQueryPoolResults,    .size = sizeof(CtsCmdCopyQueryPoolResults),    .align = alignof(CtsCmdCopyQueryPoolResults)    };
    lookup[CTS_COMMAND_CMD_DISPATCH]                  = (CtsCommandMetadata) { .handler = handleCmdDispatch,                .size = sizeof(CtsCmdDispatch),                .align = alignof(CtsCmdDispatch)                };
    lookup[CTS_COMMAND_CMD_DISPATCH_INDIRECT]         = (CtsCommandMetadata) { .handler = handleCmdDispatchIndirect,        .size = sizeof(CtsCmdDispatchIndirect),        .align = alignof(CtsCmdDispatchIndirect)        };
    lookup[CTS_COMMAND_CMD_DRAW]                      = (CtsCommandMetadata) { .handler = handleCmdDraw,                    .size = sizeof(CtsCmdDraw),                    .align = alignof(CtsCmdDraw)                    };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED]              = (CtsCommandMetadata) { .handler = handleCmdDrawIndexed,             .size = sizeof(CtsCmdDrawIndexed),             .align = alignof(CtsCmdDrawIndexed)             };
    lookup[CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT]     = (CtsCommandMetadata) { .handler = handleCmdDrawIndexedIndirect,     .size = sizeof(CtsCmdDrawIndexedIndirect),     .align = alignof(CtsCmdDrawIndexedIndirect)     };
    lookup[CTS_COMMAND_CMD_DRAW_INDIRECT]             = (CtsCommandMetadata) { .handler = handleCmdDrawIndirect,            .size = sizeof(CtsCmdDrawIndirect),            .align = alignof(CtsCmdDrawIndirect)            };
    lookup[CTS_COMMAND_CMD_END_QUERY]                 = (CtsCommandMetadata) { .handler = handleCmdEndQuery,                .size = sizeof(CtsCmdEndQuery),                .align = alignof(CtsCmdEndQuery)                };
    lookup[CTS_COMMAND_CMD_END_RENDER_PASS]           = (CtsCommandMetadata) { .handler = handleCmdEndRenderPass,           .size = sizeof(CtsCmdEndRenderPass),           .align = alignof(CtsCmdEndRenderPass)           };
    lookup[CTS_COMMAND_CMD_EXECUTE_COMMANDS]          = (CtsCommandMetadata) { .handler = handleCmdExecuteCommands,         .size = sizeof(CtsCmdExecuteCommands),         .align = alignof(CtsCmdExecuteCommands)         };
    lookup[CTS_COMMAND_CMD_FILL_BUFFER]               = (CtsCommandMetadata) { .handler = handleCmdFillBuffer,              .size = sizeof(CtsCmdFillBuffer),              .align = alignof(CtsCmdFillBuffer)              };
    lookup[CTS_COMMAND_CMD_NEXT_SUBPASS]              = (CtsCommandMetadata) { .handler = handleCmdNextSubpass,             .size = sizeof(CtsCmdNextSubpass),             .align = alignof(CtsCmdNextSubpass)             };
    lookup[CTS_COMMAND_CMD_PIPELINE_BARRIER]          = (CtsCommandMetadata) { .handler = handleCmdPipelineBarrier,         .size = sizeof(CtsCmdPipelineBarrier),         .align = alignof(CtsCmdPipelineBarrier)         };
    lookup[CTS_COMMAND_CMD_PUSH_CONSTANTS]            = (CtsCommandMetadata) { .handler = handleCmdPushConstants,           .size = sizeof(CtsCmdPushConstants),           .align = alignof(CtsCmdPushConstants)           };
    lookup[CTS_COMMAND_CMD_RESET_EVENT]               = (CtsCommandMetadata) { .handler = handleCmdResetEvent,              .size = sizeof(CtsCmdResetEvent),              .align = alignof(CtsCmdResetEvent)              };
    lookup[CTS_COMMAND_CMD_RESET_QUERY_POOL]          = (CtsCommandMetadata) { .handler = handleCmdResetQueryPool,          .size = sizeof(CtsCmdResetQueryPool),          .align = alignof(CtsCmdResetQueryPool)          };
    lookup[CTS_COMMAND_CMD_RESOLVE_IMAGE]             = (CtsCommandMetadata) { .handler = handleCmdResolveImage,            .size = sizeof(CtsCmdResolveImage),            .align = alignof(CtsCmdResolveImage)            };
    lookup[CTS_COMMAND_CMD_SET_BLEND_CONSTANTS]       = (CtsCommandMetadata) { .handler = handleCmdSetBlendConstants,       .size = sizeof(CtsCmdSetBlendConstants),       .align = alignof(CtsCmdSetBlendConstants)       };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BIAS]            = (CtsCommandMetadata) { .handler = handleCmdSetDepthBias,            .size = sizeof(CtsCmdSetDepthBias),            .align = alignof(CtsCmdSetDepthBias)            };
    lookup[CTS_COMMAND_CMD_SET_DEPTH_BOUNDS]          = (CtsCommandMetadata) { .handler = handleCmdSetDepthBounds,          .size = sizeof(CtsCmdSetDepthBounds),          .align = alignof(CtsCmdSetDepthBounds)          };
    lookup[CTS_COMMAND_CMD_SET_DEVICE_MASK]           = (CtsCommandMetadata) { .handler = handleCmdSetDeviceMask,           .size = sizeof(CtsCmdSetDeviceMask),           .align = alignof(CtsCmdSetDeviceMask)           };
    lookup[CTS_COMMAND_CMD_SET_EVENT]                 = (CtsCommandMetadata) { .handler = handleCmdSetEvent,                .size = sizeof(CtsCmdSetEvent),                .align = alignof(CtsCmdSetEvent)                };
    lookup[CTS_COMMAND_CMD_SET_LINE_WIDTH]            = (CtsCommandMetadata) { .handler = handleCmdSetLineWidth,            .size = sizeof(CtsCmdSetLineWidth),            .align = alignof(CtsCmdSetLineWidth)            };
    lookup[CTS_COMMAND_CMD_SET_SCISSOR]               = (CtsCommandMetadata) { .handler = handleCmdSetScissor,              .size = sizeof(CtsCmdSetScissor),              .align = alignof(CtsCmdSetScissor)              };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK]  = (CtsCommandMetadata) { .handler = handleCmdSetStencilCompareMask,   .size = sizeof(CtsCmdSetStencilCompareMask),   .align = alignof(CtsCmdSetStencilCompareMask)   };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_REFERENCE]     = (CtsCommandMetadata) { .handler = handleCmdSetStencilReference,     .size = sizeof(CtsCmdSetStencilReference),     .align = alignof(CtsCmdSetStencilReference)     };
    lookup[CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK]    = (CtsCommandMetadata) { .handler = handleCmdSetStencilWriteMask,     .size = sizeof(CtsCmdSetStencilWriteMask),     .align = alignof(CtsCmdSetStencilWriteMask)     };
    lookup[CTS_COMMAND_CMD_SET_VIEWPORT]              = (CtsCommandMetadata) { .handler = handleCmdSetViewport,             .size = sizeof(CtsCmdSetViewport),             .align = alignof(CtsCmdSetViewport)             };
    lookup[CTS_COMMAND_CMD_UPDATE_BUFFER]             = (CtsCommandMetadata) { .handler = handleCmdUpdateBuffer,            .size = sizeof(CtsCmdUpdateBuffer),            .align = alignof(CtsCmdUpdateBuffer)            };
    lookup[CTS_COMMAND_CMD_WAIT_EVENTS]               = (CtsCommandMetadata) { .handler = handleCmdWaitEvents,              .size = sizeof(CtsCmdWaitEvents),              .align = alignof(CtsCmdWaitEvents)              };
    lookup[CTS_COMMAND_CMD_WRITE_TIMESTAMP]           = (CtsCommandMetadata) { .handler = handleCmdWriteTimestamp,          .size = sizeof(CtsCmdWriteTimestamp),          .align = alignof(CtsCmdWriteTimestamp)          };

    return lookup;
}

static void handleAllocateMemory(const CtsCmdBase* pCmd) {
    const CtsAllocateMemory* cmd = (const CtsAllocateMemory*) pCmd;
    *cmd->result = ctsAllocateMemoryImpl(cmd->device, cmd->allocateInfo, cmd->allocator, cmd->memory);
}

static void handleMapMemory(const CtsCmdBase* pCmd) {
    const CtsMapMemory* cmd = (const CtsMapMemory*) pCmd;
    *cmd->result = ctsMapMemoryImpl(cmd->device, cmd->memory, cmd->offset, cmd->size, cmd->flags, cmd->data);
}

static void handleUnmapMemory(const CtsCmdBase* pCmd) {
    const CtsUnmapMemory* cmd = (const CtsUnmapMemory*) pCmd;
    ctsUnmapMemoryImpl(cmd->device, cmd->memory);
}

static void handleFreeMemory(const CtsCmdBase* pCmd) {
    const CtsFreeMemory* cmd = (const CtsFreeMemory*) pCmd;
    ctsFreeMemoryImpl(cmd->device, cmd->memory, cmd->allocator);
}

static void handleQueueSubmit(const CtsCmdBase* pCmd) {
    const CtsQueueSubmit* cmd = (const CtsQueueSubmit*) pCmd;
    *cmd->result = ctsQueueSubmitImpl(cmd->queue, cmd->submitCount, cmd->submits, cmd->fence);
}

static void handleSignalSemaphores(const CtsCmdBase* pCmd) {
    const CtsSignalSemaphores* cmd = (const CtsSignalSemaphores*) pCmd;
    ctsSignalSemaphores(cmd->semaphoreCount, cmd->semaphores);
}

static void handleAllocateCommandBuffers(const CtsCmdBase* pCmd) {
    const CtsAllocateCommandBuffers* cmd = (const CtsAllocateCommandBuffers*) pCmd;
    *cmd->result = ctsAllocateCommandBuffersImpl(cmd->device, cmd->allocateInfo, cmd->commandBuffers);
}

static void handleFreeCommandBuffers(const CtsCmdBase* pCmd) {
    const CtsFreeCommandBuffers* cmd = (const CtsFreeCommandBuffers*) pCmd;
    ctsFreeCommandBuffersImpl(cmd->device, cmd->commandPool, cmd->commandBufferCount, cmd->commandBuffers);
}

static void handleAllocateDescriptorSets(const CtsCmdBase* pCmd) {
    const CtsAllocateDescriptorSets* cmd = (const CtsAllocateDescriptorSets*) pCmd;
    *cmd->result = ctsAllocateDescriptorSetsImpl(cmd->device, cmd->allocateInfo, cmd->descriptorSets);
}

static void handleUpdateDescriptorSets(const CtsCmdBase* pCmd) {
    const CtsUpdateDescriptorSets* cmd = (const CtsUpdateDescriptorSets*) pCmd;
    ctsUpdateDescriptorSetsImpl(cmd->device, cmd->descriptorWriteCount, cmd->descriptorWrites, cmd->descriptorCopyCount, cmd->descriptorCopies);
}

static void handleFreeDescriptorSets(const CtsCmdBase* pCmd) {
    const CtsFreeDescriptorSets* cmd = (const CtsFreeDescriptorSets*) pCmd;
    ctsFreeDescriptorSetsImpl(cmd->device,  cmd->descriptorPool, cmd->descriptorSetCount, cmd->descriptorSets);
}

static void handleCreateBuffer(const CtsCmdBase* pCmd) {
    const CtsCreateBuffer* cmd = (const CtsCreateBuffer*) pCmd;
    *cmd->result = ctsCreateBufferImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->buffer);
}

static void handleBindBufferMemory(const CtsCmdBase* pCmd) {
    const CtsBindBufferMemory* cmd = (const CtsBindBufferMemory*) pCmd;
    *cmd->result = ctsBindBufferMemoryImpl(cmd->device, cmd->buffer, cmd->memory, cmd->offset);
}

static void handleDestroyBuffer(const CtsCmdBase* pCmd) {
    const CtsDestroyBuffer* cmd = (const CtsDestroyBuffer*) pCmd;
    ctsDestroyBufferImpl(cmd->device, cmd->buffer, cmd->allocator);
}

static void handleCreateCommandPool(const CtsCmdBase* pCmd) {
    const CtsCreateCommandPool* cmd = (const CtsCreateCommandPool*) pCmd;
    *cmd->result = ctsCreateCommandPoolImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->commandPool);
}

static void handleDestroyCommandPool(const CtsCmdBase* pCmd) {
    const CtsDestroyCommandPool* cmd = (const CtsDestroyCommandPool*) pCmd;
    ctsDestroyCommandPoolImpl(cmd->device, cmd->commandPool, cmd->allocator);
}

static void handleCreateDescriptorPool(const CtsCmdBase* pCmd) {
    const CtsCreateDescriptorPool* cmd = (const CtsCreateDescriptorPool*) pCmd;
    *cmd->result = ctsCreateDescriptorPoolImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->descriptorPool);
}

static void handleDestroyDescriptorPool(const CtsCmdBase* pCmd) {
    const CtsDestroyDescriptorPool* cmd = (const CtsDestroyDescriptorPool*) pCmd;
    ctsDestroyDescriptorPoolImpl(cmd->device, cmd->descriptorPool, cmd->allocator);
}

static void handleCreateDescriptorSetLayout(const CtsCmdBase* pCmd) {
    const CtsCreateDescriptorSetLayout* cmd = (const CtsCreateDescriptorSetLayout*) pCmd;
    *cmd->result = ctsCreateDescriptorSetLayoutImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->descriptorSetLayout);
}

static void handleDestroyDescriptorSetLayout(const CtsCmdBase* pCmd) {
    const CtsDestroyDescriptorSetLayout* cmd = (const CtsDestroyDescriptorSetLayout*) pCmd;
    ctsDestroyDescriptorSetLayoutImpl(cmd->device, cmd->descriptorSetLayout, cmd->allocator);
}

static void handleCreateGraphicsPipelines(const CtsCmdBase* pCmd) {
    const CtsCreateGraphicsPipelines* cmd = (const CtsCreateGraphicsPipelines*) pCmd;
    *cmd->result = ctsCreateGraphicsPipelinesImpl(cmd->device, cmd->pipelineCache, cmd->createInfoCount,  cmd->createInfos, cmd->allocator, cmd->pipelines);
}

static void handleDestroyPipeline(const CtsCmdBase* pCmd) {
    const CtsDestroyPipeline* cmd = (const CtsDestroyPipeline*) pCmd;
    ctsDestroyPipelineImpl(cmd->device, cmd->pipeline, cmd->allocator);
}

static void handleCreateImageView(const CtsCmdBase* pCmd) {
    const CtsCreateImageView* cmd = (const CtsCreateImageView*) pCmd;
    *cmd->result = ctsCreateImageViewImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->imageView);
}

static void handleDestroyImageView(const CtsCmdBase* pCmd) {
    const CtsDestroyImageView* cmd = (const CtsDestroyImageView*) pCmd;
    ctsDestroyImageViewImpl(cmd->device, cmd->imageView, cmd->allocator);
}

static void handleCreateImage(const CtsCmdBase* pCmd) {
    const CtsCreateImage* cmd = (const CtsCreateImage*) pCmd;
    *cmd->result = ctsCreateImageImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->image);
}

static void handleDestroyImage(const CtsCmdBase* pCmd) {
    const CtsDestroyImage* cmd = (const CtsDestroyImage*) pCmd;
    ctsDestroyImageImpl(cmd->device, cmd->image, cmd->allocator);
}

static void handleCreatePipelineLayout(const CtsCmdBase* pCmd) {
    const CtsCreatePipelineLayout* cmd = (const CtsCreatePipelineLayout*) pCmd;
    *cmd->result = ctsCreatePipelineLayoutImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->pipelineLayout);
}

static void handleDestroyPipelineLayout(const CtsCmdBase* pCmd) {
    const CtsDestroyPipelineLayout* cmd = (const CtsDestroyPipelineLayout*) pCmd;
    ctsDestroyPipelineLayoutImpl(cmd->device, cmd->pipelineLayout, cmd->allocator);
}

static void handleCreateShaderModule(const CtsCmdBase* pCmd) {
    const CtsCreateShaderModule* cmd = (const CtsCreateShaderModule*) pCmd;
    *cmd->result = ctsCreateShaderModuleImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->shaderModule);
}

static void handleDestroyShaderModule(const CtsCmdBase* pCmd) {
    const CtsDestroyShaderModule* cmd = (const CtsDestroyShaderModule*) pCmd;
    ctsDestroyShaderModuleImpl(cmd->device, cmd->shaderModule, cmd->allocator);
}

static void handleCreateFence(const CtsCmdBase* pCmd) {
    const CtsCreateFence* cmd = (const CtsCreateFence*) pCmd;
    ctsCreateFenceImpl(cmd->device, cmd->createInfo, cmd->allocator, cmd->fence);
}

static void handleDestroyFence(const CtsCmdBase* pCmd) {
    const CtsDestroyFence* cmd = (const CtsDestroyFence*) pCmd;
    ctsDestroyFenceImpl(cmd->device, cmd->fence, cmd->allocator);
}

static void handleCmdBeginQuery(const CtsCmdBase* pCmd) {
    const CtsCmdBeginQuery* cmd = (const CtsCmdBeginQuery*) pCmd;
    ctsCmdBeginQueryImpl(cmd->commandBuffer, cmd->queryPool, cmd->query, cmd->flags);
}

static void handleCmdBeginRenderPass(const CtsCmdBase* pCmd) {
    const CtsCmdBeginRenderPass* cmd = (const CtsCmdBeginRenderPass*) pCmd;
    ctsCmdBeginRenderPassImpl(cmd->commandBuffer, cmd->renderPassBegin, cmd->contents);
}

static void handleCmdBindDescriptorSets(const CtsCmdBase* pCmd) {
    const CtsCmdBindDescriptorSets* cmd = (const CtsCmdBindDescriptorSets*) pCmd;
    ctsCmdBindDescriptorSetsImpl(cmd->commandBuffer, cmd->pipelineBindPoint, cmd->pipelineLayout, cmd->firstSet, cmd->descriptorSetCount, cmd->descriptorSets, cmd->dynamicOffsetCount, cmd->dynamicOffsets);
}

static void handleCmdBindIndexBuffer(const CtsCmdBase* pCmd) {
    const CtsCmdBindIndexBuffer* cmd = (const CtsCmdBindIndexBuffer*) pCmd;
    ctsCmdBindIndexBufferImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->indexType);
}

static void handleCmdBindPipeline(const CtsCmdBase* pCmd) {
    const CtsCmdBindPipeline* cmd = (const CtsCmdBindPipeline*) pCmd;
    ctsCmdBindPipelineImpl(cmd->commandBuffer, cmd->pipelineBindPoint, cmd->pipeline);
}

static void handleCmdBindVertexBuffers(const CtsCmdBase* pCmd) {
    const CtsCmdBindVertexBuffers* cmd = (const CtsCmdBindVertexBuffers*) pCmd;
    ctsCmdBindVertexBuffersImpl(cmd->commandBuffer, cmd->firstBinding, cmd->bindingCount, cmd->buffers, cmd->offsets);
}

static void handleCmdBlitImage(const CtsCmdBase* pCmd) {
    const CtsCmdBlitImage* cmd = (const CtsCmdBlitImage*) pCmd;
    ctsCmdBlitImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->regions, cmd->filter);
}

static void handleCmdClearAttachments(const CtsCmdBase* pCmd) {
    const CtsCmdClearAttachments* cmd = (const CtsCmdClearAttachments*) pCmd;
    ctsCmdClearAttachmentsImpl(cmd->commandBuffer, cmd->attachmentCount, cmd->attachments, cmd->rectCount, cmd->rects);
}

static void handleCmdClearColorImage(const CtsCmdBase* pCmd) {
    const CtsCmdClearColorImage* cmd = (const CtsCmdClearColorImage*) pCmd;
    ctsCmdClearColorImageImpl(cmd->commandBuffer, cmd->image, cmd->imageLayout, cmd->color, cmd->rangeCount, cmd->ranges);
}

static void handleCmdClearDepthStencilImage(const CtsCmdBase* pCmd) {
    const CtsCmdClearDepthStencilImage* cmd = (const CtsCmdClearDepthStencilImage*) pCmd;
    ctsCmdClearDepthStencilImageImpl(cmd->commandBuffer, cmd->image, cmd->imageLayout, cmd->depthStencil, cmd->rangeCount, cmd->ranges);
}

static void handleCmdCopyBuffer(const CtsCmdBase* pCmd) {
    const CtsCmdCopyBuffer* cmd = (const CtsCmdCopyBuffer*) pCmd;
    ctsCmdCopyBufferImpl(cmd->commandBuffer, cmd->srcBuffer, cmd->dstBuffer, cmd->regionCount, cmd->regions);
}

static void handleCmdCopyBufferToImage(const CtsCmdBase* pCmd) {
    const CtsCmdCopyBufferToImage* cmd = (const CtsCmdCopyBufferToImage*) pCmd;
    ctsCmdCopyBufferToImageImpl(cmd->commandBuffer, cmd->srcBuffer, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->regions);
}

static void handleCmdCopyImage(const CtsCmdBase* pCmd) {
    const CtsCmdCopyImage* cmd = (const CtsCmdCopyImage*) pCmd;
    ctsCmdCopyImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->regions);
}

static void handleCmdCopyImageToBuffer(const CtsCmdBase* pCmd) {
    const CtsCmdCopyImageToBuffer* cmd = (const CtsCmdCopyImageToBuffer*) pCmd;
    ctsCmdCopyImageToBufferImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstBuffer, cmd->regionCount, cmd->regions);
}

static void handleCmdCopyQueryPoolResults(const CtsCmdBase* pCmd) {
    const CtsCmdCopyQueryPoolResults* cmd = (const CtsCmdCopyQueryPoolResults*) pCmd;
    ctsCmdCopyQueryPoolResultsImpl(cmd->commandBuffer, cmd->queryPool, cmd->firstQuery, cmd->queryCount, cmd->dstBuffer, cmd->dstOffset, cmd->stride, cmd->flags);
}

static void handleCmdDispatch(const CtsCmdBase* pCmd) {
    const CtsCmdDispatch* cmd = (const CtsCmdDispatch*) pCmd;
    ctsCmdDispatchImpl(cmd->commandBuffer, cmd->groupCountX, cmd->groupCountY, cmd->groupCountZ);
}

static void handleCmdDispatchIndirect(const CtsCmdBase* pCmd) {
    const CtsCmdDispatchIndirect* cmd = (const CtsCmdDispatchIndirect*) pCmd;
    ctsCmdDispatchIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset);
}

static void handleCmdDraw(const CtsCmdBase* pCmd) {
    const CtsCmdDraw* cmd = (const CtsCmdDraw*) pCmd;
    ctsCmdDrawImpl(cmd->commandBuffer, cmd->vertexCount, cmd->instanceCount, cmd->firstVertex, cmd->firstInstance);
}

static void handleCmdDrawIndexed(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndexed* cmd = (const CtsCmdDrawIndexed*) pCmd;
    ctsCmdDrawIndexedImpl(cmd->commandBuffer, cmd->indexCount, cmd->instanceCount, cmd->firstIndex, cmd->vertexOffset, cmd->firstInstance);
}

static void handleCmdDrawIndexedIndirect(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndexedIndirect* cmd = (const CtsCmdDrawIndexedIndirect*) pCmd;
    ctsCmdDrawIndexedIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->drawCount, cmd->stride);
}

static void handleCmdDrawIndirect(const CtsCmdBase* pCmd) {
    const CtsCmdDrawIndirect* cmd = (const CtsCmdDrawIndirect*) pCmd;
    ctsCmdDrawIndirectImpl(cmd->commandBuffer, cmd->buffer, cmd->offset, cmd->drawCount, cmd->stride);
}

static void handleCmdEndQuery(const CtsCmdBase* pCmd) {
    const CtsCmdEndQuery* cmd = (const CtsCmdEndQuery*) pCmd;
    ctsCmdEndQueryImpl(cmd->commandBuffer, cmd->queryPool, cmd->query);
}

static void handleCmdEndRenderPass(const CtsCmdBase* pCmd) {
    const CtsCmdEndRenderPass* cmd = (const CtsCmdEndRenderPass*) pCmd;
    ctsCmdEndRenderPassImpl(cmd->commandBuffer);
}

static void handleCmdExecuteCommands(const CtsCmdBase* pCmd) {
    const CtsCmdExecuteCommands* cmd = (const CtsCmdExecuteCommands*) pCmd;
    ctsCmdExecuteCommandsImpl(cmd->commandBuffer, cmd->commandBufferCount, cmd->commandBuffers);
}

static void handleCmdFillBuffer(const CtsCmdBase* pCmd) {
    const CtsCmdFillBuffer* cmd = (const CtsCmdFillBuffer*) pCmd;
    ctsCmdFillBufferImpl(cmd->commandBuffer, cmd->dstBuffer, cmd->dstOffset, cmd->size, cmd->data);
}

static void handleCmdNextSubpass(const CtsCmdBase* pCmd) {
    const CtsCmdNextSubpass* cmd = (const CtsCmdNextSubpass*) pCmd;
    ctsCmdNextSubpassImpl(cmd->commandBuffer, cmd->contents);
}

static void handleCmdPipelineBarrier(const CtsCmdBase* pCmd) {
    const CtsCmdPipelineBarrier* cmd = (const CtsCmdPipelineBarrier*) pCmd;
    ctsCmdPipelineBarrierImpl(cmd->commandBuffer, cmd->srcStageMask, cmd->dstStageMask, cmd->dependencyFlags, cmd->memoryBarrierCount, cmd->memoryBarriers, cmd->bufferMemoryBarrierCount, cmd->bufferMemoryBarriers, cmd->imageMemoryBarrierCount, cmd->imageMemoryBarriers);
}

static void handleCmdPushConstants(const CtsCmdBase* pCmd) {
    const CtsCmdPushConstants* cmd = (const CtsCmdPushConstants*) pCmd;
    ctsCmdPushConstantsImpl(cmd->commandBuffer, cmd->layout, cmd->stageFlags, cmd->offset, cmd->size, cmd->values);
}

static void handleCmdResetEvent(const CtsCmdBase* pCmd) {
    const CtsCmdResetEvent* cmd = (const CtsCmdResetEvent*) pCmd;
    ctsCmdResetEventImpl(cmd->commandBuffer, cmd->event, cmd->stageMask);
}

static void handleCmdResetQueryPool(const CtsCmdBase* pCmd) {
    const CtsCmdResetQueryPool* cmd = (const CtsCmdResetQueryPool*) pCmd;
    ctsCmdResetQueryPoolImpl(cmd->commandBuffer, cmd->queryPool, cmd->firstQuery, cmd->queryCount);
}

static void handleCmdResolveImage(const CtsCmdBase* pCmd) {
    const CtsCmdResolveImage* cmd = (const CtsCmdResolveImage*) pCmd;
    ctsCmdResolveImageImpl(cmd->commandBuffer, cmd->srcImage, cmd->srcImageLayout, cmd->dstImage, cmd->dstImageLayout, cmd->regionCount, cmd->regions);
}

static void handleCmdSetBlendConstants(const CtsCmdBase* pCmd) {
    const CtsCmdSetBlendConstants* cmd = (const CtsCmdSetBlendConstants*) pCmd;
    ctsCmdSetBlendConstantsImpl(cmd->commandBuffer, cmd->blendConstants);
}

static void handleCmdSetDepthBias(const CtsCmdBase* pCmd) {
    const CtsCmdSetDepthBias* cmd = (const CtsCmdSetDepthBias*) pCmd;
    ctsCmdSetDepthBiasImpl(cmd->commandBuffer, cmd->depthBiasConstantFactor, cmd->depthBiasClamp, cmd->depthBiasSlopeFactor);
}

static void handleCmdSetDepthBounds(const CtsCmdBase* pCmd) {
    const CtsCmdSetDepthBounds* cmd = (const CtsCmdSetDepthBounds*) pCmd;
    ctsCmdSetDepthBoundsImpl(cmd->commandBuffer, cmd->minDepthBounds, cmd->maxDepthBounds);
}

static void handleCmdSetDeviceMask(const CtsCmdBase* pCmd) {
    const CtsCmdSetDeviceMask* cmd = (const CtsCmdSetDeviceMask*) pCmd;
    ctsCmdSetDeviceMaskImpl(cmd->commandBuffer, cmd->deviceMask);
}

static void handleCmdSetEvent(const CtsCmdBase* pCmd) {
    const CtsCmdSetEvent* cmd = (const CtsCmdSetEvent*) pCmd;
    ctsCmdSetEventImpl(cmd->commandBuffer, cmd->event, cmd->stageMask);
}

static void handleCmdSetLineWidth(const CtsCmdBase* pCmd) {
    const CtsCmdSetLineWidth* cmd = (const CtsCmdSetLineWidth*) pCmd;
    ctsCmdSetLineWidthImpl(cmd->commandBuffer, cmd->lineWidth);
}

static void handleCmdSetScissor(const CtsCmdBase* pCmd) {
    const CtsCmdSetScissor* cmd = (const CtsCmdSetScissor*) pCmd;
    ctsCmdSetScissorImpl(cmd->commandBuffer, cmd->firstScissor, cmd->scissorCount, cmd->scissors);
}

static void handleCmdSetStencilCompareMask(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilCompareMask* cmd = (const CtsCmdSetStencilCompareMask*) pCmd;
    ctsCmdSetStencilCompareMaskImpl(cmd->commandBuffer, cmd->faceMask, cmd->compareMask);
}

static void handleCmdSetStencilReference(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilReference* cmd = (const CtsCmdSetStencilReference*) pCmd;
    ctsCmdSetStencilReferenceImpl(cmd->commandBuffer, cmd->faceMask, cmd->reference);
}

static void handleCmdSetStencilWriteMask(const CtsCmdBase* pCmd) {
    const CtsCmdSetStencilWriteMask* cmd = (const CtsCmdSetStencilWriteMask*) pCmd;
    ctsCmdSetStencilWriteMaskImpl(cmd->commandBuffer, cmd->faceMask, cmd->writeMask);
}

static void handleCmdSetViewport(const CtsCmdBase* pCmd) {
    const CtsCmdSetViewport* cmd = (const CtsCmdSetViewport*) pCmd;
    ctsCmdSetViewportImpl(cmd->commandBuffer, cmd->firstViewport, cmd->viewportCount, cmd->viewports);
}

static void handleCmdUpdateBuffer(const CtsCmdBase* pCmd) {
    const CtsCmdUpdateBuffer* cmd = (const CtsCmdUpdateBuffer*) pCmd;
    ctsCmdUpdateBufferImpl(cmd->commandBuffer, cmd->dstBuffer, cmd->dstOffset, cmd->dataSize, cmd->data);
}

static void handleCmdWaitEvents(const CtsCmdBase* pCmd) {
    const CtsCmdWaitEvents* cmd = (const CtsCmdWaitEvents*) pCmd;
    ctsCmdWaitEventsImpl(cmd->commandBuffer, cmd->eventCount, cmd->events, cmd->srcStageMask, cmd->dstStageMask, cmd->memoryBarrierCount, cmd->memoryBarriers, cmd->bufferMemoryBarrierCount, cmd->bufferMemoryBarriers, cmd->imageMemoryBarrierCount, cmd->imageMemoryBarriers);
}

static void handleCmdWriteTimestamp(const CtsCmdBase* pCmd) {
    const CtsCmdWriteTimestamp* cmd = (const CtsCmdWriteTimestamp*) pCmd;
    ctsCmdWriteTimestampImpl(cmd->commandBuffer, cmd->pipelineStage, cmd->queryPool, cmd->query);
}

#pragma endregion
