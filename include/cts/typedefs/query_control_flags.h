#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsQueryControlFlags;
typedef enum CtsQueryControlFlagBits {
    CTS_QUERY_CONTROL_PRECISE_BIT = 0x00000001,
} CtsQueryControlFlagBits;

#ifdef __cplusplus
}
#endif

