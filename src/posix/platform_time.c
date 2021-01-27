#include <time.h>
#include <cts/platform_time.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t ctsGetCurrentTimeNs() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)(ts.tv_sec * 1e9 + ts.tv_nsec);
}

#ifdef __cplusplus
}
#endif