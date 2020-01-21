#ifndef ECS_STATIC_MODEL_H
#define ECS_STATIC_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_.h"
#include "ecs/components/ecs_static_model_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_static_model_.h"
#include "lua/lua_script_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs_component.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_STATIC_MODEL_NAME;

/*=========================================================
TYPES
=========================================================*/

/**
A static model without any animation.
*/
struct ecs_static_model_s
{
	ecs_component_t				base;
	char						file_name[MAX_FILENAME_CHARS];
	gpu_static_model_t*			model;
	gpu_material_t*				material;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ecs_static_model.public.h"

#endif /* ECS_TRANSFORM_H */