#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsConditionVariableImpl {
    CONDITION_VARIABLE conditionVariable;
};

#ifdef __cplusplus
}
#endif