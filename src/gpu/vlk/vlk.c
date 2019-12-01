/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "gpu/gpu_intf.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/vlk_prv_plane.h"
#include "platforms/glfw/glfw.h"
#include "thirdparty/cglm/include/cglm/vec3.h"
#include "thirdparty/md5/md5model.h"

/*=========================================================
VARIABLES
=========================================================*/

static GLFWwindow* s_glfw_window;

/*=========================================================
FUNCTIONS
=========================================================*/

static void _begin_frame(gpu_type* gpu);
static void _create_model(gpu_type* gpu, gpu_model_t* model);
static void _destroy_model(gpu_type* gpu, gpu_model_t* model);
static void _end_frame(gpu_type* gpu);
static void _init(gpu_type* gpu);
static void _render_model(gpu_type* gpu, gpu_model_t* model, const vec3_t* pos);
static void _render_plane(gpu_type* gpu, gpu_plane_t* plane, const mat4_t* model_matrix);
static void _term(gpu_type* gpu);

/**
main
*/
void vlk_init_intf(gpu_type* gpu, GLFWwindow* window)
{
	memset(gpu, 0, sizeof(*gpu));
	gpu->begin_frame = &_begin_frame;
	gpu->create_model = &_create_model;
	gpu->destroy_model = &_destroy_model;
	gpu->end_frame = &_end_frame;
	gpu->init = &_init;
	gpu->render_model = &_render_model;
	gpu->render_plane = &_render_plane;
	gpu->term = &_term;

	s_glfw_window = window;
}

static void _begin_frame(gpu_type* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_swapchain__begin_frame(&vlk->swapchain, vlk);

	_vlk_frame_t* frame = &vlk->swapchain.frame;

	// TODO : Where to handle camera?
	camera_t cam;
	camera__construct(&cam);

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk->dev.per_view_set, frame, &cam, vlk->swapchain.extent);
	_vlk_per_view_set__bind(&vlk->dev.per_view_set, frame, vlk->plane_pipeline.layout);
}

static void _create_model(gpu_type* gpu, gpu_model_t* model)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	ReadMD5Model("C:\\Users\\Victor\\Desktop\\bob_lamp_update\\bob_lamp_update.md5mesh",
		&model->mdl);
}

static void _destroy_model(gpu_type* gpu, gpu_model_t* model)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	FreeModel(&model->mdl);
}

static void _end_frame(gpu_type* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_swapchain__end_frame(&vlk->swapchain);
}

static void _init(gpu_type* gpu)
{
	_vlk_t* vlk = malloc(sizeof(_vlk_t));
	if (!vlk)
	{
		FATAL("Failed to allocate Vulkan context.");
	}

	gpu->context = vlk;
	clear_struct(vlk);
	vlk->window = s_glfw_window;
	vlk->enable_validation = TRUE;

	_vlk_setup__create_requirement_lists(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
	_vlk_setup__create_surface(vlk);
	_vlk_setup__create_device(vlk);
	_vlk_setup__create_swapchain(vlk);
	_vlk_setup__create_pipelines(vlk);
}

static void _render_model(gpu_type* gpu, gpu_model_t* model, const vec3_t* pos)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

}

static void _render_plane(gpu_type* gpu, gpu_plane_t* plane, const mat4_t* model_matrix)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind the plane pipeline */
	_vlk_plane_pipeline__bind(&vlk->plane_pipeline, frame->cmd_buf);

	/* Update push constants */
	vlk_plane_push_constant pc;
	clear_struct(&pc);

	glm_mat4_copy(model_matrix, &pc.Vertex.ModelMatrix);
	glm_vec3_copy(&plane->color, &pc.Fragment.Color);
	pc.Vertex.Anchor.x = plane->anchor.x;
	pc.Vertex.Anchor.y = plane->anchor.y;
	pc.Vertex.Height = plane->height;
	pc.Vertex.Width = plane->width;

	uint32_t pcVertSize = sizeof(vlk_plane_push_constant_vertex);
	uint32_t pcFragSize = sizeof(vlk_plane_push_constant_fragment);
	uint32_t pcFragOffset = offsetof(vlk_plane_push_constant, Fragment);

	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.Vertex);
	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_FRAGMENT_BIT, pcFragOffset, pcFragSize, &pc.Fragment);

	/* Draw the plane */
	vkCmdDrawIndexed(frame->cmd_buf, 6, 1, 0, 0, 0);
}

static void _term(gpu_type* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	/* wait for device to finsih current operations. example usage is at
    application exit - wait until current ops finish, then do cleanup. */
    vkDeviceWaitIdle(vlk->dev.handle);

	_vlk_setup__destroy_pipelines(vlk);
	_vlk_setup__destroy_swapchain(vlk);
	_vlk_setup__destroy_device(vlk);
	_vlk_setup__destroy_surface(vlk);
	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_setup__destroy_requirement_lists(vlk);
	free(gpu->context);
}
