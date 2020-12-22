#pragma once

#include <stdint.h>
#include <cts/typedefs/descriptor_set_layout.h>
#include <cts/typedefs/push_constant_range.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPipelineLayout* CtsPipelineLayout;
typedef struct CtsPipelineLayoutCreateInfo {
    CtsStructureType sType;
    const void* next;
    uint32_t setLayoutCount;
    const CtsDescriptorSetLayout* setLayouts;
    uint32_t pushConstantRangeCount;
    const CtsPushConstantRange* pushConstantRanges;
} CtsPipelineLayoutCreateInfo;

#ifdef __cplusplus
}
#endif

