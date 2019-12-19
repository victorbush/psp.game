/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_static_model.h"
#include "lua/lua_script.h"
#include "thirdparty/rxi_map/src/map.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Frees all static models in the cache. */
static void unload_static_models(gpu_t* gpu);

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
}

void gpu__destruct(gpu_t* gpu)
{
	gpu->intf->__wait_idle(gpu);
	unload_static_models(gpu);
	gpu->intf->__destruct(gpu);

	map_deinit(&gpu->materials);
	map_deinit(&gpu->static_models);
	map_deinit(&gpu->textures);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu__begin_frame(gpu_t* gpu, camera_t* cam)
{
	gpu->intf->__begin_frame(gpu, cam);
}

void gpu__end_frame(gpu_t* gpu)
{
	gpu->intf->__end_frame(gpu);
}

void gpu__wait_idle(gpu_t* gpu)
{
	gpu->intf->__wait_idle(gpu);
}

gpu_anim_model_t* gpu__load_anim_model(gpu_t* gpu, const char* filename)
{
	assert(FALSE); //TODO
	return NULL;
}

gpu_material_t* gpu__load_material(gpu_t* gpu, const char* filename)
{
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
		FATAL("Failed to allocate memory for material.");
	}

	/* Construct material */
	gpu_material__construct(material, gpu, filename);

	/* Register the material in the cache */
	if (map_set(&gpu->materials, filename, material))
	{
		FATAL("Failed to register material in cache.");
	}

	return material;
}

gpu_static_model_t* gpu__load_static_model(gpu_t* gpu, const char* filename)
{
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
		FATAL("Failed to allocate memory for static model.");
	}

	/* Construct model */
	gpu_static_model__construct(model, gpu, filename);

	/* Register the model in the cache */
	if (map_set(&gpu->static_models, filename, model))
	{
		FATAL("Failed to register static model in cache.");
	}
	
	return model;
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