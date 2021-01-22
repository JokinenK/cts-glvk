#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsDependencyFlags;
typedef enum CtsDependencyFlagBits {
    CTS_DEPENDENCY_BY_REGION_BIT = 0x00000001
} CtsDependencyFlagBits;

#ifdef __cplusplus
}
#endif

