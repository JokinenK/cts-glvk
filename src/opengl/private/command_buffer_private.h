#pragma once

#include <cts/typedefs/command_pool.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/enums.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandBuffer {
    CtsDevice device;
    CtsCommandPool pool;
    CtsCmdBase* root;
    CtsCmdBase* current;
    CtsCommandBufferLevel level;
};

#ifdef __cplusplus
}
#endif