#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlRasterizationStateChanges {
    bool polygonModeChanged;
    bool cullModeChanged;
    bool frontFaceChanged;
    bool depthBiasChanged;
    bool lineWidthChanged;
} CtsGlRasterizationStateChanges;

#ifdef __cplusplus
}
#endif