#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_.h"
#include "ecs/ecs_component_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

utl_array_declare_type(ecs_component_prop_t);

enum ecs_component_prop_type_e
{
	ECS_COMPONENT_PROP_TYPE_BOOL,
	ECS_COMPONENT_PROP_TYPE_FLOAT,
	ECS_COMPONENT_PROP_TYPE_VEC2,
	ECS_COMPONENT_PROP_TYPE_VEC3,
	ECS_COMPONENT_PROP_TYPE_STRING,

	ECS_COMPONENT_PROP_TYPE__COUNT,
};

struct ecs_component_prop_s
{
	const char*					name;
	void*						value;
	size_t						value_size;		/* Size of the value buffer. Useful for strings and arrays. */
	ecs_component_prop_type_t	type;
	//boolean						is_array;
};

/**
Base struct for all components. Must be the chunk of data in each component.
*/
struct ecs_component_s
{
	boolean 							is_used;
};

#endif /* ECS_COMPONENT_H */