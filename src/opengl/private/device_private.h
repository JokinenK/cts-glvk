#pragma once

#include <stdbool.h>
#include <cts/typedefs/instance.h>
#include <cts/typedefs/physical_device.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/thread.h>
#include <cts/mutex.h>
#include <cts/condition_variable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsTextureBinding {
    GLenum target;
    GLuint handle;
} CtsTextureBinding;

struct CtsDevice {
    bool isRunning;
    CtsQueue queue;

    CtsSemaphore initSemaphore;
    CtsSemaphore dispatchSemaphore;

    CtsGlGraphicsPipeline* activeGraphicsPipeline;
    CtsFramebuffer activeFramebuffer;
    uint32_t activeSubpass;
    CtsTextureBinding activeTextures[32];

    CtsPhysicalDevice physicalDevice;
    CtsFlags activeDynamicState;
    CtsBuffer activeIndexBuffer;
    CtsGlPipelineVertexInputState* activeVertexInputState;
    CtsGlPipelineInputAssemblyState* activeInputAssemblyState;
    CtsGlPipelineTessellationState* activeTessellationState;
    CtsGlPipelineViewportState* activeViewportState;
    CtsGlPipelineRasterizationState* activeRasterizationState;
    CtsGlPipelineMultisampleState* activeMultisampleState;
    CtsGlPipelineDepthStencilState* activeDepthStencilState;
    CtsGlPipelineColorBlendState* activeColorBlendState;
};

#ifdef __cplusplus
}
#endif