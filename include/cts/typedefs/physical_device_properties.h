#pragma once

#include <stdint.h>
#include <cts/macros.h>
#include <cts/typedefs/bool.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/physical_device_limits.h>
#include <cts/typedefs/physical_device_sparse_properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPhysicalDeviceProperties {
    uint32_t                            apiVersion;
    uint32_t                            driverVersion;
    uint32_t                            vendorID;
    uint32_t                            deviceID;
    CtsPhysicalDeviceType               deviceType;
    char                                deviceName[CTS_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t                             pipelineCacheUUID[CTS_UUID_SIZE];
    CtsPhysicalDeviceLimits             limits;
    CtsPhysicalDeviceSparseProperties   sparseProperties;
} CtsPhysicalDeviceProperties;

#ifdef __cplusplus
}
#endif