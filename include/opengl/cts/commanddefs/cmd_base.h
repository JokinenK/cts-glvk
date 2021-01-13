#pragma once

#include <cts/types.h>
#include <cts/commanddefs/command_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBase {
    CtsCommandType type;
    const struct CtsCmdBase* pNext;
} CtsCmdBase;

#ifdef __cplusplus
}
#endif