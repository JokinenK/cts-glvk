#pragma once

#include <windows.h> 

#ifdef __cplusplus
extern "C" {
#endif

struct CtsMutex {
    CRITICAL_SECTION criticalSection;
};

#ifdef __cplusplus
}
#endif