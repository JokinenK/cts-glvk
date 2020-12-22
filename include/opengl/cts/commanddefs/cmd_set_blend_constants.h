#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetBlendConstants {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    float blendConstants[4];
} CtsCmdSetBlendConstants;

#ifdef __cplusplus
}
#endif