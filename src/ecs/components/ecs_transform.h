#ifndef ECS_TRANSFORM_H
#define ECS_TRANSFORM_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_.h"
#include "ecs/components/ecs_transform_.h"
#include "lua/lua_script_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs_component.h"
#include "utl/utl_math.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_TRANSFORM_NAME;

/*=========================================================
TYPES
=========================================================*/

enum ecs_transform_properties_e
{
	ECS_TRANSFORM_PROPERTY_POS,

	ECS_TRANSFORM_PROPERTY__COUNT
};

/**
A coordinate transformation that can be applied to an entity.
*/
struct ecs_transform_s
{
	ecs_component_t			base;
	vec3_t					pos;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ecs_transform.public.h"

#endif /* ECS_TRANSFORM_H */