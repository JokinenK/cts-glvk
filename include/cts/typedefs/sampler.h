#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSampler* CtsSampler;

typedef struct CtsSamplerCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsFilter magFilter;
    CtsFilter minFilter;
    CtsSamplerMipmapMode mipmapMode;
    CtsWrapType addressModeU;
    CtsWrapType addressModeV;
    CtsWrapType addressModeW;
    float mipLodBias;
    bool anisotropyEnable;
    float maxAnisotropy;
    bool compareEnable;
    CtsCompareOperator compareOp;
    float minLod;
    float maxLod;
    CtsBorderColor borderColor;
    bool unnormalizedCoordinates;
} CtsSamplerCreateInfo;

#ifdef __cplusplus
}
#endif

