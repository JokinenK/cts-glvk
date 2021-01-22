#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsQueueWaitIdle(
    CtsQueue queue
);

#ifdef __cplusplus
}
#endif