#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/command_buffer.h>
#include <cts/typedefs/pipeline_stage_flags.h>
#include <cts/semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSubmitInfo {
    CtsStructureType sType;
    const void* next;
    uint32_t waitSemaphoreCount;
    const CtsSemaphore* waitSemaphores;
    const CtsPipelineStageFlags* waitDstStageMask;
    uint32_t commandBufferCount;
    const CtsCommandBuffer* commandBuffers;
    uint32_t signalSemaphoreCount;
    const CtsSemaphore* signalSemaphores;
} CtsSubmitInfo;

#ifdef __cplusplus
}
#endif

