#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/attachment_description_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAttachmentDescription {
    CtsAttachmentDescriptionFlags flags;
    CtsFormat format;
    CtsSampleCountFlagBits samples;
    CtsAttachmentLoadOp loadOp;
    CtsAttachmentStoreOp storeOp;
    CtsAttachmentLoadOp stencilLoadOp;
    CtsAttachmentStoreOp stencilStoreOp;
    CtsImageLayout initialLayout;
    CtsImageLayout finalLayout;
} CtsAttachmentDescription;

#ifdef __cplusplus
}
#endif