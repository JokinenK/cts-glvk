#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsThreadImpl {
    HANDLE thread;
    DWORD threadId;
    bool joined;
    bool detached;
    CtsThreadCreateInfo createInfo;
};

#ifdef __cplusplus
}
#endif