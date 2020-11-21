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
#include "engine/kk_math.h"

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
	ECS_TRANSFORM_PROPERTY_ROT,
	ECS_TRANSFORM_PROPERTY_SCALE,

	ECS_TRANSFORM_PROPERTY__COUNT
};

/**
A coordinate transformation that can be applied to an entity.
*/
struct ecs_transform_s
{
	ecs_component_t			base;
	kk_vec3_t				pos;
	kk_vec4_t				rot;	/* quaternion */
	kk_vec3_t				scale;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ecs_transform.public.h"

#endif /* ECS_TRANSFORM_H */