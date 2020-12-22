#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetScissor {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t firstScissor;
    uint32_t scissorCount;
    const CtsRect2D* scissors;
} CtsCmdSetScissor;

#ifdef __cplusplus
}
#endif