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

#include "common.h"
#include "ecs/components.h"
#include "gpu/gpu.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "gpu/gpu_texture.h"
#include "gpu/pspgu/pspgu_prv.h"
#include "utl/utl.h"
#include "utl/utl_log.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

#define BUF_WIDTH (512)

/*=========================================================
DECLARATIONS
=========================================================*/

/** Allocates a buffer. */
static void* alloc_vram_buffer(_pspgu_t* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm);

/** Computes the size of video memory for a certain number of pixels given their pixel format. */
static uint32_t calc_mem_size(uint32_t width, uint32_t height, uint32_t pixel_format_psm);

static void pspgu__begin_frame(gpu_t* gpu, camera_t* cam);
static void pspgu__construct(gpu_t* gpu);
static void pspgu__destruct(gpu_t* gpu);
static void pspgu__end_frame(gpu_t* gpu);
static void pspgu__wait_idle(gpu_t* gpu);
static void pspgu_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu);
static void pspgu_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu);
static void pspgu_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, transform_comp_t* transform);
static void pspgu_material__construct(gpu_material_t* material, gpu_t* gpu);
static void pspgu_material__destruct(gpu_material_t* material, gpu_t* gpu);
static void pspgu_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform);
static void pspgu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj);
static void pspgu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu);
static void pspgu_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform);
static void pspgu_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);
static void pspgu_texture__destruct(gpu_texture_t* texture, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void pspgu__init_gpu_intf(gpu_intf_t* intf)
{
	clear_struct(intf);

	/* Allocate memory for PSP GPU implementation */
	_pspgu_t* psp = malloc(sizeof(_pspgu_t));
	if (!psp)
	{
		FATAL("Failed to allocate Vulkan context.");
	}

	clear_struct(psp);

	/* Setup GPU interface */
	intf->impl = psp;
	intf->__begin_frame = pspgu__begin_frame;
	intf->__construct = pspgu__construct;
	intf->__destruct = pspgu__destruct;
	intf->__end_frame = pspgu__end_frame;
	intf->__wait_idle = pspgu__wait_idle;
	intf->anim_model__construct = pspgu_anim_model__construct;
	intf->anim_model__destruct = pspgu_anim_model__destruct;
	intf->anim_model__render = pspgu_anim_model__render;
	intf->material__construct = pspgu_material__construct;
	intf->material__destruct = pspgu_material__destruct;
	intf->plane__render = pspgu_plane__render;
	intf->static_model__construct = pspgu_static_model__construct;
	intf->static_model__destruct = pspgu_static_model__destruct;
	intf->static_model__render = pspgu_static_model__render;
	intf->texture__construct = pspgu_texture__construct;
	intf->texture__destruct = pspgu_texture__destruct;
}

_pspgu_t* _pspgu__get_context(gpu_t* gpu)
{
	return (_pspgu_t*)gpu->intf->impl;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

static void* alloc_vram_buffer(_pspgu_t* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm)
{
	uint32_t sz = calc_mem_size(width, height, pixel_format_psm);
	
	/* Get ptr to free vram memory */
	void* result = (void*)ctx->vram_stack_ptr;

	/* Update vram ptr for the request size */
	ctx->vram_stack_ptr += sz;

	return result;
}

static uint32_t calc_mem_size(uint32_t width, uint32_t height, uint32_t pixel_format_psm)
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

static void pspgu__begin_frame(gpu_t* gpu, camera_t* cam)
{
	/* Get context */
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	sceGuStart(GU_DIRECT, ctx->display_list);

	/* Clear screen */
	sceGuClearColor(0xff550033);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

	/* Setup projection matrix */
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(45.0f, 16.0f/9.0f, 0.5f, 1000.0f);

	/* Setup view matrix */
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	ScePspFVector3 look_at;
	look_at.x = cam->pos.x + cam->dir.x;
	look_at.y = cam->pos.y + cam->dir.y;
	look_at.z = cam->pos.z + cam->dir.z;

	sceGumLookAt(&cam->pos, &look_at, &cam->up);
}

static void pspgu__construct(gpu_t* gpu)
{
	/* Get context */
	_pspgu_t* ctx = _pspgu__get_context(gpu);
	
	ctx->frame_buffer_0 = alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
	ctx->frame_buffer_1 = alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
	ctx->z_buffer = alloc_vram_buffer(ctx, BUF_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);

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

static void pspgu__destruct(gpu_t* gpu)
{
	sceGuTerm();

	/* Free implementation memory */
	free(gpu->intf->impl);
}

static void pspgu__end_frame(gpu_t* gpu)
{
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

static void pspgu__wait_idle(gpu_t* gpu)
{
	/* Nothing to do for PSP GPU */
}

static void pspgu_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu)
{
	// TODO
}

static void pspgu_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu)
{
	// TODO
}

static void pspgu_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, transform_comp_t* transform)
{
	// TODO
}

static void pspgu_material__construct(gpu_material_t* material, gpu_t* gpu)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	material->data = malloc(sizeof(_pspgu_material_t));
	if (!material->data)
	{
		FATAL("Failed to allocate memory for material.");
	}
	
	/* Get diffuse texture */
	_pspgu_texture_t* diffuse_tex = NULL;
	if (material->diffuse_texture)
	{
		diffuse_tex = (_pspgu_texture_t*)material->diffuse_texture->data;
	}

	/* Construct */
	_pspgu_material__construct((_pspgu_material_t*)material->data, ctx, material->diffuse_color, diffuse_tex);
}

static void pspgu_material__destruct(gpu_material_t* material, gpu_t* gpu)
{
	/* Free GPU data */
	_pspgu_material__destruct((_pspgu_material_t*)material->data);
	free(material->data);
}

static void pspgu_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform)
{
	uint32_t i;

	/*
	Plane vertices

	(0,0,-1)     (1,0,-1)
		1 ----2
		|    /|
		|  /  |
		|/    |
		0-----3
	(0,0,0)      (1,0,0)
	*/
	_pspgu_vertex_t __attribute__((aligned(16))) plane_vertices[4] =
	{
		{0, 0, 0xff7f0000, 0, 0, 0}, 	/* 0 - front left */
		{1, 0, 0xff7f0000, 0, 0, -1}, 	/* 1 - back left */
		{1, 1, 0xff7f0000, 1, 0, -1}, 	/* 2 - back right */
		{0, 0, 0xff1f0000, 1, 0, 0}, 	/* 3 - front left */
	};

	/* Plane indices */
	uint8_t plane_indices[6] =
	{
		0, 1, 2,
		0, 2, 3	
	};

	/* Adjust vertices based on the plane being rendered */
	for (i = 0; i < cnt_of_array(plane_vertices); ++i)
	{
		plane_vertices[i].x -= plane->anchor.x;
		plane_vertices[i].z += plane->anchor.y;
		plane_vertices[i].x *= plane->width;
		plane_vertices[i].z *= plane->height;
		plane_vertices[i].color = utl_pack_rgba_float(plane->color.x, plane->color.y, plane->color.z, 1.0f);
	}

	/* Setup model matrix */
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	sceGumTranslate((ScePspFVector3*)&transform->pos);

	// setup texture
	sceGuDisable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_4444,0,0,0);
	//sceGuTexImage(0,64,64,64,logo_start);
	//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexEnvColor(0xffff00);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);

	/* Draw the plane */
	sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_INDEX_8BIT | GU_TRANSFORM_3D, 6, plane_indices, plane_vertices);
}

static void pspgu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	model->data = malloc(sizeof(_pspgu_static_model_t));
	if (!model->data)
	{
		FATAL("Failed to allocate memory for static model.");
	}

	/* Construct */
	_pspgu_static_model__construct((_pspgu_static_model_t*)model->data, ctx, obj);
}

static void pspgu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Free GPU data */
	_pspgu_static_model__destruct((_pspgu_static_model_t*)model->data);
	free(model->data);
}

static void pspgu_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	sceGumMatrixMode(GU_MODEL);
 	sceGumLoadIdentity();
 	{
 		//ScePspFVector3 pos = { 0, 0, -2.5f };
 		//ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
 		sceGumTranslate(&transform->pos);
 		//sceGumRotateXYZ(&rot);
 	}

	/* Setup material */
	if (material)
	{
		_pspgu_material_t* mat = (_pspgu_material_t*)material->data;

		if (mat->diffuse_texture)
		{
			sceGuEnable(GU_TEXTURE_2D);
			sceGuTexMode(GU_PSM_8888, 0, 0, 0);
			sceGuTexImage(0, mat->diffuse_texture->width, mat->diffuse_texture->height, mat->diffuse_texture->height, mat->diffuse_texture->data);
			//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
		}
				
		sceGuTexEnvColor(0xffff00);
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);
		sceGuTexScale(1.0f, 1.0f);
		sceGuTexOffset(0.0f, 0.0f);
		sceGuAmbientColor(0xffffffff);
	}

	/* Draw mesh */
	_pspgu_static_model__render((_pspgu_static_model_t*)model->data, ctx);
}

static void pspgu_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	texture->data = malloc(sizeof(_pspgu_texture_t));
	if (!texture->data)
	{
		FATAL("Failed to allocate memory for texture.");
	}
	
	/* Construct */
	_pspgu_texture__construct((_pspgu_texture_t*)texture->data, ctx, img, width, height, width * height * sizeof(uint32_t));
}

static void pspgu_texture__destruct(gpu_texture_t* texture, gpu_t* gpu)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Free GPU data */
	_pspgu_texture__destruct((_pspgu_texture_t*)texture->data);
	free(texture->data);
}













// vertex_t __attribute__((aligned(16))) vertices[12*3] =
// {
// 	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
// 	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
// 	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

// 	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
// 	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
// 	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

// 	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
// 	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
// 	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

// 	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
// 	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
// 	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

// 	{0, 0, 0xff007f00, 1,-1,-1}, // 0
// 	{1, 0, 0xff007f00, 1,-1, 1}, // 3
// 	{1, 1, 0xff007f00, 1, 1, 1}, // 7

// 	{0, 0, 0xff007f00, 1,-1,-1}, // 0
// 	{1, 1, 0xff007f00, 1, 1, 1}, // 7
// 	{0, 1, 0xff007f00, 1, 1,-1}, // 4

// 	{0, 0, 0xff007f00,-1,-1,-1}, // 0
// 	{1, 0, 0xff007f00,-1, 1,-1}, // 3
// 	{1, 1, 0xff007f00,-1, 1, 1}, // 7

// 	{0, 0, 0xff007f00,-1,-1,-1}, // 0
// 	{1, 1, 0xff007f00,-1, 1, 1}, // 7
// 	{0, 1, 0xff007f00,-1,-1, 1}, // 4

// 	{0, 0, 0xff00007f,-1, 1,-1}, // 0
// 	{1, 0, 0xff00007f, 1, 1,-1}, // 1
// 	{1, 1, 0xff00007f, 1, 1, 1}, // 2

// 	{0, 0, 0xff00007f,-1, 1,-1}, // 0
// 	{1, 1, 0xff00007f, 1, 1, 1}, // 2
// 	{0, 1, 0xff00007f,-1, 1, 1}, // 3

// 	{0, 0, 0xff00007f,-1,-1,-1}, // 4
// 	{1, 0, 0xff00007f,-1,-1, 1}, // 7
// 	{1, 1, 0xff00007f, 1,-1, 1}, // 6

// 	{0, 0, 0xff00007f,-1,-1,-1}, // 4
// 	{1, 1, 0xff00007f, 1,-1, 1}, // 6
// 	{0, 1, 0xff00007f, 1,-1,-1}, // 5
// };

// /**

// */
// static void _gpu_render_model(gpu_t* gpu, gpu_model_t* model,  transform_comp_t* transform)
// {
// 	sceGumMatrixMode(GU_MODEL);
// 	sceGumLoadIdentity();
// 	{
// 		//ScePspFVector3 pos = { 0, 0, -2.5f };
// 		//ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
// //		sceGumTranslate(transform->pos);
// 		//sceGumRotateXYZ(&rot);
// 	}


// 	// setup texture

// 	sceGuTexMode(GU_PSM_4444,0,0,0);
// 	//sceGuTexImage(0,64,64,64,logo_start);
// 	//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
// 	sceGuTexEnvColor(0xffff00);
// 	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
// 	sceGuTexScale(1.0f,1.0f);
// 	sceGuTexOffset(0.0f,0.0f);
// 	sceGuAmbientColor(0xffffffff);

// 	// draw cube

// 	sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);

// }