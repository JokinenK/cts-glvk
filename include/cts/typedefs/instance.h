#pragma once

#include <stdint.h>
#include <cts/typedefs/application_info.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/instance_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsInstanceImpl* CtsInstance;

typedef struct CtsInstanceCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsInstanceCreateFlags flags;
    const CtsApplicationInfo* pApplicationInfo;
    uint32_t enabledLayerCount;
    const char* const* pEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* pEnabledExtensionNames;
} CtsInstanceCreateInfo;

#ifdef __cplusplus
}
#endif