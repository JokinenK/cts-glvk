#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsShaderModule* CtsShaderModule;
typedef struct CtsShaderModuleCreateInfo {
    CtsStructureType sType;
    const void* next;
    size_t codeSize;
    const uint32_t* code;
} CtsShaderModuleCreateInfo;

#ifdef __cplusplus
}
#endif