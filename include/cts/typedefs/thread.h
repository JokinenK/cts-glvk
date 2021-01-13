#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsThread* CtsThread;
typedef struct CtsThreadCreateInfo {
    void(*pfEntryPoint)(void* pArgs);
    void* pArgs;
} CtsThreadCreateInfo;

#ifdef __cplusplus
}
#endif