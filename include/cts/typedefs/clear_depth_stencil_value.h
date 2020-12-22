#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsClearDepthStencilValue {
    float depth;
    uint32_t stencil;
} CtsClearDepthStencilValue;

#ifdef __cplusplus
}
#endif