#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAttachmentReference {
    uint32_t        attachment;
    CtsImageLayout  layout;
} CtsAttachmentReference;

#ifdef __cplusplus
}
#endif

