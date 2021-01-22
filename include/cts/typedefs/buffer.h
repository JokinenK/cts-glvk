#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>
#include <cts/typedefs/buffer_create_flags.h>
#include <cts/typedefs/buffer_usage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferImpl* CtsBuffer;

typedef struct CtsBufferCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsBufferCreateFlags flags;
    CtsDeviceSize size;
    CtsBufferUsageFlags usage;
    CtsSharingMode sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t* pQueueFamilyIndices;
} CtsBufferCreateInfo;

#ifdef __cplusplus
}
#endif