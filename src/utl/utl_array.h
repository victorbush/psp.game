#ifndef UTL_ARRAY_H
#define UTL_ARRAY_H

/*=========================================================
INCLUDES
=========================================================*/

#include <stdlib.h>
#include <stdint.h>

/*=========================================================
TYPES
=========================================================*/

/**
Declares an array type.
@param type_name A name to identify the type.
@param type The actual C type.
*/
#define utl_array_declare_type_ex(type_name, type) \
typedef struct					\
{								\
	##type *	data;			/* pointer to array data */ \
	uint32_t	count;			/* number of used elements in the array */ \
	uint32_t	max;			/* max number of elements that can currently fit in the array */ \
} utl_array_ ## type_name ## _t

/**
Declares an array type. Used when the type name is the same as
the underlying C type.

@param type_name A name to identify the type.
*/
#define utl_array_declare_type(type_name) \
	utl_array_declare_type_ex(type_name, type_name)

#define utl_array_t(type_name) \
	utl_array_ ## type_name ##_t

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Creates an array variable and initializes it to an empty array.

@param type_name The name of the data type of the array contents.
@param var_name The name of the new variable.
*/
#define utl_array_create(type_name, var_name) \
	utl_array_##type_name##_t var_name; \
	##var_name##.data = NULL; \
	##var_name##.count = 0; \
	##var_name##.max = 0

/**
Destroys an array.
*/
#define utl_array_destroy(arr) \
	free((arr)->data); \
	(arr)->data = NULL; \
	(arr)->count = 0; \
	(arr)->max = 0

/**
Resizes an array to hold the specified number of elements.

@param arr Pointer to the array.
@param num_elements Number of elements the array will hold.
*/
#define utl_array_resize(arr, num_elements) \
do { \
	(arr)->data = realloc( \
		(arr)->data,		\
		sizeof(*((arr)->data)) * (num_elements)); \
	(arr)->max = num_elements; \
	(arr)->count = min((arr)->max, (arr)->count); \
} while(0) 

/**
Pushes an element to the end of the array.

@param arr Pointer to the array.
@param element The element to push.
*/
#define utl_array_push(arr, element) \
do { \
		/* double array size when out of room to (potentially) minimize reallocs */ \
		int new_count = (arr)->max != 0 ? (arr)->max * 2 : 1; \
		if ((arr)->count == (arr)->max) \
			utl_array_resize((arr), new_count); \
		(arr)->data[(arr)->count++] = (element); \
} while (0)

/*
Declare common types.
*/
utl_array_declare_type_ex(string, char*);
utl_array_declare_type(int);
utl_array_declare_type(uint32_t);
utl_array_declare_type(uint16_t);
utl_array_declare_type(uint8_t);
utl_array_declare_type(int32_t);
utl_array_declare_type(int16_t);
utl_array_declare_type(int8_t);

#endif /* UTL_ARRAY_H */