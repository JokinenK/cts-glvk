#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsCommandPoolCreateFlags;
typedef enum CtsCommandPoolCreateFlagBits {
    CTS_COMMAND_POOL_CREATE_TRANSIENT_BIT               = 0x00000001,
    CTS_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT    = 0x00000002,
} CtsCommandPoolCreateFlagBits;

#ifdef __cplusplus
}
#endif

