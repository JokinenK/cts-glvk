#pragma once

#include <cts/typedefs/surface.h>
#include <cts/typedefs/queue_family_properties.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsInstance {
    CtsSurface surface;

    struct CtsPhysicalDevice physicalDevice;
    struct CtsQueueFamilyProperties queueFamilyProperties;
};

#ifdef __cplusplus
}
#endif