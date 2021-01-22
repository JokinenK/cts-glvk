#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsCommandBufferResetFlags;
typedef enum CtsCommandBufferResetFlagBits {
    CTS_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT = 0x00000001
} CtsCommandBufferResetFlagBits;

#ifdef __cplusplus
}
#endif

