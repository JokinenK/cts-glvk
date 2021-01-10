#pragma once

#include <cts/types.h>
#include <cts/commanddefs/command_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBase {
    CtsCommandType type;
    const void* next;
} CtsCmdBase;

#ifdef __cplusplus
}
#endif