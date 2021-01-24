#pragma once

#include <stdint.h>
#include <cts/typedefs/access_flags.h>
#include <cts/typedefs/dependency_flags.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/pipeline_stage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSubpassDependency {
    uint32_t                srcSubpass;
    uint32_t                dstSubpass;
    CtsPipelineStageFlags   srcStageMask;
    CtsPipelineStageFlags   dstStageMask;
    CtsAccessFlags          srcAccessMask;
    CtsAccessFlags          dstAccessMask;
    CtsDependencyFlags      dependencyFlags;
} CtsSubpassDependency;

#ifdef __cplusplus
}
#endif

