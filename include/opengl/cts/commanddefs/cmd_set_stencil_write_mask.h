#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilWriteMask {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsStencilFaceFlags faceMask;
    uint32_t writeMask;
} CtsCmdSetStencilWriteMask;

#ifdef __cplusplus
}
#endif