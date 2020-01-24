/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/editor/ed.h"
#include "app/editor/ed_cmd.h"
#include "app/editor/ed_ui_properties.h"
#include "log/log.h"

#include "autogen/ed_undo.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## internal
void _ed_ui_properties__construct(_ed_ui_properties_t* prop, _ed_t* ed)
{
	clear_struct(prop);
	prop->ed = ed;
}

//## internal
void _ed_ui_properties__destruct(_ed_ui_properties_t* prop)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _ed_ui_properties__think(_ed_ui_properties_t* prop)
{
	// TODO : Visibility option in view menu
	//if (!prop->is_visible)
	//{
	//	return;
	//}

	if (igBegin("Properties", NULL, 0))
	{
		_ed_t* ed = prop->ed;

		if (ed->selected_entity == ECS_INVALID_ID)
		{
			igEnd();
			return;
		}

		ecs_t* ecs = &ed->world.ecs;

		igColumns(2, NULL, FALSE);
		igText("Id");
		igNextColumn();

		char id[12];
		_snprintf_s(id, sizeof(id), _TRUNCATE, "%i", ed->selected_entity);
		igText(id);

		igNextColumn();

		const char* key;
		map_iter_t iter = map_iter(&ecs->component_registry);
		while ((key = map_next(&ecs->component_registry, &iter)))
		{
			comp_intf_t** cached_comp = (gpu_static_model_t**)map_get(&ecs->component_registry, key);
			if (!cached_comp)
			{
				continue;
			}

			/* Dereference double pointer for simplicity */
			comp_intf_t* comp = *cached_comp;
			if (!comp)
			{
				continue;
			}

			// TODO : Check if selected entity has this component
			igColumns(1, NULL, FALSE);
			igText(comp->name);
			//igCollapsingHeader(comp->name, 0);

			/* Make sure component supports enumerating property info */
			if (!comp->get_property)
			{
				continue;
			}

			uint32_t prop_idx = 0;
			ecs_component_prop_t prop_info;

			while (comp->get_property(ecs, ed->selected_entity, prop_idx, &prop_info))
			{
				switch (prop_info.type)
				{
				case ECS_COMPONENT_PROP_TYPE_STRING:
				{
					/* Save old value -- memory freed by under buffer */
					//char* old_val = malloc(prop_info.value_size);
					//strncpy_s(old_val, prop_info.value_size, (const char*)prop_info.value, prop_info.value_size - 1);

					/* Don't save off old value until enter key is pressed. Use the imgui text callback for this. */

					char* old_val = NULL;

					if (igInputText(prop_info.name, (char*)prop_info.value, prop_info.value_size, ImGuiInputTextFlags_EnterReturnsTrue, NULL, NULL))
					{
						log__dbg("Hi\n");
					}
				}
				break;

				case ECS_COMPONENT_PROP_TYPE_VEC3:
				{
					vec3_t old_val = *(vec3_t*)prop_info.value;

					if (igInputFloat3(prop_info.name, (float*)prop_info.value, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
					{
						/* Memory is freed by the under buffer */
						_ed_cmd__set_component_property_cmd_t* cmd = malloc(sizeof(_ed_cmd__set_component_property_cmd_t));
						if (!cmd)
						{
							log__fatal("Failed to allocate memory.");
							break;
						}

						cmd->component = comp;
						cmd->ecs = &ed->world.ecs;
						cmd->entity = ed->selected_entity;
						cmd->property_idx = prop_idx;

						_ed_undo__create_vec3(&ed->undo_buffer, cmd, old_val, *(vec3_t*)prop_info.value, _ed_cmd__set_component_property);
					}
				}
				break;

				}

				prop_idx++;
			}
		}

		igEnd();
	}
}
