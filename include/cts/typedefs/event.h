#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/event_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsEvent* CtsEvent;

typedef struct CtsEventCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsEventCreateFlags flags;
} CtsEventCreateInfo;

#ifdef __cplusplus
}
#endif