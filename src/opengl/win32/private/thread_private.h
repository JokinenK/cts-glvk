#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsThread {
    HANDLE thread;
    DWORD threadId;
    bool joined;
    bool detached;
};

#ifdef __cplusplus
}
#endif