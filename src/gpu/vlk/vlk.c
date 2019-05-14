/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "gpu/gpu_intf.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

static void _begin_frame(gpu_type* gpu);
static void _end_frame(gpu_type* gpu);
static void _init(gpu_type* gpu);
static void _render_model(gpu_type* gpu, const vec3_t* pos);
static void _term(gpu_type* gpu);

/**
main
*/
void vlk_init_intf(gpu_type* gpu)
{
	memset(gpu, 0, sizeof(*gpu));
	gpu->begin_frame = &_begin_frame;
	gpu->end_frame = &_end_frame;
	gpu->init = &_init;
	gpu->render_model = &_render_model;
	gpu->term = &_term;
}

static void _begin_frame(gpu_type* gpu)
{

}

static void _end_frame(gpu_type* gpu)
{

}

static void _init(gpu_type* gpu)
{
	_vlk_type* vlk = malloc(sizeof(_vlk_type));
	gpu->context = vlk;

	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
}

static void _render_model(gpu_type* gpu, const vec3_t* pos)
{

}

static void _term(gpu_type* gpu)
{
	_vlk_type* vlk = (_vlk_type*)gpu->context;

	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__create_instance(vlk);
	free(gpu->context);
}
