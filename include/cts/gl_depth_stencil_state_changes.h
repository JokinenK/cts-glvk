#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlDepthStencilStateChanges {
    bool depthWriteEnableChanged;
    bool depthCompareOpChanged;
    bool depthBoundsTestEnableChanged;
    bool depthBoundsChanged;
    bool frontFuncChanged;
    bool frontStencilChanged;
    bool frontFaceWriteMaskChanged;
    bool backFuncChanged;
    bool backStencilChanged;
    bool backFaceWriteMaskChanged;
} CtsGlDepthStencilStateChanges;

#ifdef __cplusplus
}
#endif