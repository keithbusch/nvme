#ifdef CREATE_TSTRUCT
#undef CREATE_TSTRUCT

#define __stringify_1(x...) #x
#define __stringify(x...)  __stringify_1(x)
#define __TSTRUCT_INCLUDE(TSTRUCT) __stringify(TSTRUCT.h)
#define TSTRUCT_INCLUDE(TSTRUCT) __TSTRUCT_INCLUDE(TSTRUCT)

#define TSTRUCT_HEADER_MULTI_READ

#include TSTRUCT_INCLUDE(TSTRUCT_INC_FILE)

#include "tstruct_handler.h"

#undef TSTRUCT_HEADER_MULTI_READ

#define CREATE_TSTRUCT
#endif
