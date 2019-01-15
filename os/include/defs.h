#ifndef _DEFS_H
#define _DEFS_H

/* Explicitly-sized versions of integer types */
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

typedef uint32_t size_t;

/*
 * Pointers and addresses are 32 bits long.
 * We use pointer types to represent addresses,
 * uintptr_t to represent the numerical values of addresses.
 */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

#define NULL 0

#endif
