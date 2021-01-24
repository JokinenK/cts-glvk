#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsQueryResultFlags;
typedef enum CtsQueryResultFlagBits {
    CTS_QUERY_RESULT_64_BIT                 = 0x00000001,
    CTS_QUERY_RESULT_WAIT_BIT               = 0x00000002,
    CTS_QUERY_RESULT_WITH_AVAILABILITY_BIT  = 0x00000004,
    CTS_QUERY_RESULT_PARTIAL_BIT            = 0x00000008,
} CtsQueryResultFlagBits;

#ifdef __cplusplus
}
#endif

