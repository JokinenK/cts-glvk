#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsImageCreateFlags;
typedef enum CtsImageCreateFlagBits {
    CTS_IMAGE_CREATE_SPARSE_BINDING_BIT = 0x00000001,
    CTS_IMAGE_CREATE_SPARSE_RESIDENCY_BIT = 0x00000002,
    CTS_IMAGE_CREATE_SPARSE_ALIASED_BIT = 0x00000004,
    CTS_IMAGE_CREATE_MUTABLE_FORMAT_BIT = 0x00000008,
    CTS_IMAGE_CREATE_CUBE_COMPATIBLE_BIT = 0x00000010,
} CtsImageCreateFlagBits;

#ifdef __cplusplus
}
#endif