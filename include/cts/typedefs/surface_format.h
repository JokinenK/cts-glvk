#pragma once

#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSurfaceFormat {
    CtsFormat format;
    CtsColorSpace colorSpace;
} CtsSurfaceFormat;

#ifdef __cplusplus
}
#endif


