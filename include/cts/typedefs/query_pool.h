#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/query_pipeline_statistic_flags.h>
#include <cts/typedefs/query_pool_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueryPool* CtsQueryPool;

typedef struct CtsQueryPoolCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsQueryPoolCreateFlags flags;
    CtsQueryType queryType;
    uint32_t queryCount;
    CtsQueryPipelineStatisticFlags pipelineStatistics;
} CtsQueryPoolCreateInfo;

#ifdef __cplusplus
}
#endif
                              