#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlColorBlendStateChanges {
    bool blendEnableChanged;
    bool colorWriteMaskChanged;
    bool blendFactorChanged;
    bool blendOpChanged;
    bool blendConstantsChanged;
} CtsGlColorBlendStateChanges;

#ifdef __cplusplus
}
#endif