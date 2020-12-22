#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/instance_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsInstance* CtsInstance;

typedef struct CtsInstanceCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsInstanceCreateFlags flags;
    //const VkApplicationInfo* applicationInfo;
    uint32_t enabledLayerCount;
    const char* const* enabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* enabledExtensionNames;
} CtsInstanceCreateInfo;

#ifdef __cplusplus
}
#endif