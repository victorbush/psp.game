#ifndef GPU_INTF_H
#define GPU_INTF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "engine/camera.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_model.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_texture.h"
#include "platform/platform.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_s gpu_t;

typedef void (*gpu_init_func)(gpu_t* gpu);
typedef void (*gpu_term_func)(gpu_t* gpu);
typedef void (*gpu_begin_frame_func)(gpu_t* gpu, camera_t* cam);
typedef void (*gpu_end_frame_func)(gpu_t* gpu);
typedef void (*gpu_wait_idle_func)(gpu_t* gpu);

typedef void (*gpu_create_model_func)(gpu_t* gpu, gpu_model_t* model);
typedef void (*gpu_destroy_model_func)(gpu_t* gpu, gpu_model_t* model);
typedef void (*gpu_render_model_func)(gpu_t* gpu, gpu_model_t* model, transform_comp_t* transform);
typedef void (*gpu_render_plane_func)(gpu_t* gpu, gpu_plane_t* plane, transform_comp_t* transform);


typedef gpu_static_model_t* (*gpu_load_static_model_func)(gpu_t* gpu, const char* filename);
typedef void (*gpu_unload_static_models_func)(gpu_t* gpu);
typedef void (*gpu_render_static_model_func)(gpu_t* gpu, gpu_static_model_t* model, transform_comp_t* transform);

typedef gpu_texture_t* (*gpu_load_texture_func)(gpu_t* gpu, const char* filename);
typedef void (*gpu_unload_textures_func)(gpu_t* gpu);

typedef gpu_material_t* (*gpu_load_material_func)(gpu_t* gpu, const char* filename);
typedef void (*gpu_unload_materials_func)(gpu_t* gpu);

struct gpu_s
{
	void* context;
	platform_t*				platform;

	gpu_init_func 			init;
	gpu_term_func 			term;
	gpu_begin_frame_func	begin_frame;
	gpu_end_frame_func		end_frame;
	gpu_wait_idle_func		wait_idle;			/* Waits until the GPU has finished executing the current command buffer. */


	gpu_create_model_func		create_model;
	gpu_destroy_model_func		destroy_model;
	gpu_render_model_func		render_model;

	gpu_render_plane_func		render_plane;


	gpu_load_static_model_func		load_static_model;
	gpu_unload_static_models_func	unload_static_models;
	gpu_render_static_model_func	render_static_model;

	gpu_load_texture_func		load_texture;
	gpu_unload_textures_func	unload_textures;

	gpu_load_material_func		load_material;
	gpu_unload_materials_func	unload_materials;
};

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Parses a material file.
*/
boolean gpu__parse_material
	(
	const char*					filename,
	gpu_material_t*				out__material
	);

#endif /* GPU_INTF_H */