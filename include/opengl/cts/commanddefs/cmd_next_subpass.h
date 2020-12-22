#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdNextSubpass {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsSubpassContents contents;
} CtsCmdNextSubpass;

#ifdef __cplusplus
}
#endif