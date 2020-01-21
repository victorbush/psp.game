/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/ecs_component.h"
#include "ecs/components/ecs_transform.h"
#include "log/log.h"
#include "lua/lua_script.h"
#include "thirdparty/cimgui/imgui_jetz.h"

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

//## public
void ecs_transform__add(ecs_t* ecs, entity_id_t ent)
{
	ecs_transform_t* comp = &ecs->transform_comp[ent];
	clear_struct(comp);
	comp->base.is_used = TRUE;
}
//
////## public
//void ecs_transform__draw_attributes_editor(ecs_t* ecs, entity_id_t ent)
//{
//	ecs_transform_t* comp = &ecs->transform_comp[ent];
//
//	igColumns(1, NULL, FALSE);
//	igText(ECS_TRANSFORM_NAME);
//
//	if (igInputFloat3("Position", &comp->pos, NULL, ImGuiInputTextFlags_EnterReturnsTrue))
//	{
//		printf("hi");
//
//
//	}
//	
//	//igColumns(2, NULL, FALSE);
//	//igText("Position");
//	//igNextColumn();
//	//igInputFloat3("Position", &comp->pos, NULL, 0);
//	//igNextColumn();
//}

//## public
boolean ecs_transform__get_property
	(
	ecs_t*						ecs, 
	entity_id_t					ent,
	uint32_t					property_idx, 
	ecs_component_prop_t*		out__property
	)
{
	ecs_transform_t* comp = &ecs->transform_comp[ent];
	clear_struct(out__property);

	switch (property_idx)
	{
	case ECS_TRANSFORM_PROPERTY_POS:
		out__property->value = &comp->pos;
		out__property->type = ECS_COMPONENT_PROP_TYPE_VEC3;
		out__property->name = ECS_TRANSFORM_NAME;
		return TRUE;

	default:
		return FALSE;
	}
}

//## public
void ecs_transform__load(ecs_t* ecs, entity_id_t ent, lua_script_t* lua)
{
	/* Add component to the entity */
	ecs_transform__add(ecs, ent);
	ecs_transform_t* comp = &ecs->transform_comp[ent];
	
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

		/* Position */
		if (!strncmp(key, POS_NAME, sizeof(key)))
		{
			if (!lua_script__get_array_of_float(lua, &comp->pos, 3))
			{
				log__error("Invalid position.");
				continue;
			}
		}
	}
}

//## public
void ecs_transform__register(ecs_t* ecs)
{
	/* Setup interface */
	clear_struct(&transform_intf);
	// TODO : Create a string copy utl function
	strncpy_s(transform_intf.name, sizeof(transform_intf.name), ECS_TRANSFORM_NAME, sizeof(transform_intf.name) - 1);
	transform_intf.get_property = ecs_transform__get_property;
	transform_intf.load = ecs_transform__load;

	/* Register with ECS */
	ecs__register_component_intf(ecs, &transform_intf);
}
