#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsExtent2D {
    uint32_t    width;
    uint32_t    height;
} CtsExtent2D;

typedef struct CtsExtent3D {
    uint32_t    width;
    uint32_t    height;
    uint32_t    depth;
} CtsExtent3D;

#ifdef __cplusplus
}
#endif