/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "ecs/ecs_component.h"
#include "ecs/components/ecs_static_model.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "lua/lua_script.h"

/*=========================================================
CONSTANTS
=========================================================*/

const char* ECS_STATIC_MODEL_NAME = "static_model";
static const char* MATERIAL_NAME = "material";
static const char* MODEL_NAME = "model";

/*=========================================================
VARIABLES
=========================================================*/

static comp_intf_t static_model_intf;

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void ecs_static_model__add(ecs_t* ecs, entity_id_t ent)
{
	ecs_static_model_t* comp = &ecs->static_model_comp[ent];
	clear_struct(comp);
	comp->base.is_used = TRUE;
}

//## public
boolean ecs_static_model__get_property
	(
	ecs_t*						ecs, 
	entity_id_t					ent,
	uint32_t					property_idx, 
	ecs_component_prop_t*		out__property
	)
{
	ecs_static_model_t* comp = &ecs->static_model_comp[ent];
	clear_struct(out__property);

	switch (property_idx)
	{
	case ECS_STATIC_MODEL_PROPERTY_MATERIAL:
		out__property->name = MATERIAL_NAME;
		out__property->value = comp->material_filename;
		out__property->value_size = sizeof(comp->material_filename);
		out__property->type = ECS_COMPONENT_PROP_TYPE_STRING;
		break;

	case ECS_STATIC_MODEL_PROPERTY_MODEL:
		out__property->name = MODEL_NAME;
		out__property->value = comp->model_filename;
		out__property->value_size = sizeof(comp->model_filename);
		out__property->type = ECS_COMPONENT_PROP_TYPE_STRING;
		break;

	default:
		return FALSE;
	}
}

//## public
void ecs_static_model__load(ecs_t* ecs, entity_id_t ent, lua_script_t* lua)
{
	/* Add component to the entity */
	ecs_static_model__add(ecs, ent);
	ecs_static_model_t* comp = &ecs->static_model_comp[ent];
	
	/* Loop through component members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		char key[MAX_COMPONENT_NAME];
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			log__error("Expected key.");
		}

		/* Material */
		if (!strncmp(key, MATERIAL_NAME, sizeof(key)))
		{
			if (!lua_script__get_string(lua, comp->material_filename, sizeof(comp->material_filename)))
			{
				log__error("Invalid material filename.");
				continue;
			}

			/* Load material */
			comp->material = gpu__load_material(g_gpu, comp->material_filename);
		}

		/* Model */
		if (!strncmp(key, MODEL_NAME, sizeof(key)))
		{
			if (!lua_script__get_string(lua, comp->model_filename, sizeof(comp->model_filename)))
			{
				log__error("Invalid model filename.");
			}

			/* Load model */
			comp->model = gpu__load_static_model(g_gpu, comp->model_filename);
		}
	}
}

//## public
void ecs_static_model__register(ecs_t* ecs)
{
	/* Setup interface */
	clear_struct(&static_model_intf);
	// TODO : Create a string copy utl function
	strncpy_s(static_model_intf.name, sizeof(static_model_intf.name), ECS_STATIC_MODEL_NAME, sizeof(static_model_intf.name) - 1);
	static_model_intf.get_property = ecs_static_model__get_property;
	static_model_intf.load = ecs_static_model__load;

	/* Register with ECS */
	ecs__register_component_intf(ecs, &static_model_intf);
}
