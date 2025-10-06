#ifndef _GENERAL_STRINGS_
#define _GENERAL_STRINGS_

#include "c_types.h"
#include "g_stdlib.h"

/*Pointer to standard c-style string (null-terminated) - to be freed by caller*/
CHAR16* UINT16ToUnicode(UINT16 num) {
	if (num == 0) {
		CHAR16* str = malloc(sizeof(CHAR16) * 2);
		*str = '0';
		*(str + 1) = '\0';
		return str;
	}
	
	static const UINT16 pow10[4] = {
		10u, 100u, 1000u, 10000u
	};
	
	unsigned int b = -(num > 0) & (15 - __builtin_clzl(num));
    unsigned int a = (b * 77) >> 8; //approx for ln2/ln10
    unsigned int digitCount = 2 + a + (num >= pow10[a]); //+1 for null terminator
	
	CHAR16* str = malloc(sizeof(CHAR16) * digitCount);
	if (str == nullptr)
		return nullptr;
	
	/*Now begin conversion*/
	str += digitCount - 1;
	*str = '\0';
	while (num > 0) {
		--str;
		*str = (CHAR16)('0' + num % 10);
		num /= 10;
	}
	return str;
}

UINT16 UnicodeToUINT16(CHAR16* str) {
	UINT16 num = 0;
	while (*str != '\0') {
		if (*str > '9' || *str < '0') {
			return -1;
		}
		
		num *= 10;
		num += *str - '0';
		str++;
	}
	return num;
}

#endif