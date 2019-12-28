#ifndef ECS_STATIC_MODEL_H
#define ECS_STATIC_MODEL_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_STATIC_MODEL_NAME;

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void ecs_static_model__add(ecs_t* ecs, entity_id_t ent);

void ecs_static_model__load(ecs_t* ecs, entity_id_t ent, struct lua_script_s* script);

void ecs_static_model__register(ecs_t* ecs);

#endif /* ECS_TRANSFORM_H */