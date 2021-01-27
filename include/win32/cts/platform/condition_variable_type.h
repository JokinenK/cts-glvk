#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CONDITION_VARIABLE CtsPlatformConditionVariable;

#ifdef __cplusplus
}
#endif