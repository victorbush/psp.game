#ifndef ECS_TRANSFORM_H
#define ECS_TRANSFORM_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "lua/lua_script.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_TRANSFORM_NAME;

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void ecs_transform__add(ecs_t* ecs, entity_id_t ent);

void ecs_transform__load(ecs_t* ecs, entity_id_t ent, lua_script_t* script);

void ecs_transform__register(ecs_t* ecs);

#endif /* ECS_TRANSFORM_H */