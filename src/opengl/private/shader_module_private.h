#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsShaderModule {
    size_t codeSize;
    char* code;
};

#ifdef __cplusplus
}
#endif