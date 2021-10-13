#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsQueueWaitIdle(
    VkQueue queue
);

#ifdef __cplusplus
}
#endif