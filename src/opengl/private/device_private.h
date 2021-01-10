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

struct CtsDevice {
    bool isRunning;
    CtsThread thread;
    CtsQueue queue;

    CtsSemaphore initSemaphore;
    CtsSemaphore dispatchSemaphore;

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