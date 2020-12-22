#pragma once

#include <stdint.h>
#include <cts/typedefs/descriptor_set_layout.h>
#include <cts/typedefs/push_constant_range.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipelineLayout {
    uint32_t setLayoutCount;
    CtsDescriptorSetLayout* setLayouts;
    uint32_t pushConstantRangeCount;
    CtsPushConstantRange* pushConstantRanges;
};

#ifdef __cplusplus
}
#endif