#pragma once

#include <stdint.h>
#include <cts/typedefs/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union CtsClearRect {
    CtsRect2D rect;
    uint32_t baseArrayLayer;
    uint32_t layerCount;
} CtsClearRect;

#ifdef __cplusplus
}
#endif