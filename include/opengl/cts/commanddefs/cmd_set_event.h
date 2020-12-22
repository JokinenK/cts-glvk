#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetEvent {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsEvent event;
    CtsPipelineStageFlags stageMask;
} CtsCmdSetEvent;

#ifdef __cplusplus
}
#endif