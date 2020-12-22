#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/types.h>
#include <cts/typedefs/gl_pipeline.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipeline {
    CtsPipelineBindPoint bindPoint;

    union {
        CtsGlGraphicsPipeline* graphics;
    };
};

#ifdef __cplusplus
}
#endif