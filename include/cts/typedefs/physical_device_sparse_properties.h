#pragma once

#include <cts/typedefs/bool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPhysicalDeviceSparseProperties {
    CtsBool32   residencyStandard2DBlockShape;
    CtsBool32   residencyStandard2DMultisampleBlockShape;
    CtsBool32   residencyStandard3DBlockShape;
    CtsBool32   residencyAlignedMipSize;
    CtsBool32   residencyNonResidentStrict;
} CtsPhysicalDeviceSparseProperties;

#ifdef __cplusplus
}
#endif