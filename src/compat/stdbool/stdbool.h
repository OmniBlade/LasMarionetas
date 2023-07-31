#ifndef COMPAT_STD_BOOL
#define COMPAT_STD_BOOL

#define __bool_true_false_are_defined 1

#ifndef __cplusplus

typedef unsigned char _Bool;

#define bool _Bool
#define false 0
#define true 1

#endif

#endif /* COMPAT_STD_BOOL */
