#pragma once

#include <cts/typedefs/instance.h>
#include <cts/typedefs/surface.h>
#include <cts/typedefs/queue_family_properties.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsInstanceImpl {
    struct CtsPhysicalDeviceImpl physicalDevice;
};

#ifdef __cplusplus
}
#endif