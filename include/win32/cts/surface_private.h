#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <stdint.h>
#include "cts/surface.h"
#include "cts/object_base.h"
#include "cts/gl_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSurface {
    struct CtsObjectBase base;
    struct CtsGlContext context;
};

struct CtsGlContext* ctsSurfaceGetGlContext(struct CtsSurface* surface);
bool ctsSurfaceQueryDeviceDetails(struct CtsSurface* surface, uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID);
VkResult ctsGetSurfaceExtent(struct CtsSurface* surface, VkExtent2D* pExtent);

#ifdef __cplusplus
}
#endif

