#pragma once

#include <cts/typedefs/offset.h>
#include <cts/typedefs/extent.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsRect2D {
    CtsOffset2D offset;
    CtsExtent2D extent;
} CtsRect2D;

#ifdef __cplusplus
}
#endif