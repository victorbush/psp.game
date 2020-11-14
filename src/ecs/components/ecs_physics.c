/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/ecs_component.h"
#include "ecs/components/ecs_physics.h"
#include "engine/kk_log.h"
#include "lua/lua_script.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
CONSTANTS
=========================================================*/

const char* ECS_PHYSICS_NAME = "physics";
static const char* INERTIA_NAME = "inertia";
static const char* MASS_NAME = "mass";

/*=========================================================
VARIABLES
=========================================================*/

static comp_intf_t physics_intf;

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void ecs_physics__add(ecs_t* ecs, entity_id_t ent)
{
	ecs_physics_t* comp = &ecs->physics_comp[ent];
	clear_struct(comp);
	comp->base.is_used = TRUE;
}

//## public
boolean ecs_physics__get_property
	(
	ecs_t*						ecs, 
	entity_id_t					ent,
	uint32_t					property_idx, 
	ecs_component_prop_t*		out__property
	)
{
	ecs_physics_t* comp = &ecs->physics_comp[ent];
	clear_struct(out__property);

	switch (property_idx)
	{
	case ECS_PHYSICS_PROPERTY_MASS:
		out__property->value = &comp->mass;
		out__property->type = ECS_COMPONENT_PROP_TYPE_FLOAT;
		out__property->name = MASS_NAME;
		return TRUE;

	case ECS_PHYSICS_PROPERTY_INERTIA:
		out__property->value = &comp->inertia;
		out__property->type = ECS_COMPONENT_PROP_TYPE_FLOAT;
		out__property->name = INERTIA_NAME;
		return TRUE;

	default:
		return FALSE;
	}
}

//## public
void ecs_physics__load(ecs_t* ecs, entity_id_t ent, lua_script_t* lua)
{
	/* Add component to the entity */
	ecs_physics__add(ecs, ent);
	ecs_physics_t* comp = &ecs->physics_comp[ent];
	
	/* Loop through component members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		char key[MAX_COMPONENT_NAME];
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			kk_log__error("Expected key.");
		}

		/* Inertia */
		if (!strncmp(key, INERTIA_NAME, sizeof(key)))
		{
			if (!lua_script__get_float(lua, &comp->inertia))
			{
				kk_log__error("Invalid inertia.");
				continue;
			}

			/* Compute inverse inertia */
			comp->inverse_inertia = comp->inertia ? (1.0f / comp->inertia) : 0.0f;
		}

		/* Mass */
		if (!strncmp(key, MASS_NAME, sizeof(key)))
		{
			if (!lua_script__get_float(lua, &comp->mass))
			{
				kk_log__error("Invalid mass.");
				continue;
			}

			/* Compute inverse mass */
			comp->inverse_mass = comp->mass ? (1.0f / comp->mass) : 0.0f;
		}
	}
}

//## public
void ecs_physics__register(ecs_t* ecs)
{
	/* Setup interface */
	clear_struct(&physics_intf);
	// TODO : Create a string copy utl function
	strncpy_s(physics_intf.name, sizeof(physics_intf.name), ECS_PHYSICS_NAME, sizeof(physics_intf.name) - 1);
	physics_intf.get_property = ecs_physics__get_property;
	physics_intf.load = ecs_physics__load;

	/* Register with ECS */
	ecs__register_component_intf(ecs, &physics_intf);
}
