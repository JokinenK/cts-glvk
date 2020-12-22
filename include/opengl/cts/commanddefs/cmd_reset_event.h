#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetEvent {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsEvent event;
    CtsPipelineStageFlags stageMask;
} CtsCmdResetEvent;

#ifdef __cplusplus
}
#endif