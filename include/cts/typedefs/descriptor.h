#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/image_view.h>
#include <cts/typedefs/sampler.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorImageInfo {
    CtsSampler      sampler;
    CtsImageView    imageView;
    CtsImageLayout  imageLayout;
} CtsDescriptorImageInfo;

typedef struct CtsDescriptorBufferInfo {
    CtsBuffer       buffer;
    CtsDeviceSize   offset;
    CtsDeviceSize   range;
} CtsDescriptorBufferInfo;

#ifdef __cplusplus
}
#endif
