#ifndef _BASIC_C_TYPES_
#define _BASIC_C_TYPES_

#if !defined(_MSC_VER) && (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L )) && !defined(__cplusplus)

// ANSI C 1999/2000 stdint.h integer width declarations

typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef signed char         int8_t;   // unqualified 'char' is unsigned on ARM
typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;

#else
#include <stdint.h>
#endif


#include <stddef.h>

typedef bool BOOLEAN;
typedef uint16_t CHAR16;
#define WCHAR CHAR16

typedef uint64_t   UINT64;
typedef int64_t    INT64;

typedef uint32_t   UINT32;
typedef int32_t    INT32;

typedef uint16_t   UINT16;
typedef int16_t    INT16;

typedef uint8_t    UINT8;
typedef unsigned char       CHAR8;
typedef int8_t     INT8;

#undef VOID
typedef void       VOID;

typedef int64_t    INTN;
typedef uint64_t   UINTN;

#endif