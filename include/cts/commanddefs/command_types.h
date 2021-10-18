#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CtsCommandType {
    CTS_COMMAND_CREATE_GRAPHICS_PIPELINES,
    CTS_COMMAND_DESTROY_PIPELINE,

    CTS_COMMAND_CREATE_IMAGE_VIEW,
    CTS_COMMAND_DESTROY_IMAGE_VIEW,

    CTS_COMMAND_CREATE_IMAGE,
    CTS_COMMAND_DESTROY_IMAGE,

    CTS_COMMAND_CREATE_SAMPLER,
    CTS_COMMAND_DESTROY_SAMPLER,

    CTS_COMMAND_CREATE_SWAPCHAIN,
    CTS_COMMAND_DESTROY_SWAPCHAIN,

    CTS_COMMAND_CREATE_FRAMEBUFFER,
    CTS_COMMAND_DESTROY_FRAMEBUFFER,

    CTS_COMMAND_CREATE_FENCE,
    CTS_COMMAND_RESET_FENCES,
    CTS_COMMAND_GET_FENCE_STATUS,
    CTS_COMMAND_SIGNAL_FENCE,
    CTS_COMMAND_WAIT_FOR_FENCES,
    CTS_COMMAND_DESTROY_FENCE,

    CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS,
    CTS_COMMAND_UPDATE_DESCRIPTOR_SETS,
    CTS_COMMAND_FREE_DESCRIPTOR_SETS,

    CTS_COMMAND_ALLOCATE_MEMORY,
    CTS_COMMAND_MAP_MEMORY,
    CTS_COMMAND_UNMAP_MEMORY,
    CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES,
    CTS_COMMAND_FREE_MEMORY,

    CTS_COMMAND_QUEUE_PRESENT,

    CTS_COMMAND_CMD_BEGIN_QUERY,
    CTS_COMMAND_CMD_BEGIN_RENDER_PASS,
    CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS,
    CTS_COMMAND_CMD_BIND_INDEX_BUFFER,
    CTS_COMMAND_CMD_BIND_PIPELINE,
    CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS,
    CTS_COMMAND_CMD_BLIT_IMAGE,
    CTS_COMMAND_CMD_CLEAR_ATTACHMENTS,
    CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE,
    CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
    CTS_COMMAND_CMD_COPY_BUFFER,
    CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE,
    CTS_COMMAND_CMD_COPY_IMAGE,
    CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER,
    CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS,
    CTS_COMMAND_CMD_DISPATCH,
    CTS_COMMAND_CMD_DISPATCH_BASE,
    CTS_COMMAND_CMD_DISPATCH_INDIRECT,
    CTS_COMMAND_CMD_DRAW,
    CTS_COMMAND_CMD_DRAW_INDEXED,
    CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT,
    CTS_COMMAND_CMD_DRAW_INDIRECT,
    CTS_COMMAND_CMD_END_QUERY,
    CTS_COMMAND_CMD_END_RENDER_PASS,
    CTS_COMMAND_CMD_EXECUTE_COMMANDS,
    CTS_COMMAND_CMD_FILL_BUFFER,
    CTS_COMMAND_CMD_NEXT_SUBPASS,
    CTS_COMMAND_CMD_PIPELINE_BARRIER,
    CTS_COMMAND_CMD_PUSH_CONSTANTS,
    CTS_COMMAND_CMD_RESET_EVENT,
    CTS_COMMAND_CMD_RESET_QUERY_POOL,
    CTS_COMMAND_CMD_RESOLVE_IMAGE,
    CTS_COMMAND_CMD_SET_BLEND_CONSTANTS,
    CTS_COMMAND_CMD_SET_DEPTH_BIAS,
    CTS_COMMAND_CMD_SET_DEPTH_BOUNDS,
    CTS_COMMAND_CMD_SET_DEVICE_MASK,
    CTS_COMMAND_CMD_SET_EVENT,
    CTS_COMMAND_CMD_SET_LINE_WIDTH,
    CTS_COMMAND_CMD_SET_SCISSOR,
    CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK,
    CTS_COMMAND_CMD_SET_STENCIL_REFERENCE,
    CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK,
    CTS_COMMAND_CMD_SET_VIEWPORT,
    CTS_COMMAND_CMD_UPDATE_BUFFER,
    CTS_COMMAND_CMD_WAIT_EVENTS,
    CTS_COMMAND_CMD_WRITE_TIMESTAMP,

    CTS_COMMAND_CREATE_WIN32_SURFACE_KHR,
    CTS_COMMAND_DESTROY_SURFACE_KHR,

    NUM_CTS_COMMANDS
} CtsCommandType;

#ifdef __cplusplus
}
#endif