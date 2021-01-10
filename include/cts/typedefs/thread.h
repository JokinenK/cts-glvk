#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsThread* CtsThread;
typedef struct CtsThreadCreateInfo {
    void(*entryPoint)(void* pArgs);
    void* args;
} CtsThreadCreateInfo;

#ifdef __cplusplus
}
#endif