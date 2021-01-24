#pragma once

#include <stdint.h>
#include <cts/typedefs/access_flags.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBufferMemoryBarrier {
    CtsStructureType    sType;
    const void*         pNext;
    CtsAccessFlags      srcAccessMask;
    CtsAccessFlags      dstAccessMask;
    uint32_t            srcQueueFamilyIndex;
    uint32_t            dstQueueFamilyIndex;
    CtsBuffer           buffer;
    CtsDeviceSize       offset;
    CtsDeviceSize       size;
} CtsBufferMemoryBarrier;

#ifdef __cplusplus
}
#endif