/*=========================================================
Common types and utilties. 
=========================================================*/

#ifndef COMMON_H
#define COMMON_H

/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>
#include <stdlib.h>
#include <stdint.h>

/*=========================================================
CONSTANTS
=========================================================*/

#define MAX_FILENAME_CHARS 		(255)
#define SCREEN_WIDTH			(480)
#define SCREEN_HEIGHT			(272)

/*=========================================================
TYPES
=========================================================*/

typedef int boolean;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/**
Force inline attribute. Used for math functions. Borrowed from CGLM source.
*/
#if defined(_MSC_VER)
#define KK_INLINE __forceinline
#else
#define KK_INLINE static inline __attribute((always_inline))
#endif

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Zeros a struct.
@param ptr Pointer to the struct to clear.
*/
#define clear_struct(ptr)	memset(ptr, 0, sizeof(*ptr))

/**
Determines the number of elements in an array. Array size must be
known at compile time.
@param arr The array to get the number of elements in.
*/
#define cnt_of_array(arr)	( sizeof(arr) / sizeof((arr)[0]) )

#ifndef max
#define max( a, b ) ( ((a)>(b))?(a):(b) )
#endif

#ifndef min
#define min( a, b ) ( ((a)<(b))?(a):(b) )
#endif

#ifdef JETZ_CONFIG_PLATFORM_PSP
int strncpy_s
	(
	char*       _Destination,
	size_t     _SizeInBytes,
	char const* _Source,
	size_t     _MaxCount
	);
#endif

#endif /* COMMON_H */