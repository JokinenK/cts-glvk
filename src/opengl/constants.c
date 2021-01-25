#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <glad/glad.h>
#include <cts/constants.h>
#include <private/constants_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char* strstrIgnoreCase(const char* pLhs, const char* pRhs);

static const GLchar*    gVendorName                     = NULL;
static const GLchar*    gRendererName                   = NULL;
static uint32_t         gVendorId                       = 0;
static uint32_t         gOpenGLVersion                  = 0;
          
const char* getVendor() {
    if (gVendorName == NULL) {
        gVendorName = glGetString(GL_VENDOR);
    }

    return gVendorName;
}

const char* getRenderer() {
    if (gRendererName == NULL) {
        gRendererName = glGetString(GL_RENDERER);
    }

    return gRendererName;
}

uint32_t getVendorID() {
    if (gVendorId == 0) {
        const char* pVendor = getVendor();
        if (pVendor == NULL) { return 0; }

        for (uint32_t i = 0; i < kNumVendors; ++i) {
            if (strstrIgnoreCase(kVendorNames[i], pVendor) != NULL) {
                gVendorId = kVendorIDs[i];
                break;
            }
        }
    }

    return gVendorId;
}

uint32_t getOpenGLVersion() {
    if (gOpenGLVersion == 0) {
        GLint major = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);

        GLint minor = 0;
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        
        gOpenGLVersion = CTS_MAKE_VERSION(major, minor, 0);
    }

    return gOpenGLVersion;
}

static const char* strstrIgnoreCase(const char* pNeedle, const char* pHaystack) {
    if (pNeedle == NULL) {
        return pHaystack;
    }

    if (pHaystack == NULL) {
        return NULL;
    }

    for (; *pHaystack; ++pHaystack) {
        const char* h;
        const char* n;

        for (h = pHaystack, n = pNeedle; *h && *n && (tolower(*h) == tolower(*n)); ++h, ++n) {
        }

        if (*n == '\0') {
            return pHaystack;
        }
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif
