#include <windows.h>
#include <cts/time.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t ctsGetCurrentTimeNs() {
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);

  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);

  double seconds = ((double)counter.QuadPart) / (double)freq.QuadPart;
  return (uint64_t)(seconds * 1e9);
}

#ifdef __cplusplus
}
#endif