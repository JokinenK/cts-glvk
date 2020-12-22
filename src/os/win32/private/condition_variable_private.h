#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsConditionVariable {
    CONDITION_VARIABLE conditionVariable;
};

#ifdef __cplusplus
}
#endif