/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "lua/lua_script.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Loads data from the material script file. */
static void load_material_script(gpu_material_t* material, const char* filename);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_material__construct(gpu_material_t* material, gpu_t* gpu, const char* filename)
{
	clear_struct(material);

	load_material_script(material, filename);
	gpu->intf->material__construct(material, gpu);
}

void gpu_material__destruct(gpu_material_t* material, gpu_t* gpu)
{
	gpu->intf->material__destruct(material);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void load_material_script(gpu_material_t* material, const char* filename)
{
	const char* MATERIAL_VAR = "material";
	const char* AMBIENT_COLOR_VAR = "a";
	const char* DIFFUSE_COLOR_VAR = "d";
	const char* SPECULAR_COLOR_VAR = "s";
	const char* DIFFUSE_TEXTURE_VAR = "dt";

	lua_script_t script;
	lua_script__construct(&script);
	if (!lua_script__execute_file(&script, filename))
	{
		FATAL("Failed to load material file.");
	}

	/* Find material definition */
	if (lua_script__push(&script, MATERIAL_VAR))
	{
		/* Ambient color */
		lua_script__get_array_of_float_var(&script, AMBIENT_COLOR_VAR, &material->ambient_color, 3);

		/* Diffuse color */
		lua_script__get_array_of_float_var(&script, DIFFUSE_COLOR_VAR, &material->diffuse_color, 3);

		/* Specular color */
		lua_script__get_array_of_float_var(&script, SPECULAR_COLOR_VAR, &material->specular_color, 3);

		/* Diffuse texture */
		lua_script__get_string_var(&script, DIFFUSE_TEXTURE_VAR, material->diffuse_texture_name, sizeof(material->diffuse_texture_name));
	}

	/* Cleanup script */
	lua_script__destruct(&script);

	/* Load textures */
	//gpu__asdfasdfsadf
}