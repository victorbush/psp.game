/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "engine/world.h"
#include "geo/geo.h"
#include "lua/lua_script.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void load_world_file(world_t* world, ecs_t* ecs, const char* filename);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void world__construct(world_t* world, ecs_t* ecs, const char* filename)
{
	clear_struct(world);

	geo__construct(&world->geo);
	load_world_file(world, ecs, filename);
}

void world__destruct(world_t* world)
{
	geo__destruct(&world->geo);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void load_world_file(world_t* world, ecs_t* ecs, const char* filename)
{
	char key[128];

	/* Load the world script file */
	lua_script_t script;
	lua_script__construct(&script);
	lua_script__execute_file(&script, filename);

	if (!lua_script__push(&script, "world"))
	{
		FATAL("Expected world definition.");
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
				FATAL("Failed to allocate new entity.");
			}

			/* Load components */
			boolean comp_loop = lua_script__start_loop(&script);
			while (comp_loop && lua_script__next(&script))
			{
				/* Get component name */
				if (!lua_script__get_key(&script, key, sizeof(key)))
				{
					LOG_ERROR("Expected component name.");
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
				LOG_ERROR("Invalid geometry object.");
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
						FATAL("Failed to allocation plane.");
					}

					geo_plane__construct(plane, &g_engine->gpu);
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