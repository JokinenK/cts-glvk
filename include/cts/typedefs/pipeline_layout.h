#pragma once

#include <stdint.h>
#include <cts/typedefs/descriptor_set_layout.h>
#include <cts/typedefs/push_constant_range.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPipelineLayoutImpl* CtsPipelineLayout;
typedef struct CtsPipelineLayoutCreateInfo {
    CtsStructureType                sType;
    const void*                     pNext;
    uint32_t                        setLayoutCount;
    const CtsDescriptorSetLayout*   pSetLayouts;
    uint32_t                        pushConstantRangeCount;
    const CtsPushConstantRange*     pPushConstantRanges;
} CtsPipelineLayoutCreateInfo;

#ifdef __cplusplus
}
#endif

