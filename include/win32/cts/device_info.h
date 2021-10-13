#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsWin32DeviceInfo {
    uint32_t    vendorId;
    uint32_t    deviceId;
    char        instance[0xFF];
    uint8_t     uuid[16];
} CtsWin32DeviceInfo;

bool ctsWin32ParseDeviceInfo(uint32_t vendorId, CtsWin32DeviceInfo* pDeviceInfo);

#ifdef __cplusplus
}
#endif