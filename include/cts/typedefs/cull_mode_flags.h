#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsCullModeFlags;
typedef enum CtsCullModeFlagBits {
    CTS_CULL_MODE_NONE = 0,
    CTS_CULL_MODE_FRONT_BIT = 1,
    CTS_CULL_MODE_BACK_BIT = 2,
    CTS_CULL_MODE_FRONT_AND_BACK = 3,

    NUM_CTS_CULL_MODES
} CtsCullModeFlagBits;

#ifdef __cplusplus
}
#endif