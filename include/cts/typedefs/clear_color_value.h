#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union CtsClearColorValue {
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
} CtsClearColorValue;

#ifdef __cplusplus
}
#endif