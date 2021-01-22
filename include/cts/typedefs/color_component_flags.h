#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsColorComponentFlags;
typedef enum CtsColorComponentFlagBits {
    CTS_COLOR_COMPONENT_R_BIT = 0x00000001,
    CTS_COLOR_COMPONENT_G_BIT = 0x00000002,
    CTS_COLOR_COMPONENT_B_BIT = 0x00000004,
    CTS_COLOR_COMPONENT_A_BIT = 0x00000008,
} CtsColorComponentFlagBits;

#ifdef __cplusplus
}
#endif

