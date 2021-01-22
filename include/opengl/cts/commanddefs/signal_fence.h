#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSignalFence {
    CtsCmdBase base;
    CtsDevice device;
    CtsFence fence;
} CtsSignalFence;

#ifdef __cplusplus
}
#endif