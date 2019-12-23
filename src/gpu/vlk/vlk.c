/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "common.h"
#include "ecs/components.h"
#include "gpu/gpu.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "platform/glfw/glfw.h"
#include "thirdparty/cglm/include/cglm/affine.h"
#include "thirdparty/cglm/include/cglm/vec3.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void vlk__begin_frame(gpu_t* gpu, camera_t* cam);
static void vlk__construct(gpu_t* gpu);
static void vlk__destruct(gpu_t* gpu);
static void vlk__end_frame(gpu_t* gpu);
static void vlk__wait_idle(gpu_t* gpu);
static void vlk_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu);
static void vlk_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu);
static void vlk_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, transform_comp_t* transform);
static void vlk_material__construct(gpu_material_t* material, gpu_t* gpu);
static void vlk_material__destruct(gpu_material_t* material);
static void vlk_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform);
static void vlk_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj);
static void vlk_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu);
static void vlk_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform);
static void vlk_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);
static void vlk_texture__destruct(gpu_texture_t* texture, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void vlk__init_gpu_intf(gpu_intf_t* intf, GLFWwindow* window)
{
	clear_struct(intf);

	/* Allocate memory for Vulkan implementation */
	_vlk_t* vlk = malloc(sizeof(_vlk_t));
	if (!vlk)
	{
		FATAL("Failed to allocate Vulkan context.");
	}

	clear_struct(vlk);
	vlk->window = window;
	vlk->enable_validation = TRUE;

	/* Setup GPU interface */
	intf->impl = vlk;
	intf->__begin_frame = vlk__begin_frame;
	intf->__construct = vlk__construct;
	intf->__destruct = vlk__destruct;
	intf->__end_frame = vlk__end_frame;
	intf->__wait_idle = vlk__wait_idle;
	intf->anim_model__construct = vlk_anim_model__construct;
	intf->anim_model__destruct = vlk_anim_model__destruct;
	intf->anim_model__render = vlk_anim_model__render;
	intf->material__construct = vlk_material__construct;
	intf->material__destruct = vlk_material__destruct;
	intf->plane__render = vlk_plane__render;
	intf->static_model__construct = vlk_static_model__construct;
	intf->static_model__destruct = vlk_static_model__destruct;
	intf->static_model__render = vlk_static_model__render;
	intf->texture__construct = vlk_texture__construct;
	intf->texture__destruct = vlk_texture__destruct;
}

_vlk_t* _vlk__get_context(gpu_t* gpu)
{
	return (_vlk_t*)gpu->intf->impl;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

static void vlk__begin_frame(gpu_t* gpu, camera_t* cam)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_swapchain__begin_frame(&vlk->swapchain, vlk);

	/* Get current frame data */
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk->dev.per_view_set, frame, cam, vlk->swapchain.extent);
	_vlk_per_view_set__bind(&vlk->dev.per_view_set, frame, vlk->plane_pipeline.layout);
	_vlk_per_view_set__bind(&vlk->dev.per_view_set, frame, vlk->obj_pipeline.layout);
}

static void vlk__construct(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	_vlk_setup__create_requirement_lists(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
	_vlk_setup__create_surface(vlk);
	_vlk_setup__create_device(vlk);
	_vlk_setup__create_swapchain(vlk);
	_vlk_setup__create_pipelines(vlk);
}

static void vlk__destruct(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	_vlk_setup__destroy_pipelines(vlk);
	_vlk_setup__destroy_swapchain(vlk);
	_vlk_setup__destroy_device(vlk);
	_vlk_setup__destroy_surface(vlk);
	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_setup__destroy_requirement_lists(vlk);

	/* Free implementation memory */
	free(gpu->intf->impl);
}

static void vlk__end_frame(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_swapchain__end_frame(&vlk->swapchain);
}

static void vlk__wait_idle(gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* wait for device to finsih current operations. example usage is at
	application exit - wait until current ops finish, then do cleanup. */
	vkDeviceWaitIdle(vlk->dev.handle);
}

static void vlk_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* Allocate data used for the Vulkan GPU implementation */
	model->data = malloc(sizeof(_vlk_anim_model_t));
	if (!model->data)
	{
		FATAL("Failed to allocate memory for model.");
	}

	/* Initialize GPU data */
	_vlk_anim_model__construct((_vlk_anim_model_t*)model->data, &vlk->dev, &model->md5);
}

static void vlk_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* Free GPU data */
	_vlk_anim_model__destruct((_vlk_anim_model_t*)model->data);
	free(model->data);
}

static void vlk_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, transform_comp_t* transform)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind pipeline */
	_vlk_md5_pipeline__bind(&vlk->md5_pipeline, frame->cmd_buf);

	// TODO : Modelview, push constants, etc.

	/* Render the model */
	_vlk_anim_model__render((_vlk_anim_model_t*)model->data, frame->cmd_buf, transform);
}

static void vlk_material__construct(gpu_material_t* material, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_material_ubo_t ubo;

	/* Allocate data used for the Vulkan implementation */
	material->data = malloc(sizeof(_vlk_material_t));
	if (!material->data)
	{
		FATAL("Failed to allocate memory for material.");
	}

	/* Setup material UBO */
	ubo.ambient = material->ambient_color;
	ubo.diffuse = material->diffuse_color;
	ubo.specular = material->specular_color;

	_vlk_texture_t* diffuse_tex = (_vlk_texture_t*)material->diffuse_texture->data;

	/* Construct */
	_vlk_material__construct((_vlk_material_t*)material->data, &ubo, &vlk->dev.material_layout, diffuse_tex);
}

static void vlk_material__destruct(gpu_material_t* material)
{
	/* Free Vulkan implementation memory */
	_vlk_material__destruct((_vlk_material_t*)material->data);
	free(material->data);
}

static void vlk_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind the plane pipeline */
	_vlk_plane_pipeline__bind(&vlk->plane_pipeline, frame->cmd_buf);

	/* Update push constants */
	_vlk_plane_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);
	glm_translate(&pc.vertex.model_matrix, &transform->pos);
	glm_vec3_copy(&plane->color, &pc.fragment.color);
	pc.vertex.anchor.x = plane->anchor.x;
	pc.vertex.anchor.y = plane->anchor.y;
	pc.vertex.height = plane->height;
	pc.vertex.width = plane->width;

	uint32_t pcVertSize = sizeof(_vlk_plane_push_constant_vertex_t);
	uint32_t pcFragSize = sizeof(_vlk_plane_push_constant_fragment_t);
	uint32_t pcFragOffset = offsetof(_vlk_plane_push_constant_t, fragment);

	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);
	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_FRAGMENT_BIT, pcFragOffset, pcFragSize, &pc.fragment);

	/* Draw the plane */
	vkCmdDrawIndexed(frame->cmd_buf, 6, 1, 0, 0, 0);
}

static void vlk_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* Allocate data used for the Vulkan implementation */
	model->data = malloc(sizeof(_vlk_static_model_t));
	if (!model->data)
	{
		FATAL("Failed to allocate memory for static model.");
	}

	/* Construct */
	_vlk_static_model__construct((_vlk_static_model_t*)model->data, &vlk->dev, obj);
}

static void vlk_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* Free GPU data */
	_vlk_static_model__destruct((gpu_static_model_t*)model->data);
	free(model->data);
}

static void vlk_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind pipeline */
	_vlk_obj_pipeline__bind(&vlk->obj_pipeline, frame->cmd_buf);

	/* Update push constants */
	_vlk_obj_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);
	glm_translate(&pc.vertex.model_matrix, &transform->pos);

	uint32_t pcVertSize = sizeof(_vlk_obj_push_constant_vertex_t);

	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);

	/* Bind material descriptor set */
	_vlk_material_t* mat = (_vlk_material_t*)material->data;
	_vlk_material_set__bind(&mat->descriptor_set, frame, vlk->obj_pipeline.layout);

	/* Render the model */
	_vlk_static_model__render((_vlk_static_model_t*)model->data, frame->cmd_buf);
}

void vlk_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height)
{
	_vlk_t* vlk = _vlk__get_context(gpu);
	
	/* Allocate data used for the Vulkan implementation */
	texture->data = malloc(sizeof(_vlk_texture_t));
	if (!texture->data)
	{
		FATAL("Failed to allocate memory for texture.");
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
	_vlk_t* vlk = _vlk__get_context(gpu);

	/* Free GPU data */
	_vlk_texture__destruct((_vlk_texture_t*)texture->data);
	free(texture->data);
}
