/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/components/ecs_transform.h"
#include "lua/lua_script.h"
#include "utl/utl_log.h"

/*=========================================================
CONSTANTS
=========================================================*/

const char* ECS_TRANSFORM_NAME = "transform";
static const char* POS_NAME = "pos";

/*=========================================================
VARIABLES
=========================================================*/

static comp_intf_t transform_intf;

/*=========================================================
FUNCTIONS
=========================================================*/

void ecs_transform__add(ecs_t* ecs, entity_id_t ent)
{
	transform_comp_t* comp = &ecs->transform_comp[ent];
	clear_struct(comp);
	comp->base.is_used = TRUE;
}

void ecs_transform__load(ecs_t* ecs, entity_id_t ent, struct lua_script_s* lua)
{
	/* Add component to the entity */
	ecs_transform__add(ecs, ent);
	transform_comp_t* comp = &ecs->transform_comp[ent];
	
	/* Loop through component members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		char key[MAX_COMPONENT_NAME];
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			LOG_ERROR("Expected key.");
		}

		/* Position */
		if (!strncmp(key, POS_NAME, sizeof(key)))
		{
			if (!lua_script__get_array_of_float(lua, &comp->pos, 3))
			{
				LOG_ERROR("Invalid position.");
				continue;
			}
		}
	}
}

void ecs_transform__register(ecs_t* ecs)
{
	/* Setup interface */
	clear_struct(&transform_intf);
	// TODO : Create a string copy utl function
	strncpy_s(transform_intf.name, sizeof(transform_intf.name), ECS_TRANSFORM_NAME, sizeof(transform_intf.name) - 1);
	transform_intf.load = ecs_transform__load;

	/* Register with ECS */
	ecs__register_component_intf(ecs, &transform_intf);
}
