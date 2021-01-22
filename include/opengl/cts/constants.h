#pragma once

#include <stdint.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsDeviceSize getUniformBufferOffsetAlignment();
CtsDeviceSize getTextureBufferOffsetAlignment();

#ifdef __cplusplus
}
#endif
