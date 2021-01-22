#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsFenceCreateFlags;
typedef enum CtsFenceCreateFlagBits {
    CTS_FENCE_CREATE_SIGNALED_BIT = 0x00000001,
} CtsFenceCreateFlagBits;

#ifdef __cplusplus
}
#endif