#ifndef GPU_INTF_H
#define GPU_INTF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "engine/camera.h"
#include "platform/platform.h"
#include "thirdparty/rxi_map/src/map.h"
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_anim_model_s gpu_anim_model_t;
typedef struct gpu_material_s gpu_material_t;
typedef struct gpu_plane_s gpu_plane_t;
typedef struct gpu_static_model_s gpu_static_model_t;
typedef struct gpu_texture_s gpu_texture_t;

typedef struct gpu_s gpu_t;
typedef struct gpu_intf_s gpu_intf_t;

typedef void (*gpu_begin_frame_func)(gpu_t* gpu, camera_t* cam);
typedef void (*gpu_construct_func)(gpu_t* gpu);
typedef void (*gpu_destruct_func)(gpu_t* gpu);
typedef void (*gpu_end_frame_func)(gpu_t* gpu);
typedef void (*gpu_wait_idle_func)(gpu_t* gpu);

typedef void (*gpu_plane_construct_func)(gpu_plane_t* plane, gpu_t* gpu);
typedef void (*gpu_plane_destruct_func)(gpu_plane_t* plane, gpu_t* gpu);
typedef void (*gpu_plane_render_func)(gpu_plane_t* plane, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform);

typedef void (*gpu_anim_model_construct_func)(gpu_anim_model_t* model, gpu_t* gpu);
typedef void (*gpu_anim_model_destruct_func)(gpu_anim_model_t* model, gpu_t* gpu);
typedef void (*gpu_anim_model_render_func)(gpu_anim_model_t* model, gpu_t* gpu, transform_comp_t* transform);

typedef void (*gpu_material_construct_func)(gpu_material_t* material, gpu_t* gpu);
typedef void (*gpu_material_destruct_func)(gpu_material_t* material, gpu_t* gpu);

typedef void (*gpu_static_model_construct_func)(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj);
typedef void (*gpu_static_model_destruct_func)(gpu_static_model_t* model, gpu_t* gpu);
typedef void (*gpu_static_model_render_func)(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform);

typedef void (*gpu_texture_construct_func)(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);
typedef void (*gpu_texture_destruct_func)(gpu_texture_t* texture, gpu_t* gpu);

struct gpu_intf_s
{
	void*							impl;				/* Memory used by the GPU implementation. */

	gpu_begin_frame_func			__begin_frame;
	gpu_construct_func 				__construct;
	gpu_destruct_func 				__destruct;
	gpu_end_frame_func				__end_frame;
	gpu_wait_idle_func				__wait_idle;		/* Waits until the GPU has finished executing the current command buffer. */

	gpu_anim_model_construct_func	anim_model__construct;
	gpu_anim_model_destruct_func	anim_model__destruct;
	gpu_anim_model_render_func		anim_model__render;
	gpu_material_construct_func		material__construct;
	gpu_material_destruct_func		material__destruct;
	gpu_plane_construct_func		plane__construct;
	gpu_plane_destruct_func			plane__destruct;
	gpu_plane_render_func			plane__render;
	gpu_static_model_construct_func	static_model__construct;
	gpu_static_model_destruct_func	static_model__destruct;
	gpu_static_model_render_func	static_model__render;
	gpu_texture_construct_func		texture__construct;
	gpu_texture_destruct_func		texture__destruct;
};

struct gpu_s
{
	gpu_intf_t*					intf;				/* Interface that implements GPU functions. */

	map_t(gpu_material_t*)		materials;			/* Material chache. */
	map_t(gpu_static_model_t*)	static_models;		/* Static model cache. */
	map_t(gpu_texture_t*)		textures;			/* Texture cache. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a GPU.

@param gpu The GPU context to construct.
@param intf The GPU interface to use.
*/
void gpu__construct(gpu_t* gpu, gpu_intf_t* intf);

/**
Destructs a GPU.
*/
void gpu__destruct(gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Begins a render frame.
*/
void gpu__begin_frame(gpu_t* gpu, camera_t* cam);

/**
Ends a render frame.
*/
void gpu__end_frame(gpu_t* gpu);

/**
Waits until the GPU has finished executing the current command buffer.
*/
void gpu__wait_idle(gpu_t* gpu);

/**
Returns the specified animated model, loading it if needed.

@param gpu The GPU context.
@param filename The model file to load.
@returns The loaded model if found, NULL otherwise.
*/
gpu_anim_model_t* gpu__load_anim_model(gpu_t* gpu, const char* filename);

/**
Returns the specified material, loading it if needed.

@param gpu The GPU context.
@param filename The material file to load.
@returns The loaded material if found, NULL otherwise.
*/
gpu_material_t* gpu__load_material(gpu_t* gpu, const char* filename);

/**
Returns the specified static model, loading it if needed.

@param gpu The GPU context.
@param filename The model file to load.
@returns The loaded model if found, NULL otherwise.
*/
gpu_static_model_t* gpu__load_static_model(gpu_t* gpu, const char* filename);

/**
Returns the specified texture, loading it if needed.

@param gpu The GPU context.
@param filename The texture file to load.
@returns The loaded texture if found, NULL otherwise.
*/
gpu_texture_t* gpu__load_texture(gpu_t* gpu, const char* filename);

#endif /* GPU_INTF_H */