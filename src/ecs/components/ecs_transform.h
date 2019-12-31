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

/**
A coordinate transformation that can be applied to an entity.
*/
struct ecs_transform_s
{
	ecs_component_t		base;
	vec3_t				pos;
};

/*=========================================================
FUNCTIONS
=========================================================*/

void ecs_transform__add(ecs_t* ecs, entity_id_t ent);

void ecs_transform__load(ecs_t* ecs, entity_id_t ent, lua_script_t* script);

void ecs_transform__register(ecs_t* ecs);

#endif /* ECS_TRANSFORM_H */