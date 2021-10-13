#pragma once

#include "vulkan/vk_icd.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsObjectBase {
    VK_LOADER_DATA loaderData;
};

#ifdef __cplusplus
}
#endif