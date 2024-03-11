#pragma once

/* stdint */
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef union
{
	uint64_t bits[2];
	struct {
		uint64_t top;
		uint64_t bottom;
	};
}
uint128_t;