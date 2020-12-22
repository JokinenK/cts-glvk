#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetViewport {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t firstViewport;
    uint32_t viewportCount;
    const CtsViewport* viewports;
} CtsCmdSetViewport;

#ifdef __cplusplus
}
#endif