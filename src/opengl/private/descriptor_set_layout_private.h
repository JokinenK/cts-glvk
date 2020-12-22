#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/descriptor_set_layout.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorSetLayout {
    uint32_t bindingCount;
    CtsDescriptorSetLayoutBinding* bindings;
};

#ifdef __cplusplus
}
#endif