#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlColorBlendStateChanges {
    bool colorWriteMaskChanged;
    bool blendFactorChanged;
    bool blendOpChanged;
} CtsGlColorBlendStateChanges;

#ifdef __cplusplus
}
#endif