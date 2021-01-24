
#pragma once

#include <stdint.h>
#include <cts/macros.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsExtensionProperties {
    char        extensionName[CTS_MAX_EXTENSION_NAME_SIZE];
    uint32_t    specVersion;
} CtsExtensionProperties;

#ifdef __cplusplus
}
#endif
