#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsSampleCountFlags;
typedef enum CtsSampleCountFlagBits {
    CTS_SAMPLE_COUNT_1_BIT = 0x00000001,
    CTS_SAMPLE_COUNT_2_BIT = 0x00000002,
    CTS_SAMPLE_COUNT_4_BIT = 0x00000004,
    CTS_SAMPLE_COUNT_8_BIT = 0x00000008,
    CTS_SAMPLE_COUNT_16_BIT = 0x00000010,
    CTS_SAMPLE_COUNT_32_BIT = 0x00000020,
    CTS_SAMPLE_COUNT_64_BIT = 0x00000040
} CtsSampleCountFlagBits;

#ifdef __cplusplus
}
#endif