#pragma once

#include <stdint.h>

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
