#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsCommandBufferUsageFlags;
typedef enum CtsCommandBufferUsageFlagBits {
    CTS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001,
    CTS_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT = 0x00000002,
    CTS_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT = 0x00000004,
} CtsCommandBufferUsageFlagBits;

#ifdef __cplusplus
}
#endif

