#pragma once

#include <cts/commanddefs/cmd_base.h>
#include <cts/commanddefs/command_types.h>
#include <cts/trampoline.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCommandMetadata {
    CtsCmdTrampolineFn handler;
    size_t size;
    size_t align;
} CtsCommandMetadata;

const CtsCommandMetadata* ctsGetCommandMetadata(CtsCommandType commandType);
const size_t ctsGetMaxCommandSize();
const size_t ctsGetMaxCommandAlign();

#ifdef __cplusplus
}
#endif
