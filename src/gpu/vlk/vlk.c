/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "common.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "engine/kk_math.h"
#include "gpu/gpu.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "gpu/gpu_window.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_material.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/models/vlk_static_model.h"
#include "gpu/vlk/models/vlk_static_model.h"
#include "platform/glfw/glfw.h"
#include "thirdparty/cglm/include/cglm/affine.h"
#include "thirdparty/cglm/include/cglm/vec3.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void vlk__construct(gpu_t* gpu);
static void vlk__destruct(gpu_t* gpu);
static void vlk__wait_idle(gpu_t* gpu);
static void vlk_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu);
static void vlk_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu);
static void vlk_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, gpu_frame_t* frame, ecs_transform_t* transform);
static void vlk_plane__construct(gpu_plane_t* plane, gpu_t* gpu);
static void vlk_plane__destruct(gpu_plane_t* plane, gpu_t* gpu);
static void vlk_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material);
static void vlk_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, kk_vec3_t verts[4]);
static void vlk_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, ecs_transform_t* transform);
static void vlk_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);
static void vlk_texture__destruct(gpu_texture_t* texture, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void vlk__init_gpu_intf
	(
	gpu_intf_t*						intf,
	vlk_create_surface_func			create_surface_func,
	vlk_create_temp_surface_func	create_temp_surface_func
	)
{
	clear_struct(intf);

	/* Allocate memory for Vulkan implementation */
	_vlk_t* vlk = malloc(sizeof(_vlk_t));
	if (!vlk)
	{
		kk_log__fatal("Failed to allocate Vulkan context.");
	}

	clear_struct(vlk);
	vlk->enable_validation = TRUE;
	vlk->create_surface_func = create_surface_func;
	vlk->create_temp_surface_func = create_temp_surface_func;

	/* Setup GPU interface */
	intf->impl = vlk;
	intf->__construct = vlk__construct;
	intf->__destruct = vlk__destruct;
	intf->__wait_idle = vlk__wait_idle;
	intf->anim_model__construct = vlk_anim_model__construct;
	intf->anim_model__destruct = vlk_anim_model__destruct;
	intf->anim_model__render = vlk_anim_model__render;
	intf->frame__construct = vlk_frame__construct;
	intf->frame__destruct = vlk_frame__destruct;
	intf->material__construct = vlk_material__construct;
	intf->material__destruct = vlk_material__destruct;
	intf->plane__construct = vlk_plane__construct;
	intf->plane__destruct = vlk_plane__destruct;
	intf->plane__render = vlk_plane__render;
	intf->plane__update_verts = vlk_plane__update_verts;
	intf->static_model__construct = vlk_static_model__construct;
	intf->static_model__destruct = vlk_static_model__destruct;
	intf->static_model__render = vlk_static_model__render;
	intf->texture__construct = vlk_texture__construct;
	intf->texture__destruct = vlk_texture__destruct;
	intf->window__begin_frame = vlk_window__begin_frame;
	intf->window__construct = vlk_window__construct;
	intf->window__destruct = vlk_window__destruct;
	intf->window__end_frame = vlk_window__end_frame;
	intf->window__render_imgui = vlk_window__render_imgui;
	intf->window__resize = vlk_window__resize;
}

_vlk_t* _vlk__from_base(gpu_t* gpu)
{
	return (_vlk_t*)gpu->intf->impl;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

static void vlk__construct(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	vlk->base = gpu;

	_vlk_setup__create_requirement_lists(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
	_vlk_setup__create_device(vlk);
}

static void vlk__destruct(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	_vlk_setup__destroy_device(vlk);
	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__destroy_instance(vlk);
	_vlk_setup__destroy_requirement_lists(vlk);

	/* Free implementation memory */
	free(gpu->intf->impl);
}

static void vlk__wait_idle(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* wait for device to finsih current operations. example usage is at
	application exit - wait until current ops finish, then do cleanup. */
	vkDeviceWaitIdle(vlk->dev.handle);
}

static void vlk_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Allocate data used for the Vulkan GPU implementation */
	model->data = malloc(sizeof(_vlk_anim_model_t));
	if (!model->data)
	{
		kk_log__fatal("Failed to allocate memory for model.");
	}

	/* Initialize GPU data */
	_vlk_anim_model__construct((_vlk_anim_model_t*)model->data, &vlk->dev, &model->md5);
}

static void vlk_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Free GPU data */
	_vlk_anim_model__destruct((_vlk_anim_model_t*)model->data);
	free(model->data);
}

static void vlk_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, gpu_frame_t* frame, ecs_transform_t* transform)
{
	//_vlk_t* vlk = _vlk__from_base(window->gpu);
	//_vlk_window_t* vlk_window = (_vlk_window_t*)window->data;
	//_vlk_frame_t* frame = &vlk_window->swapchain.frame;

	///* Bind pipeline */
	//_vlk_md5_pipeline__bind(&vlk->md5_pipeline, frame->cmd_buf);

	//// TODO : Modelview, push constants, etc.

	///* Render the model */
	//_vlk_anim_model__render((_vlk_anim_model_t*)model->data, frame->cmd_buf, transform);
}

static void vlk_plane__construct(gpu_plane_t* plane, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Allocate data used for the Vulkan implementation */
	plane->data = malloc(sizeof(_vlk_plane_t));
	if (!plane->data)
	{
		kk_log__fatal("Failed to allocate memory for material.");
	}

	_vlk_plane__construct((_vlk_plane_t*)plane->data, &vlk->dev);
}

static void vlk_plane__destruct(gpu_plane_t* plane, gpu_t* gpu)
{
	_vlk_plane__destruct((_vlk_plane_t*)plane->data);
}

static void vlk_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material)
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);

	/* Bind the plane pipeline */
	_vlk_plane_pipeline__bind(&vlk_window->plane_pipeline, vlk_frame->cmd_buf);

	/* Bind per-view descriptor set */
	_vlk_per_view_set__bind(&vlk_window->per_view_set, vlk_frame->cmd_buf, vlk_frame, vlk_window->plane_pipeline.layout);

	/* Bind material descriptor set */
	//_vlk_material_t* mat = (_vlk_material_t*)material->data;
	//_vlk_material_set__bind(&mat->descriptor_set, vlk_frame, vlk_window->plane_pipeline.layout);

	/* Update push constants */
	_vlk_plane_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);

	uint32_t pcVertSize = sizeof(_vlk_plane_push_constant_vertex_t);
	vkCmdPushConstants(vlk_frame->cmd_buf, vlk_window->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);

	/* Draw the plane */
	_vlk_plane__render((_vlk_plane_t*)plane->data, vlk_frame->cmd_buf);
}

static void vlk_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, kk_vec3_t verts[4])
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	_vlk_plane__update_verts((_vlk_plane_t*)plane->data, verts);
}

static void vlk_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, ecs_transform_t* transform)
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_static_model_t* vlk_model = (_vlk_static_model_t*)model->data;

	/* Bind pipeline */
	_vlk_obj_pipeline__bind(&vlk_window->obj_pipeline, vlk_frame->cmd_buf);

	/* Bind per-view descriptor set */
	_vlk_per_view_set__bind(&vlk_window->per_view_set, vlk_frame->cmd_buf, vlk_frame, vlk_window->obj_pipeline.layout);

	/* Bind material descriptor set */
	_vlk_material_set__bind(&vlk_model->material_set, vlk_frame, vlk_window->obj_pipeline.layout);

	/* Update push constants */
	_vlk_obj_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);
	glm_translate(&pc.vertex.model_matrix, &transform->pos);

	kk_vec3_t axis;
	float angle = glm_quat_angle(&transform->rot);
	glm_quat_axis(&transform->rot, &axis);
	glm_rotate(&pc.vertex.model_matrix, angle, &axis);

	uint32_t pcVertSize = sizeof(_vlk_obj_push_constant_vertex_t);

	vkCmdPushConstants(vlk_frame->cmd_buf, vlk_window->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);

	/* Render the model */
	_vlk_static_model__render(vlk_model, vlk_frame->cmd_buf);
}

//void vlk_static_model__render_picker_buffer(gpu_static_model_t* model, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, vec3_t id_color)
//{
//	_vlk_t* vlk = _vlk__from_base(gpu);
//	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
//	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
//
//	/* Bind pipeline */
//
//	_vlk_obj_pipeline__bind(&vlk_window->obj_pipeline, vlk_frame->cmd_buf);
//
//}

void vlk_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height)
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	
	/* Allocate data used for the Vulkan implementation */
	texture->data = malloc(sizeof(_vlk_texture_t));
	if (!texture->data)
	{
		kk_log__fatal("Failed to allocate memory for texture.");
	}

	// TODO : width/height uint32_t


	_vlk_texture_create_info_t tex_create_info;
	clear_struct(&tex_create_info);
	tex_create_info.data = img;
	tex_create_info.width = (uint32_t)width;
	tex_create_info.height = (uint32_t)height;

	// TODO : Size
	tex_create_info.size = width * height * sizeof(uint32_t);

	_vlk_texture__construct((_vlk_texture_t*)texture->data, &vlk->dev, &tex_create_info);
}

void vlk_texture__destruct(gpu_texture_t* texture, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Free GPU data */
	_vlk_texture__destruct((_vlk_texture_t*)texture->data);
	free(texture->data);
}
