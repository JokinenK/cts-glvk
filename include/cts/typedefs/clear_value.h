#pragma once

#include <cts/typedefs/clear_color_value.h>
#include <cts/typedefs/clear_depth_stencil_value.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union CtsClearValue {
    CtsClearColorValue color;
    CtsClearDepthStencilValue depthStencil;
} CtsClearValue;

#ifdef __cplusplus
}
#endif