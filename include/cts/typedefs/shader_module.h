#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsShaderModuleImpl* CtsShaderModule;
typedef struct CtsShaderModuleCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    size_t codeSize;
    const uint32_t* pCode;
} CtsShaderModuleCreateInfo;

#ifdef __cplusplus
}
#endif