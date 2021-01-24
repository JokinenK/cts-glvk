#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsCullModeFlags;
typedef enum CtsCullModeFlagBits {
    CTS_CULL_MODE_NONE              = 0,
    CTS_CULL_MODE_FRONT_BIT         = 0x00000001,
    CTS_CULL_MODE_BACK_BIT          = 0x00000002,
    CTS_CULL_MODE_FRONT_AND_BACK    = 0x00000003,
} CtsCullModeFlagBits;

#ifdef __cplusplus
}
#endif