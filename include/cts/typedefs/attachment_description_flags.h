#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsAttachmentDescriptionFlags;
typedef enum CtsAttachmentDescriptionFlagBits {
    CTS_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT = 0x00000001,
} CtsAttachmentDescriptionFlagBits;

#ifdef __cplusplus
}
#endif

