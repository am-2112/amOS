#ifndef _GENERAL_STRINGS_
#define _GENERAL_STRINGS_

#include "c_types.h"
#include "g_stdlib.h"

/*Pointer to standard c-style string (null-terminated) - to be freed by caller*/
extern CHAR16* UINT16ToUnicode(UINT16 num);
extern UINT16 UnicodeToUINT16(CHAR16* str);

extern CHAR16* UINT32ToUnicode(UINT32 num);
extern UINT32 UnicodeToUINT32(CHAR16* str);

extern CHAR16* UINT64ToUnicode(UINT64 num);
extern UINT64 UnicodeToUINT64(CHAR16* str);

#endif