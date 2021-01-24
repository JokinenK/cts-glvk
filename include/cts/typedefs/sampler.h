#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/sampler_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSamplerImpl* CtsSampler;

typedef struct CtsSamplerCreateInfo {
    CtsStructureType        sType;
    const void*             pNext;
    CtsSamplerCreateFlags   flags;
    CtsFilter               magFilter;
    CtsFilter               minFilter;
    CtsSamplerMipmapMode    mipmapMode;
    CtsSamplerAddressMode   addressModeU;
    CtsSamplerAddressMode   addressModeV;
    CtsSamplerAddressMode   addressModeW;
    float                   mipLodBias;
    bool                    anisotropyEnable;
    float                   maxAnisotropy;
    bool                    compareEnable;
    CtsCompareOp            compareOp;
    float                   minLod;
    float                   maxLod;
    CtsBorderColor          borderColor;
    bool                    unnormalizedCoordinates;
} CtsSamplerCreateInfo;

#ifdef __cplusplus
}
#endif

