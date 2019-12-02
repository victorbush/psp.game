/*=========================================================
INCLUDES
=========================================================*/

#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>

#include "ecs/components.h"
#include "gpu/gpu.h"
#include "platforms/common.h"

/*=========================================================
MACROS
=========================================================*/

#define _get_ctx(context) ( (_gpu_ctx_type*)context )

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	void* 		frame_buffer_0;	
	void*		frame_buffer_1;
	void*		z_buffer;
	uint32_t 	vram_stack_ptr;	/* using a simple stack for vram right now */

	/* display list (16 byte aligned) */
 	uint32_t __attribute__((aligned(16))) display_list[262144];

} _gpu_ctx_type;

/*=========================================================
VARIABLES
=========================================================*/

#define BUF_WIDTH (512)

/*=========================================================
DECLARATIONS
=========================================================*/

/**
Allocates a buffer 
*/
static void* _alloc_vram_buffer(_gpu_ctx_type* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm);

/**
Computes the size of video memory for a certain number of pixels given their pixel format.
*/
static uint32_t _calc_mem_size(uint32_t width, uint32_t height, uint32_t pixel_format_psm);

static void _gpu_begin_frame(gpu_t* gpu);
static void _gpu_create_model(gpu_t* gpu, gpu_model_t* model);
static void _gpu_destroy_model(gpu_t* gpu, gpu_model_t* model);
static void _gpu_end_frame(gpu_t* gpu);
static void _gpu_init(gpu_t* gpu);
static void _gpu_render_model(gpu_t* gpu, gpu_model_t* model, transform_comp_t* transform);
static void _gpu_render_plane(gpu_t* gpu, gpu_plane_t* plane, transform_comp_t* transform);
static void _gpu_term(gpu_t* gpu);
static void _test(gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void psp_gpu__init(gpu_t* gpu)
{
	memset( gpu, 0, sizeof( *gpu ) );
	gpu->begin_frame = &_gpu_begin_frame;
	gpu->create_model = &_gpu_create_model;
	gpu->destroy_model = &_gpu_destroy_model;
	gpu->end_frame = &_gpu_end_frame;
	gpu->init = &_gpu_init;
	gpu->render_model = &_gpu_render_model;
	gpu->render_plane = &_gpu_render_plane;
	gpu->term = &_gpu_term;
	gpu->test = &_test;
	gpu->context = NULL;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

static void _test(gpu_intf_type* gpu)
{
	int i = 0;
	int j =0;
}

static void* _alloc_vram_buffer(_gpu_ctx_type* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm)
{
	uint32_t sz = _calc_mem_size(width, height, pixel_format_psm);
	
	// Get ptr to free vram memory
	void* result = (void*)ctx->vram_stack_ptr;

	// Update vram ptr for the request size
	ctx->vram_stack_ptr += sz;

	return result;
}

static uint32_t _calc_mem_size(uint32_t width, uint32_t height, uint32_t pixel_format_psm)
{
	switch (pixel_format_psm)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
	}
}

static void _gpu_begin_frame(gpu_t* gpu)
{
	// Get context
	_gpu_ctx_type* ctx = _get_ctx(gpu->context);

	sceGuStart(GU_DIRECT, ctx->display_list);

	// clear screen
	sceGuClearColor(0xff550033);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);



		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);



		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();




	//	pspDebugScreenSetTextColor(0xFFFFFFFF);
	//		pspDebugScreenPrintf("TEST");

}

static void _gpu_end_frame(gpu_t* gpu)
{
	// Get context
	//_gpu_ctx_type* ctx = _get_ctx(gpu->context);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

static void _gpu_init(gpu_t* gpu)
{
	_gpu_ctx_type* ctx = malloc(sizeof(_gpu_ctx_type));
	gpu->context = (void*)ctx;
	
	ctx->frame_buffer_0 = _alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
	ctx->frame_buffer_1 = _alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
	ctx->z_buffer = _alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, ctx->display_list);
	sceGuDrawBuffer(GU_PSM_8888, ctx->frame_buffer_0, BUF_WIDTH);
	sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, ctx->frame_buffer_1, BUF_WIDTH);
	sceGuDepthBuffer(ctx->z_buffer, BUF_WIDTH);
	sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
	sceGuViewport(2048,2048, SCREEN_WIDTH, SCREEN_HEIGHT);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	//sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

static void _gpu_render_plane(gpu_t* gpu, gpu_plane_t* plane,  transform_comp_t* transform)
{
	sceGuClearColor(0xff550000);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
}

static void _gpu_term(gpu_t* gpu)
{
    sceGuTerm();

	free(gpu->context);
}








struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) vertices[12*3] =
{
	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 0, 0xff007f00, 1,-1, 1}, // 3
	{1, 1, 0xff007f00, 1, 1, 1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 1, 0xff007f00, 1, 1, 1}, // 7
	{0, 1, 0xff007f00, 1, 1,-1}, // 4

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 0, 0xff007f00,-1, 1,-1}, // 3
	{1, 1, 0xff007f00,-1, 1, 1}, // 7

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 1, 0xff007f00,-1, 1, 1}, // 7
	{0, 1, 0xff007f00,-1,-1, 1}, // 4

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 0, 0xff00007f, 1, 1,-1}, // 1
	{1, 1, 0xff00007f, 1, 1, 1}, // 2

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 1, 0xff00007f, 1, 1, 1}, // 2
	{0, 1, 0xff00007f,-1, 1, 1}, // 3

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 0, 0xff00007f,-1,-1, 1}, // 7
	{1, 1, 0xff00007f, 1,-1, 1}, // 6

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 1, 0xff00007f, 1,-1, 1}, // 6
	{0, 1, 0xff00007f, 1,-1,-1}, // 5
};

/**

*/
static void _render_model(gpu_t* gpu, gpu_model_t* model,  transform_comp_t* transform)
{
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	{
		//ScePspFVector3 pos = { 0, 0, -2.5f };
		//ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
		sceGumTranslate(transform->pos);
		//sceGumRotateXYZ(&rot);
	}


	// setup texture

	sceGuTexMode(GU_PSM_4444,0,0,0);
	//sceGuTexImage(0,64,64,64,logo_start);
	//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexEnvColor(0xffff00);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);

	// draw cube

	sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);

}