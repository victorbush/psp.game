/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/ecs_component.h"
#include "ecs/components/ecs_player.h"
#include "engine/kk_log.h"
#include "lua/lua_script.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
CONSTANTS
=========================================================*/

const char* ECS_PLAYER_NAME = "player";

/*=========================================================
VARIABLES
=========================================================*/

static comp_intf_t player_intf;

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void ecs_player__add(ecs_t* ecs, entity_id_t ent)
{
	ecs_player_t* comp = &ecs->player_comp[ent];
	clear_struct(comp);
	comp->base.is_used = TRUE;
}

//## public
boolean ecs_player__get_property
	(
	ecs_t*						ecs, 
	entity_id_t					ent,
	uint32_t					property_idx, 
	ecs_component_prop_t*		out__property
	)
{
	ecs_player_t* comp = &ecs->player_comp[ent];
	clear_struct(out__property);

	switch (property_idx)
	{
	//case ECS_TRANSFORM_PROPERTY_POS:
	//	out__property->value = &comp->pos;
	//	out__property->type = ECS_COMPONENT_PROP_TYPE_VEC3;
	//	out__property->name = POS_NAME;
	//	return TRUE;

	default:
		return FALSE;
	}
}

//## public
void ecs_player__load(ecs_t* ecs, entity_id_t ent, lua_script_t* lua)
{
	if (ent != 0)
	{
		kk_log__error("Player component can only be on entity 0.");
		
		boolean loop = lua_script__start_loop(lua);
		while (loop && lua_script__next(lua))
		{
		}

		return;
	}

	/* Add component to the entity */
	ecs_player__add(ecs, ent);
	ecs_player_t* comp = &ecs->player_comp[ent];
	
	/* Loop through component members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		//char key[MAX_COMPONENT_NAME];
		//if (!lua_script__get_key(lua, key, sizeof(key)))
		//{
		//	kk_log__error("Expected key.");
		//}

		/* Position */
		//if (!strncmp(key, POS_NAME, sizeof(key)))
		//{
		//	if (!lua_script__get_array_of_float(lua, (float*)&comp->pos, 3))
		//	{
		//		kk_log__error("Invalid position.");
		//		continue;
		//	}
		//}
	}
}

//## public
void ecs_player__register(ecs_t* ecs)
{
	/* Setup interface */
	clear_struct(&player_intf);
	// TODO : Create a string copy utl function
	strncpy_s(player_intf.name, sizeof(player_intf.name), ECS_PLAYER_NAME, sizeof(player_intf.name) - 1);
	player_intf.get_property = ecs_player__get_property;
	player_intf.load = ecs_player__load;

	/* Register with ECS */
	ecs__register_component_intf(ecs, &player_intf);
}
