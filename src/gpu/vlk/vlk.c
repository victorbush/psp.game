/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/gpu_intf.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

static void _begin_frame(gpu_type* gpu);
static void _end_frame(gpu_type* gpu);
static void _init(gpu_type* gpu);
static void _render_model(gpu_type* gpu);
static void _term(gpu_type* gpu);

/**
main
*/
void vlk_init_gpu_intf(gpu_type* gpu)
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

}

static void _render_model(gpu_type* gpu)
{

}

static void _term(gpu_type* gpu)
{

}
