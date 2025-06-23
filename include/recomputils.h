#ifndef __RECOMPUTILS_H__
#define __RECOMPUTILS_H__

#include "modding.h"

RECOMP_IMPORT("*", void* recomp_alloc(unsigned long size));
RECOMP_IMPORT("*", void recomp_free(void* memory));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

// These functions let you get the return value of a function from within a return hook for that function.
// Calling these outside of a return hook will give an undefined result.
// Calling the function for a return type that doesn't match the function's actual return type also gives an undefined result.
RECOMP_IMPORT("*", s32 recomphook_get_return_s32());
RECOMP_IMPORT("*", u32 recomphook_get_return_u32());
RECOMP_IMPORT("*", void* recomphook_get_return_ptr());
RECOMP_IMPORT("*", s16 recomphook_get_return_s16());
RECOMP_IMPORT("*", u16 recomphook_get_return_u16());
RECOMP_IMPORT("*", s8 recomphook_get_return_s8());
RECOMP_IMPORT("*", u8 recomphook_get_return_u8());
RECOMP_IMPORT("*", s64 recomphook_get_return_s64());
RECOMP_IMPORT("*", u64 recomphook_get_return_u64());
RECOMP_IMPORT("*", float recomphook_get_return_float());
RECOMP_IMPORT("*", double recomphook_get_return_double());

#endif
