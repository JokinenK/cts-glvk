#pragma once

#include <stdbool.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/flags.h>
#include <cts/typedefs/framebuffer.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/query_control_flags.h>
#include <cts/typedefs/query_pipeline_statistic_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCommandBufferInheritanceInfo {
    CtsStructureType                sType;
    const void*                     pNext;
    CtsRenderPass                   renderPass;
    uint32_t                        subpass;
    CtsFramebuffer                  framebuffer;
    bool                            occlusionQueryEnable;
    CtsQueryControlFlags            queryFlags;
    CtsQueryPipelineStatisticFlags  pipelineStatistics;
} CtsCommandBufferInheritanceInfo;

#ifdef __cplusplus
}
#endif

