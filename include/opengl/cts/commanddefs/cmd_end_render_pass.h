#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdEndRenderPass {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
} CtsCmdEndRenderPass;

#ifdef __cplusplus
}
#endif