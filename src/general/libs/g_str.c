#include "g_str.h"

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

CHAR16* UINT32ToUnicode(UINT32 num) {
	if (num == 0) {
		CHAR16* str = malloc(sizeof(CHAR16) * 2);
		*str = '0';
		*(str + 1) = '\0';
		return str;
	}

	static const UINT32 pow10[10] = {
		1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
	};

	unsigned int b = -(num > 0) & (31 - __builtin_clzl(num));
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
UINT32 UnicodeToUINT32(CHAR16* str) {
	UINT32 num = 0;
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

CHAR16* UINT64ToUnicode(UINT64 num) {
	if (num == 0) {
		CHAR16* str = malloc(sizeof(CHAR16) * 2);
		*str = '0';
		*(str + 1) = '\0';
		return str;
	}

	static const UINT64 pow10[20] = {
		1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL,
		100000000ULL, 1000000000ULL, 10000000000ULL, 100000000000ULL, 1000000000000ULL, 10000000000000ULL,
		100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL, 1000000000000000000ULL, 10000000000000000000ULL
	};

	unsigned int b = -(num > 0) & (63 - __builtin_clzl(num));
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
UINT64 UnicodeToUINT64(CHAR16* str) {
	UINT32 num = 0;
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