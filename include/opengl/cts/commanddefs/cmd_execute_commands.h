#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdExecuteCommands {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t commandBufferCount;
    const CtsCommandBuffer* pCommandBuffers;
} CtsCmdExecuteCommands;

#ifdef __cplusplus
}
#endif