#pragma once

#include <cts/commanddefs/cmd_base.h>
#include <cts/commanddefs/command_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*CtsCommandHandler)(const CtsCmdBase* pCmd);
typedef struct CtsCommandMetadata {
    CtsCommandHandler handler;
    size_t size;
    size_t align;
} CtsCommandMetadata;

const CtsCommandMetadata* ctsGetCommandMetadata(CtsCommandType pCommandType);

#ifdef __cplusplus
}
#endif
