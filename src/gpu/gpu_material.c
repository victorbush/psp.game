/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "lua/lua_script.h"

#include "autogen/gpu_material.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Constructs a material.

@param material The material to construct.
@param gpu The GPU context.
@param create_info The material info.
*/
void gpu_material__construct(gpu_material_t* material, gpu_t* gpu, gpu_material_create_info_t* create_info)
{
	clear_struct(material);

	load_material_from_create_info(material, gpu, create_info);
	gpu->intf->material__construct(material, gpu);
}

//## public
/**
Constructs a material using a material script file.

@param material The material to construct.
@param gpu The GPU context.
@param filename The file that defines the material.
*/
void gpu_material__construct_from_file(gpu_material_t* material, gpu_t* gpu, const char* filename)
{
	clear_struct(material);

	load_material_from_script(material, gpu, filename);
	gpu->intf->material__construct(material, gpu);
}

//## public
/**
Constructs a material using a material script file.

@param material The material to construct.
@param gpu The GPU context.
@param tinymat The material from the tinyobj lib.
*/
void gpu_material__construct_from_tinyobj(gpu_material_t* material, gpu_t* gpu, tinyobj_material_t* tinymat)
{
	clear_struct(material);

	load_material_from_tinyobj(material, gpu, tinymat);
	gpu->intf->material__construct(material, gpu);
}

//## public
/**
Destructs a material.

@param material The material to destruct.
@param gpu The GPU context.
*/
void gpu_material__destruct(gpu_material_t* material, gpu_t* gpu)
{
	gpu->intf->material__destruct(material, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## static
static void load_material_from_create_info(gpu_material_t* material, gpu_t* gpu, gpu_material_create_info_t* create_info)
{
	material->ambient_color = create_info->ambient_color;
	material->diffuse_color = create_info->diffuse_color;
	material->specular_color = create_info->specular_color;

	material->has_diffuse_texture = create_info->diffuse_texture != NULL;

	if (create_info->diffuse_texture)
	{
		material->diffuse_texture = create_info->diffuse_texture;
	}
	else
	{
		material->diffuse_texture = gpu__get_default_texture(gpu);
	}
}

//## static
static void load_material_from_script(gpu_material_t* material, gpu_t* gpu, const char* filename)
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
		kk_log__fatal("Failed to load material file.");
	}

	/* Find material definition */
	if (lua_script__push(&script, MATERIAL_VAR))
	{
		/* Ambient color */
		lua_script__get_array_of_float_var(&script, AMBIENT_COLOR_VAR, (float*)&material->ambient_color, 3);

		/* Diffuse color */
		lua_script__get_array_of_float_var(&script, DIFFUSE_COLOR_VAR, (float*)&material->diffuse_color, 3);

		/* Specular color */
		lua_script__get_array_of_float_var(&script, SPECULAR_COLOR_VAR, (float*)&material->specular_color, 3);

		/* Diffuse texture */
		lua_script__get_string_var(&script, DIFFUSE_TEXTURE_VAR, material->diffuse_texture_name, sizeof(material->diffuse_texture_name));
	}

	/* Cleanup script */
	lua_script__destruct(&script);

	/* Load textures */
	if (material->diffuse_texture_name[0])
	{
		material->diffuse_texture = gpu__load_texture(gpu, material->diffuse_texture_name);
		material->has_diffuse_texture = TRUE;
	}
	else
	{
		material->diffuse_texture = gpu__get_default_texture(gpu);
		material->has_diffuse_texture = FALSE;
	}
}

//## static
static void load_material_from_tinyobj(gpu_material_t* material, gpu_t* gpu, tinyobj_material_t* tinymat)
{
	material->ambient_color.x = tinymat->ambient[0];
	material->ambient_color.y = tinymat->ambient[1];
	material->ambient_color.z = tinymat->ambient[2];

	material->diffuse_color.x = tinymat->diffuse[0];
	material->diffuse_color.y = tinymat->diffuse[1];
	material->diffuse_color.z = tinymat->diffuse[2];

	material->specular_color.x = tinymat->specular[0];
	material->specular_color.y = tinymat->specular[1];
	material->specular_color.z = tinymat->specular[2];

	if (tinymat->diffuse_texname)
	{
		strcpy_s(material->diffuse_texture_name, cnt_of_array(material->diffuse_texture_name), tinymat->diffuse_texname);
		material->diffuse_texture = gpu__load_texture(gpu, material->diffuse_texture_name);
		material->has_diffuse_texture = TRUE;
	}
	else
	{
		material->diffuse_texture = gpu__get_default_texture(gpu);
		material->has_diffuse_texture = FALSE;
	}
}