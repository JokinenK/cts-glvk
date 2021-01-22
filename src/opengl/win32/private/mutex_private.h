#pragma once

#include <windows.h> 

#ifdef __cplusplus
extern "C" {
#endif

struct CtsMutexImpl {
    CRITICAL_SECTION criticalSection;
};

#ifdef __cplusplus
}
#endif