#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*CtsThreadEntryPoint)(void* pArgs);
typedef struct CtsThreadImpl* CtsThread;
typedef struct CtsThreadCreateInfo {
    CtsThreadEntryPoint pfEntryPoint;
    void*               pArgs;
} CtsThreadCreateInfo;

#ifdef __cplusplus
}
#endif