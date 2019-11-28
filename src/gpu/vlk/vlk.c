/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "gpu/gpu_intf.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "platforms/glfw/glfw.h"
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
static void _render_model(gpu_type* gpu, const vec3_t* pos);
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
	gpu->term = &_term;

	s_glfw_window = window;
}

static void _begin_frame(gpu_type* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_swapchain__begin_frame(&vlk->swapchain);
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
	gpu->context = vlk;
	clear_struct(vlk);
	vlk->window = s_glfw_window;

	_vlk_setup__create_requirement_lists(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
	_vlk_setup__create_surface(vlk);
	_vlk_setup__create_device(vlk);
	_vlk_setup__create_swapchain(vlk);
}

static void _render_model(gpu_type* gpu, gpu_model_t* model, const vec3_t* pos)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

}

static void _term(gpu_type* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	_vlk_setup__destroy_swapchain(vlk);
	_vlk_setup__destroy_device(vlk);
	_vlk_setup__destroy_surface(vlk);
	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_setup__destroy_requirement_lists(vlk);
	free(gpu->context);
}
