#ifndef _GENERAL_STRINGS_
#define _GENERAL_STRINGS_

#include "c_types.h"
#include "g_stdlib.h"

/*Pointer to standard c-style string (null-terminated) - to be freed by caller*/
extern CHAR16* UINT16ToUnicode(UINT16 num);

extern UINT16 UnicodeToUINT16(CHAR16* str);

#endif