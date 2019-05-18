#ifndef COMMON_H
#define COMMON_H

/*=========================================================
INCLUDES
=========================================================*/

#include <stdint.h>

/*=========================================================
CONSTANTS
=========================================================*/

#define MAX_FILENAME_CHARS 	( 255 )
#define SCREEN_WIDTH (480)
#define SCREEN_HEIGHT (272)

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

/*=========================================================
MACROS
=========================================================*/

//#define compiler_assert_msg(test, msg) typedef char compiler_assert_##msg[(!!(test))*2-1]
//#define compiler_assert(test) compiler_assert_msg(test, "Compiler assert.")

#define cnt_of_array(arr)	( sizeof( arr ) / sizeof((arr)[0] ) )

/**
Zeros a struct.
@param ptr Pointer to the struct to clear.
*/
#define clear_struct(ptr)	memset(ptr, 0, sizeof(*ptr))

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* COMMON_H */