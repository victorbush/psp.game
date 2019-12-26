/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "engine/world.h"
#include "lua/lua_script.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void load_world_file(world_t* world, const char* filename);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void world__construct(world_t* world, const char* filename)
{
	clear_struct(world);

}

void world__destruct(world_t* world)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void load_world_file(world_t* world, const char* filename)
{
	char key[128];

	lua_script_t script;
	lua_script__construct(&script);
	lua_script__execute_file(&script, filename);

	lua_script__push(&script, "entities");
	
	boolean loop = lua_script__start_loop(&script);
	while (loop && lua_script__next(&script))
	{
		if (!lua_script__get_key(&script, key, sizeof(key)))
		{
			lua_script__cancel_loop(&script);
			break;
		}
	}

	lua_script__destruct(&script);
}