#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueSubmit {
    CtsCmdBase base;
    CtsQueue queue;
    uint32_t submitCount;
    const CtsSubmitInfo* pSubmits;
    CtsFence fence;
    CtsResult* pResult;
} CtsQueueSubmit;

#ifdef __cplusplus
}
#endif