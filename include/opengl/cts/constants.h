#pragma once

#include <stdint.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* getVendor();
const char* getRenderer();

uint32_t getVendorID();
uint32_t getOpenGLVersion();

#ifdef __cplusplus
}
#endif
