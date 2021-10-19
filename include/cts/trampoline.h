#pragma once

#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXPAND_ARG_15(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_14(__VA_ARGS__))
#define EXPAND_ARG_14(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_13(__VA_ARGS__))
#define EXPAND_ARG_13(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_12(__VA_ARGS__))
#define EXPAND_ARG_12(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_11(__VA_ARGS__))
#define EXPAND_ARG_11(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_10(__VA_ARGS__))
#define EXPAND_ARG_10(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_9(__VA_ARGS__))
#define EXPAND_ARG_9(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_8(__VA_ARGS__))
#define EXPAND_ARG_8(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_7(__VA_ARGS__))
#define EXPAND_ARG_7(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_6(__VA_ARGS__))
#define EXPAND_ARG_6(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_5(__VA_ARGS__))
#define EXPAND_ARG_5(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_4(__VA_ARGS__))
#define EXPAND_ARG_4(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_3(__VA_ARGS__))
#define EXPAND_ARG_3(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_2(__VA_ARGS__))
#define EXPAND_ARG_2(First, ...) GLUE(cmd->, First) ## , EXPAND(EXPAND_ARG_1(__VA_ARGS__))
#define EXPAND_ARG_1(First, ...) GLUE(cmd->, First)
#define EXPAND_ARG_N(NumArgs) GLUE(EXPAND_ARG_, EXPAND(NumArgs))

#define CTS_TRAMPOLINE_FN(StructName) _##StructName##Trampoline

#define CTS_DEFINE_TRAMPOLINE(StructName, FuncName, ...)                       \
    static void CTS_TRAMPOLINE_FN(StructName)(const struct CtsCmdBase* pCmd) { \
        const struct StructName* cmd = (const struct StructName*) pCmd;        \
        *cmd->pResult = FuncName(                                              \
            EXPAND(EXPAND_ARG_N(NARGS(__VA_ARGS__))(__VA_ARGS__))              \
        );                                                                     \
    }

#define CTS_DEFINE_TRAMPOLINE_VOID(StructName, FuncName, ...)                  \
    static void CTS_TRAMPOLINE_FN(StructName)(const struct CtsCmdBase* pCmd) { \
        const struct StructName* cmd = (const struct StructName*) pCmd;        \
        FuncName(                                                              \
            EXPAND(EXPAND_ARG_N(NARGS(__VA_ARGS__))(__VA_ARGS__))              \
        );                                                                     \
    }

typedef void (*CtsCmdTrampolineFn)(const struct CtsCmdBase*);

#ifdef __cplusplus
}
#endif