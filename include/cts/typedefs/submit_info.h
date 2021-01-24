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
    CtsStructureType                sType;
    const void*                     pNext;
    uint32_t                        waitSemaphoreCount;
    const CtsSemaphore*             pWaitSemaphores;
    const CtsPipelineStageFlags*    pWaitDstStageMask;
    uint32_t                        commandBufferCount;
    const CtsCommandBuffer*         pCommandBuffers;
    uint32_t                        signalSemaphoreCount;
    const CtsSemaphore*             pSignalSemaphores;
} CtsSubmitInfo;

#ifdef __cplusplus
}
#endif

