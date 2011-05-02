#ifndef _WRTYPES_H_
#define _WRTYPES_H_

// TYPES
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef u8  uint8;
typedef u16 uint16;
typedef float f32;


#ifndef FORCEINLINE
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define FORCEINLINE __forceinline
#define MSC_FORCEINLINE __forceinline
#else
#define FORCEINLINE inline __attribute__((always_inline))
#define MSC_FORCEINLINE
#endif
#endif

#endif




