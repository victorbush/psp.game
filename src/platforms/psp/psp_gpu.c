/*=========================================================
INCLUDES
=========================================================*/

#include <pspge.h>
#include <pspgu.h>
#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>

#include "gpu/gpu_intf.h"
#include "platforms/psp/psp_common.h"

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
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

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

static void _gpu_init(gpu_intf_type* gpu);
static void _gpu_term(gpu_intf_type* gpu);
static void _gpu_begin_frame(gpu_intf_type* gpu);
static void _gpu_end_frame(gpu_intf_type* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

/** 
psp_gpu_init_intf
*/
void psp_gpu_init_intf(gpu_intf_type* gpu)
{
	memset( gpu, 0, sizeof( *gpu ) );
	gpu->init = &_gpu_init;
	gpu->term = &_gpu_term;
	gpu->begin_frame = &_gpu_begin_frame;
	gpu->end_frame = &_gpu_end_frame;
	gpu->context = NULL;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

/**
_alloc_vram_buffer
*/
static void* _alloc_vram_buffer(_gpu_ctx_type* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm)
{
	uint32_t sz = _calc_mem_size(width, height, pixel_format_psm);
	
	// Get ptr to free vram memory
	void* result = (void*)ctx->vram_stack_ptr;

	// Update vram ptr for the request size
	ctx->vram_stack_ptr += sz;

	return result;
}

/**
_calc_mem_size
*/
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

/**
_gpu_begin_frame
*/
static void _gpu_begin_frame(gpu_intf_type* gpu)
{
	// Get context
	_gpu_ctx_type* ctx = _get_ctx(gpu->context);

	sceGuStart(GU_DIRECT, ctx->display_list);

	// clear screen
	sceGuClearColor(0xff554433);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
}

/**
_gpu_end_frame
*/
static void _gpu_end_frame(gpu_intf_type* gpu)
{
	// Get context
	_gpu_ctx_type* ctx = _get_ctx(gpu->context);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

/**
_gpu_init
*/
static void _gpu_init(gpu_intf_type* gpu)
{
	_gpu_ctx_type* ctx = malloc(sizeof(_gpu_ctx_type));
	gpu->context = (void*)ctx;
	
	ctx->frame_buffer_0 = _alloc_vram_buffer(ctx, BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
	ctx->frame_buffer_1 = _alloc_vram_buffer(ctx, BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
	ctx->z_buffer = _alloc_vram_buffer(ctx, BUF_WIDTH, SCR_HEIGHT, GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, ctx->display_list);
	sceGuDrawBuffer(GU_PSM_8888, ctx->frame_buffer_0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, ctx->frame_buffer_1, BUF_WIDTH);
	sceGuDepthBuffer(ctx->z_buffer, BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
	sceGuViewport(2048,2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
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

/**
_gpu_term
*/
static void _gpu_term(gpu_intf_type* gpu)
{
    sceGuTerm();

	free(gpu->context);
}
