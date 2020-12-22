#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetLineWidth {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    float lineWidth;
} CtsCmdSetLineWidth;

#ifdef __cplusplus
}
#endif