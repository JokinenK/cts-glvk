#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h> 

#ifdef __cplusplus
extern "C" {
#endif

typedef CRITICAL_SECTION CtsPlatformMutex;

#ifdef __cplusplus
}
#endif