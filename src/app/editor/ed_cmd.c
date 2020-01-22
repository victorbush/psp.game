/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/editor/ed_cmd.h"
#include "app/editor/ed_undo.h"
#include "ecs/ecs.h"
#include "log/log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _ed_cmd__set_component_property(void* context, _ed_undo_cmd_val_t* old_val, _ed_undo_cmd_val_t* new_val)
{
	_ed_cmd__set_component_property_cmd_t* ctx = (_ed_cmd__set_component_property_cmd_t*)context;
	ecs_component_prop_t prop;
	
	/* Get the property info for the component from the specified entity */
	if (!ctx->component->get_property(ctx->ecs, ctx->entity, ctx->property_idx, &prop))
	{
		log__error("Failed to get property.");
		return;
	}

	/* Assign the value to the property */
	switch (prop.type)
	{
	case ECS_COMPONENT_PROP_TYPE_BOOL:
		*(bool*)prop.value = new_val->bool_val;
		break;

	case ECS_COMPONENT_PROP_TYPE_FLOAT:
		*(float*)prop.value = new_val->float_val;
		break;

	case ECS_COMPONENT_PROP_TYPE_STRING:
		log__fatal("TODO");
		break;

	case ECS_COMPONENT_PROP_TYPE_VEC2:
		*(vec2_t*)prop.value = new_val->vec2_val;
		break;

	case ECS_COMPONENT_PROP_TYPE_VEC3:
		*(vec3_t*)prop.value = new_val->vec3_val;
		break;

	default:
		log__error("Unknown component property type.");
		break;
	}
}