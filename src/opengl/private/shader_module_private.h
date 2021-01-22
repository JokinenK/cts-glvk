#pragma once

#include <stdint.h>
#include <cts/typedefs/shader_module.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsShaderModuleImpl {
    size_t codeSize;
    char* code;
};

#ifdef __cplusplus
}
#endif