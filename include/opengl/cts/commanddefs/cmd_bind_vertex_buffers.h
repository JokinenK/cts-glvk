#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindVertexBuffers {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t firstBinding;
    uint32_t bindingCount;
    const CtsBuffer* buffers;
    const CtsDeviceSize* offsets;
} CtsCmdBindVertexBuffers;

#ifdef __cplusplus
}
#endif