#pragma once

#include <stdint.h>
#include <cts/typedefs/clear_value.h>
#include <cts/typedefs/image_aspect_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union CtsClearAttachment {
    CtsImageAspectFlags aspectMask;
    uint32_t            colorAttachment;
    CtsClearValue       clearValue;
} CtsClearAttachment;

#ifdef __cplusplus
}
#endif