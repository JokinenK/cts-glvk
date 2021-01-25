#pragma once

#include <cts/typedefs/format_feature_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFormatProperties {
    CtsFormatFeatureFlags   linearTilingFeatures;
    CtsFormatFeatureFlags   optimalTilingFeatures;
    CtsFormatFeatureFlags   bufferFeatures;
} CtsFormatProperties;

#ifdef __cplusplus
}
#endif