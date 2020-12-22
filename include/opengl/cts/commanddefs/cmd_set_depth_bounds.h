#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDepthBounds {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    float minDepthBounds;
    float maxDepthBounds;
} CtsCmdSetDepthBounds;

#ifdef __cplusplus
}
#endif