#ifdef _MSC_VER
#    define alignas __alignas
#    define alignof __alignof 
#    define __alignas_is_defined 1
#    define __alignof_is_defined 1
#else
#    include <stdalign.h>
#endif
