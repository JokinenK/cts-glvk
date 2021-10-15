#pragma once

#include <stdint.h>
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsShaderModule {
    struct CtsObjectBase base;
    size_t codeSize;
    char* code;
};

#ifdef __cplusplus
}
#endif