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
	##type## *	data;			/* pointer to array data */ \
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
	utl_array_init(&##var_name##)

/**
Destroys an array.
*/
#define utl_array_destroy(arr) \
	free((arr)->data); \
	(arr)->data = NULL; \
	(arr)->count = 0; \
	(arr)->max = 0

/**
Initializes an array variable to an empty array.

@param arr A pointer to the array to initialize.
*/
#define utl_array_init(arr) \
	(arr)->data = NULL; \
	(arr)->count = 0; \
	(arr)->max = 0

/**
Resizes an array to hold the specified number of elements.
Does not update the count of elements in the array.
Useful if you are going to push a number of elements
to the back of the array in quick succession.

@param arr Pointer to the array.
@param num_elements Number of elements the array will hold.
*/
#define utl_array_reserve(arr, num_elements) \
do { \
	/* Handle empty/invalid array size */ \
	if (num_elements <= 0) { \
		free((arr)->data); \
		(arr)->data = NULL; \
		(arr)->max = 0; \
		(arr)->count = 0; \
		break; \
	} \
\
	/* Update memory allocation */ \
	void* new_ptr = realloc( \
		(arr)->data,		\
		sizeof(*((arr)->data)) * (num_elements)); \
\
	/* realloc returns NULL when allocation fails */ \
	if (new_ptr) { \
		(arr)->data = new_ptr; \
		(arr)->max = num_elements; \
		(arr)->count = min((arr)->max, (arr)->count); \
	} else { \
		/* Memory allocation failed */ \
		assert(0); \
	} \
} while(0) 

/**
Resizes an array to hold the specified number of elements,
and updates the count of elements to include all possible.
Useful if you need to memcpy a chunk of data into an array.

@param arr Pointer to the array.
@param num_elements Number of elements the array will hold.
*/
#define utl_array_resize(arr, num_elements) \
do { \
	utl_array_reserve((arr), (num_elements)); \
	(arr)->count = num_elements; \
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
			utl_array_reserve((arr), new_count); \
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