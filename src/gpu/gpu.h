#ifndef GPU_H
#define GPU_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/kk_camera_.h"
#include "ecs/components/ecs_transform_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_anim_model_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_plane_.h"
#include "gpu/gpu_static_model_.h"
#include "gpu/gpu_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_texture.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/rxi_map/src/map.h"
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
TYPES
=========================================================*/

typedef void (*gpu_construct_func)(gpu_t* gpu);
typedef void (*gpu_destruct_func)(gpu_t* gpu);
typedef void (*gpu_wait_idle_func)(gpu_t* gpu);

typedef void (*gpu_frame_construct_func)(gpu_frame_t* frame, gpu_t* gpu);
typedef void (*gpu_frame_destruct_func)(gpu_frame_t* frame, gpu_t* gpu);

typedef void (*gpu_anim_model_construct_func)(gpu_anim_model_t* model, gpu_t* gpu);
typedef void (*gpu_anim_model_destruct_func)(gpu_anim_model_t* model, gpu_t* gpu);
typedef void (*gpu_anim_model_render_func)(gpu_anim_model_t* model, gpu_t* gpu, gpu_frame_t* frame, ecs_transform_t* transform);

typedef void (*gpu_material_construct_func)(gpu_material_t* material, gpu_t* gpu);
typedef void (*gpu_material_destruct_func)(gpu_material_t* material, gpu_t* gpu);

typedef void (*gpu_plane_construct_func)(gpu_plane_t* plane, gpu_t* gpu);
typedef void (*gpu_plane_destruct_func)(gpu_plane_t* plane, gpu_t* gpu);
typedef void (*gpu_plane_render_func)(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material);
typedef void (*gpu_plane_update_verts_func)(gpu_plane_t* plane, gpu_t* gpu, kk_vec3_t verts[4]);

typedef void (*gpu_static_model_construct_func)(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj);
typedef void (*gpu_static_model_destruct_func)(gpu_static_model_t* model, gpu_t* gpu);
typedef void (*gpu_static_model_render_func)(gpu_static_model_t* model, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, ecs_transform_t* transform);

typedef void (*gpu_texture_construct_func)(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);
typedef void (*gpu_texture_destruct_func)(gpu_texture_t* texture, gpu_t* gpu);

typedef void (*gpu_window_begin_frame_func)(gpu_window_t* window, gpu_frame_t* frame, kk_camera_t* camera);
typedef void (*gpu_window_construct_func)(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height);
typedef void (*gpu_window_destruct_func)(gpu_window_t* window, gpu_t* gpu);
typedef void (*gpu_window_end_frame_func)(gpu_window_t* window, gpu_frame_t* frame);
typedef void (*gpu_window_render_imgui_func)(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data);
typedef void (*gpu_window_resize_func)(gpu_window_t* window, uint32_t width, uint32_t height);

struct gpu_intf_s
{
	void*							impl;				/* Memory used by the GPU implementation. */

	gpu_construct_func 				__construct;
	gpu_destruct_func 				__destruct;
	gpu_wait_idle_func				__wait_idle;		/* Waits until the GPU has finished executing the current command buffer. */

	gpu_anim_model_construct_func	anim_model__construct;
	gpu_anim_model_destruct_func	anim_model__destruct;
	gpu_anim_model_render_func		anim_model__render;
	gpu_frame_construct_func		frame__construct;
	gpu_frame_destruct_func			frame__destruct;
	gpu_material_construct_func		material__construct;
	gpu_material_destruct_func		material__destruct;
	gpu_plane_construct_func		plane__construct;
	gpu_plane_destruct_func			plane__destruct;
	gpu_plane_render_func			plane__render;
	gpu_plane_update_verts_func		plane__update_verts;
	gpu_static_model_construct_func	static_model__construct;
	gpu_static_model_destruct_func	static_model__destruct;
	gpu_static_model_render_func	static_model__render;
	gpu_texture_construct_func		texture__construct;
	gpu_texture_destruct_func		texture__destruct;
	gpu_window_begin_frame_func		window__begin_frame;
	gpu_window_construct_func		window__construct;
	gpu_window_destruct_func		window__destruct;
	gpu_window_end_frame_func		window__end_frame;
	gpu_window_render_imgui_func	window__render_imgui;
	gpu_window_resize_func			window__resize;
};

struct gpu_s
{
	gpu_intf_t*					intf;				/* Interface that implements GPU functions. */

	map_t(gpu_material_t*)		materials;			/* Material chache. */
	map_t(gpu_static_model_t*)	static_models;		/* Static model cache. */
	map_t(gpu_texture_t*)		textures;			/* Texture cache. */

	gpu_material_t				default_material;
	gpu_texture_t				default_texture;
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
Gets a default placeholder material.
*/
gpu_material_t* gpu__get_default_material(gpu_t* gpu);

/**
Gets a default placeholder texture.
*/
gpu_texture_t* gpu__get_default_texture(gpu_t* gpu);

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

/**
Waits until the GPU has finished executing the current command buffer.
*/
void gpu__wait_idle(gpu_t* gpu);

#endif /* GPU_H */