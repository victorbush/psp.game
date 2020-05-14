/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "engine/kk_log.h"
#include "engine/kk_world.h"
#include "geo/geo.h"
#include "lua/lua_script.h"

#include "autogen/kk_world.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Loads a world.
@param world The world to construct.
@param ecs The entity component system to use.
@param filename The name of the world file.
*/
void kk_world__construct(kk_world_t* world, const char* filename)
{
	clear_struct(world);

	ecs__construct(&world->ecs);
	geo__construct(&world->geo);
	load_world_file(world, filename);
}

//## public
/**
Destructs a world.
@param world The world to destruct.
*/
void kk_world__destruct(kk_world_t* world)
{
	geo__destruct(&world->geo);
	ecs__destruct(&world->ecs);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void kk_world__export_lua(kk_world_t* world, const char* filename)
{
	FILE* f = NULL;
	errno_t err = fopen_s(&f, filename, "w");
	ecs_t* ecs = &world->ecs;

	if (err != 0 || !f)
	{
		kk_log__error("Failed to open file.");
		return;
	}

	/* Start world */
	fprintf_s(f, "world = \n{\n");
	
	/* Start entities */
	fprintf_s(f, "\tentities = \n\t{\n");
	{
		entity_id_t ent = ECS_INVALID_ID;
		boolean first_entity = TRUE;

		/* Iterate through entities */
		while (ecs__iterate(&world->ecs, &ent))
		{
			/* Add a comma to the end of the previous entity */
			if (!first_entity)
			{
				fprintf_s(f, ",\n");
			}
			else
			{
				first_entity = FALSE;
			}

			/* Begin entity */
			fprintf_s(f, "\t\t{\n");

			/* Iterate components */
			const char* key;
			map_iter_t iter = map_iter(&ecs->component_registry);
			boolean first_component = TRUE;

			while ((key = map_next(&ecs->component_registry, &iter)))
			{
				comp_intf_t** cached_comp = (comp_intf_t**)map_get(&ecs->component_registry, key);
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

				/* Add a comma to the end of the previous component */
				if (!first_component)
				{
					fprintf_s(f, ",\n");
				}
				else
				{
					first_component = FALSE;
				}

				/* Begin component */
				fprintf_s(f, "\t\t\t%s = {\n", comp->name);

				/* Write component properties */
				ecs_component_prop_t prop_info;
				uint32_t prop_idx = 0;

				while (comp->get_property(ecs, ent, prop_idx, &prop_info))
				{
					/* Add a comma to the end of the previous property */
					if (prop_idx > 0)
					{
						fprintf_s(f, ",\n");
					}

					/* Begin property */
					fprintf_s(f, "\t\t\t\t%s = ", prop_info.name);

					switch (prop_info.type)
					{
					case ECS_COMPONENT_PROP_TYPE_BOOL:
					{
						boolean val = *(boolean*)prop_info.value;
						fprintf_s(f, "%s", val ? "true" : "false");
						break;
					}
					case ECS_COMPONENT_PROP_TYPE_FLOAT:
					{
						float val = *(float*)prop_info.value;
						fprintf_s(f, "%.2f", val);
						break;
					}
					case ECS_COMPONENT_PROP_TYPE_STRING:
					{
						const char* val = (const char*)prop_info.value;
						fprintf_s(f, "\"%s\"", val);
						break;
					}
					case ECS_COMPONENT_PROP_TYPE_VEC2:
					{
						kk_vec2_t val = *(kk_vec2_t*)prop_info.value;
						fprintf_s(f, "{ %.2f, %.2f }", val.x, val.y);
						break;
					}
					case ECS_COMPONENT_PROP_TYPE_VEC3:
					{
						kk_vec3_t val = *(kk_vec3_t*)prop_info.value;
						fprintf_s(f, "{ %.2f, %.2f, %.2f }", val.x, val.y, val.z);
						break;
					}
					default:
						kk_log__error("Unknown component type.");
						break;
					}

					prop_idx++;
				}

				/* End component */
				fprintf_s(f, "\n\t\t\t}");
			}

			/* End entity */
			fprintf_s(f, "\n\t\t}");
		}
	}
	/* End entities */
	fprintf_s(f, "\n\t},\n");

	/* Start geometry */
	fprintf_s(f, "\tgeometry =\n\t{");
	{

	}
	/* End geometry */
	fprintf_s(f, "\t}\n");

	/* End world */
	fprintf_s(f, "}\n");

	/* Close file */
	fclose(f);
}

//## static
static void load_world_file(kk_world_t* world, const char* filename)
{
	char key[128];
	ecs_t* ecs = &world->ecs;

	/* Load the world script file */
	lua_script_t script;
	lua_script__construct(&script);

	if (!lua_script__execute_file(&script, filename))
	{
		kk_log__error("File does not exist.");
	}

	if (!lua_script__push(&script, "world"))
	{
		kk_log__error("Expected world definition.");
	}

	/* Proess entities list */
	if (lua_script__push(&script, "entities"))
	{
		boolean loop = lua_script__start_loop(&script);
		while (loop && lua_script__next(&script))
		{
			/* Create new entity */
			entity_id_t ent = ecs__alloc_entity(ecs);
			if (ent == ECS_INVALID_ID)
			{
				kk_log__fatal("Failed to allocate new entity.");
			}

			/* Load components */
			boolean comp_loop = lua_script__start_loop(&script);
			while (comp_loop && lua_script__next(&script))
			{
				/* Get component name */
				if (!lua_script__get_key(&script, key, sizeof(key)))
				{
					kk_log__error("Expected component name.");
					continue;
				}

				/* Load the component */
				ecs__load_component(ecs, ent, key, &script);
			}
		}

		/* Pop entities list */
		lua_script__pop(&script, 1);
	}

	/* Process geometry */
	if (lua_script__push(&script, "geometry"))
	{
		boolean loop = lua_script__start_loop(&script);
		while (loop && lua_script__next(&script))
		{
			/* Check type */
			if (!lua_script__push(&script, "type"))
			{
				kk_log__error("Invalid geometry object.");
				continue;
			}

			char type[64];
			if (lua_script__get_string(&script, type, sizeof(type)))
			{
				if (!strncmp(type, "plane", sizeof(type - 1)))
				{
					/* Load plane */
					lua_script__pop(&script, 1);

					struct geo_plane_s* plane = geo__alloc_plane(&world->geo);
					if (!plane)
					{
						kk_log__fatal("Failed to allocate plane.");
					}

					geo_plane__construct(plane, g_gpu);
					geo_plane__load(plane, &script);
				}
			}
			else
			{
				lua_script__pop(&script, 1);
			}
		}

		/* Pop geometry list */
		lua_script__pop(&script, 1);
	}

	/* Done with script */
	lua_script__destruct(&script);
}