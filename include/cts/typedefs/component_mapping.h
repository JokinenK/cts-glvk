#pragma once

#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsComponentMapping {
    CtsComponentSwizzle r;
    CtsComponentSwizzle g;
    CtsComponentSwizzle b;
    CtsComponentSwizzle a;
} CtsComponentMapping;

#ifdef __cplusplus
}
#endif