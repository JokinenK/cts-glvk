#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsViewport {
    float   x;
    float   y;
    float   width;
    float   height;
    float   minDepth;
    float   maxDepth;
} CtsViewport;

#ifdef __cplusplus
}
#endif

