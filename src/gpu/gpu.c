/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_static_model.h"
#include "gpu/gpu_texture.h"
#include "lua/lua_script.h"
#include "thirdparty/rxi_map/src/map.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Frees all materials in the cache. */
static void unload_materials(gpu_t* gpu);

/** Frees all static models in the cache. */
static void unload_static_models(gpu_t* gpu);

/** Frees all textures in the cache. */
static void unload_textures(gpu_t* gpu);

static void create_default_material(gpu_t* gpu);
static void create_default_texture(gpu_t* gpu);
static void destroy_default_material(gpu_t* gpu);
static void destroy_default_texture(gpu_t* gpu);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu__construct(gpu_t* gpu, gpu_intf_t* intf)
{
	clear_struct(gpu);
	gpu->intf = intf;

	map_init(&gpu->materials);
	map_init(&gpu->static_models);
	map_init(&gpu->textures);

	gpu->intf->__construct(gpu);

	create_default_texture(gpu);
	create_default_material(gpu);
}

void gpu__destruct(gpu_t* gpu)
{
	gpu->intf->__wait_idle(gpu);
	destroy_default_material(gpu);
	destroy_default_texture(gpu);
	unload_materials(gpu);
	unload_static_models(gpu);
	unload_textures(gpu);
	gpu->intf->__destruct(gpu);

	map_deinit(&gpu->materials);
	map_deinit(&gpu->static_models);
	map_deinit(&gpu->textures);
}

/*=========================================================
FUNCTIONS
=========================================================*/

gpu_material_t* gpu__get_default_material(gpu_t* gpu)
{
	return &gpu->default_material;
}

gpu_texture_t* gpu__get_default_texture(gpu_t* gpu)
{
	return &gpu->default_texture;
}

gpu_anim_model_t* gpu__load_anim_model(gpu_t* gpu, const char* filename)
{
	assert(FALSE); //TODO
	return NULL;
}

gpu_material_t* gpu__load_material(gpu_t* gpu, const char* filename)
{
	kk_log__dbg_fmt("gpu__load_material: %s", filename);

	gpu_material_t** cached_material = NULL;

	/* Check if material is already in cache */
	cached_material = map_get(&gpu->materials, filename);
	if (cached_material)
	{
		return *cached_material;
	}

	/* Allocate memory for the material */
	gpu_material_t* material = malloc(sizeof(gpu_material_t));
	if (!material)
	{
		kk_log__fatal("Failed to allocate memory for material.");
	}

	/* Construct material */
	gpu_material__construct_from_file(material, gpu, filename);

	/* Register the material in the cache */
	if (map_set(&gpu->materials, filename, material))
	{
		kk_log__fatal("Failed to register material in cache.");
	}

	return material;
}

gpu_static_model_t* gpu__load_static_model(gpu_t* gpu, const char* filename)
{
	kk_log__dbg_fmt("gpu__load_static_model: %s", filename);

	gpu_static_model_t** cached_model = NULL;

	/* Check if model is already in cache */
	cached_model = map_get(&gpu->static_models, filename);
	if (cached_model)
	{
		return *cached_model;
	}

	/* Allocate memory for the model */
	gpu_static_model_t* model = malloc(sizeof(gpu_static_model_t));
	if (!model)
	{
		kk_log__fatal("Failed to allocate memory for static model.");
	}

	/* Construct model */
	gpu_static_model__construct(model, gpu, filename);

	/* Register the model in the cache */
	if (map_set(&gpu->static_models, filename, model))
	{
		kk_log__fatal("Failed to register static model in cache.");
	}
	
	return model;
}

gpu_texture_t* gpu__load_texture(gpu_t* gpu, const char* filename)
{
	kk_log__dbg_fmt("gpu__load_texture: %s", filename);

	gpu_texture_t** cached_texture = NULL;

	/* Check if texture is already in cache */
	cached_texture = map_get(&gpu->textures, filename);
	if (cached_texture)
	{
		return *cached_texture;
	}

	/* Allocate memory for the texture */
	gpu_texture_t* tex = malloc(sizeof(gpu_texture_t));
	if (!tex)
	{
		kk_log__fatal("Failed to allocate memory for texture.");
	}

	/* Construct texture */
	gpu_texture__construct(tex, gpu, filename);

	/* Register the model in the cache */
	if (map_set(&gpu->textures, filename, tex))
	{
		kk_log__fatal("Failed to register texture in cache.");
	}

	return tex;
}

void gpu__wait_idle(gpu_t* gpu)
{
	gpu->intf->__wait_idle(gpu);
}

static void create_default_material(gpu_t* gpu)
{
	kk_log__dbg("gpu::create_default_material");

	gpu_material_create_info_t create_info;
	clear_struct(&create_info);
	
	create_info.ambient_color.x = 1.0f;
	create_info.ambient_color.y = 1.0f;
	create_info.ambient_color.z = 1.0f;

	create_info.diffuse_color.x = 1.0f;
	create_info.diffuse_color.y = 1.0f;
	create_info.diffuse_color.z = 1.0f;

	create_info.diffuse_texture = NULL;

	gpu_material__construct(&gpu->default_material, gpu, &create_info);
}

static void create_default_texture(gpu_t* gpu)
{
	kk_log__dbg("gpu::create_default_texture");

	unsigned char bmp_data[] =
	{
		120, 135, 245, 255,
		100, 255, 188, 255,
		120, 135, 245, 255,
		100, 255, 188, 255,
	};

	gpu_texture__construct_from_data(&gpu->default_texture, gpu, bmp_data, 2, 2);
}

static void destroy_default_material(gpu_t* gpu)
{
	gpu_material__destruct(&gpu->default_material, gpu);
}

static void destroy_default_texture(gpu_t* gpu)
{
	gpu_texture__destruct(&gpu->default_texture, gpu);
}

static void unload_materials(gpu_t* gpu)
{
	const char* key;
	map_iter_t iter = map_iter(&gpu->materials);

	while ((key = map_next(&gpu->materials, &iter)))
	{
		/* The map returns a pointer to the value, so need a double pointer here */
		gpu_material_t** material = (gpu_material_t**)map_get(&gpu->materials, key);
		if (!material)
		{
			continue;
		}

		/* Destruct */
		gpu_material__destruct(*material, gpu);

		/* Free material data */
		free((*material));
	}

	/* Clear cache */
	map_deinit(&gpu->materials);
	map_init(&gpu->materials);
}

static void unload_static_models(gpu_t* gpu)
{
	const char* key;
	map_iter_t iter = map_iter(&gpu->static_models);

	while ((key = map_next(&gpu->static_models, &iter)))
	{
		/* The map returns a pointer to the value, so need a double pointer here */
		gpu_static_model_t** model = (gpu_static_model_t**)map_get(&gpu->static_models, key);
		if (!model)
		{
			continue;
		}

		/* Destruct */
		gpu_static_model__destruct(*model, gpu);

		/* Free model data */
		free((*model));
	}

	/* Clear cache */
	map_deinit(&gpu->static_models);
	map_init(&gpu->static_models);
}

static void unload_textures(gpu_t* gpu)
{
	const char* key;
	map_iter_t iter = map_iter(&gpu->textures);

	while ((key = map_next(&gpu->textures, &iter)))
	{
		/* The map returns a pointer to the value, so need a double pointer here */
		gpu_texture_t** tex = (gpu_texture_t**)map_get(&gpu->textures, key);
		if (!tex)
		{
			continue;
		}

		/* Destruct */
		gpu_texture__destruct(*tex, gpu);

		/* Free model data */
		free((*tex));
	}

	/* Clear cache */
	map_deinit(&gpu->textures);
	map_init(&gpu->textures);
}