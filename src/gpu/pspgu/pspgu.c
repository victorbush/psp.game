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
#include "global.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "engine/kk_math.h"
#include "gpu/gpu.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "gpu/gpu_texture.h"
#include "gpu/pspgu/pspgu.h"
#include "gpu/pspgu/pspgu_window.h"
#include "utl/utl.h"

#include "autogen/pspgu.static.h"

/*=========================================================
MACROS / CONSTANTS
=========================================================*/

#define BUF_WIDTH (512)

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Creates a GPU interface for the PSP GPU.
*/
void pspgu__init_gpu_intf(gpu_intf_t* intf)
{
	kk_log__dbg("pspgu - init gpu interface");
	clear_struct(intf);

	/* Allocate memory for PSP GPU implementation */
	_pspgu_t* psp = malloc(sizeof(_pspgu_t));
	if (!psp)
	{
		kk_log__fatal("Failed to allocate Vulkan context.");
	}

	clear_struct(psp);

	/* Setup GPU interface */
	intf->impl = psp;
	intf->__construct = pspgu__construct;
	intf->__destruct = pspgu__destruct;
	intf->__wait_idle = pspgu__wait_idle;
	intf->anim_model__construct = pspgu_anim_model__construct;
	intf->anim_model__destruct = pspgu_anim_model__destruct;
	intf->anim_model__render = pspgu_anim_model__render;
	intf->frame__construct = pspgu_frame__construct;
	intf->frame__construct = pspgu_frame__destruct;
	intf->material__construct = pspgu_material__construct;
	intf->material__destruct = pspgu_material__destruct;
	intf->plane__construct = pspgu_plane__construct;
	intf->plane__destruct = pspgu_plane__destruct;
	intf->plane__render = pspgu_plane__render;
	intf->plane__update_verts = pspgu_plane__update_verts;
	intf->static_model__construct = pspgu_static_model__construct;
	intf->static_model__destruct = pspgu_static_model__destruct;
	intf->static_model__render = pspgu_static_model__render;
	intf->texture__construct = pspgu_texture__construct;
	intf->texture__destruct = pspgu_texture__destruct;
	intf->window__begin_frame = pspgu_window__begin_frame;
	intf->window__construct = pspgu_window__construct;
	intf->window__destruct = pspgu_window__destruct;
	intf->window__end_frame = pspgu_window__end_frame;
	intf->window__render_imgui = pspgu_window__render_imgui;
	intf->window__resize = pspgu_window__resize;
}

//## internal
_pspgu_t* _pspgu__get_context(gpu_t* gpu)
{
	return (_pspgu_t*)gpu->intf->impl;
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

//## static
/** Allocates a buffer. */
static void* alloc_vram_buffer(_pspgu_t* ctx, uint32_t width, uint32_t height, uint32_t pixel_format_psm)
{
	uint32_t sz = calc_mem_size(width, height, pixel_format_psm);
	
	/* Get ptr to free vram memory */
	void* result = (void*)ctx->vram_stack_ptr;

	/* Update vram ptr for the request size */
	ctx->vram_stack_ptr += sz;

	return result;
}

//## static
/** Computes the size of video memory for a certain number of pixels given their pixel format. */
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

//## static
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
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

//## static
static void pspgu__destruct(gpu_t* gpu)
{
	sceGuTerm();

	/* Free implementation memory */
	free(gpu->intf->impl);
}

//## static
static void pspgu__wait_idle(gpu_t* gpu)
{
	/* Nothing to do for PSP GPU */
}

//## static
static void pspgu_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu)
{
	// TODO
}

//## static
static void pspgu_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu)
{
	// TODO
}

//## static
static void pspgu_anim_model__render(gpu_anim_model_t* model, gpu_t* gpu, gpu_frame_t* frame, ecs_transform_t* transform)
{
	// TODO
}

//## static
static void pspgu_frame__construct(gpu_frame_t* frame, gpu_t* gpu)
{
}

//## static
static void pspgu_frame__destruct(gpu_frame_t* frame, gpu_t* gpu)
{
}

//## static
static void pspgu_material__construct(gpu_material_t* material, gpu_t* gpu)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	material->data = malloc(sizeof(_pspgu_material_t));
	if (!material->data)
	{
		kk_log__fatal("Failed to allocate memory for material.");
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

//## static
static void pspgu_material__destruct(gpu_material_t* material, gpu_t* gpu)
{
	/* Free GPU data */
	_pspgu_material__destruct((_pspgu_material_t*)material->data);
	free(material->data);
}

//## static
static void pspgu_plane__construct(gpu_plane_t* plane, gpu_t* gpu)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	plane->data = malloc(sizeof(_pspgu_plane_t));
	if (!plane->data)
	{
		kk_log__fatal("Failed to allocate memory for plane.");
	}

	/* Construct */
	_pspgu_plane__construct((_pspgu_plane_t*)plane->data, ctx);
}

//## static
static void pspgu_plane__destruct(gpu_plane_t* plane, gpu_t* gpu)
{
	/* Free GPU data */
	_pspgu_plane__destruct((_pspgu_plane_t*)plane->data);
	free(plane->data);
}

//## static
static void pspgu_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);
	
	/* Setup model matrix */
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	/* Setup material */
	sceGuDisable(GU_TEXTURE_2D);

	if (material)
	{
		if (material->diffuse_texture)
		{
			_pspgu_material_t* mat = (_pspgu_material_t*)material->data;

			sceGuEnable(GU_TEXTURE_2D);
			sceGuTexMode(GU_PSM_8888, 0, 0, 0);
			sceGuTexImage(0, mat->diffuse_texture->width, mat->diffuse_texture->height, mat->diffuse_texture->width, mat->diffuse_texture->data);
			//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
			sceGuTexEnvColor(0xffff00);
			sceGuTexFilter(GU_LINEAR,GU_LINEAR);
			sceGuTexScale(1.0f, 1.0f);
			sceGuTexOffset(0.0f, 0.0f);
		}

		sceGuAmbientColor(0xffffffff);
	}

	/* Draw the plane */
	_pspgu_plane__render((_pspgu_plane_t*)plane->data, ctx);
}

//## static
static void pspgu_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, kk_vec3_t verts[4])
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);
	_pspgu_plane__update_verts((_pspgu_plane_t*)plane->data, ctx, verts);
}

//## static
static void pspgu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const tinyobj_t* obj)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	model->data = malloc(sizeof(_pspgu_static_model_t));
	if (!model->data)
	{
		kk_log__fatal("Failed to allocate memory for static model.");
	}

	/* Construct */
	_pspgu_static_model__construct((_pspgu_static_model_t*)model->data, ctx, obj);
}

//## static
static void pspgu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu)
{
	/* Free GPU data */
	_pspgu_static_model__destruct((_pspgu_static_model_t*)model->data);
	free(model->data);
}

//## static
static void pspgu_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, ecs_transform_t* transform)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	sceGumMatrixMode(GU_MODEL);
 	sceGumLoadIdentity();
 	{
		/* Translate */
 		sceGumTranslate(&transform->pos);

		/* Rotate */
		kk_mat4_t rotation_matrix;
		kk_math_quat_mat4(&transform->rot, &rotation_matrix);
		sceGumMultMatrix(&rotation_matrix);
 	}

	/* Setup material */
	//if (material)
	//{
	//	_pspgu_material_t* mat = (_pspgu_material_t*)material->data;

	//	if (mat->diffuse_texture)
	//	{
	//		sceGuEnable(GU_TEXTURE_2D);
	//		sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	//		sceGuTexImage(0, mat->diffuse_texture->width, mat->diffuse_texture->height, mat->diffuse_texture->width, mat->diffuse_texture->data);
	//		//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	//	}
	//			
	//	sceGuTexEnvColor(0xffff00);
	//	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	//	sceGuTexScale(1.0f, 1.0f);
	//	sceGuTexOffset(0.0f, 0.0f);
	//	sceGuAmbientColor(0xffffffff);
	//}

	/* Draw mesh */
	_pspgu_static_model__render((_pspgu_static_model_t*)model->data, ctx);
}

//## static
static void pspgu_texture__construct(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height)
{
	_pspgu_t* ctx = _pspgu__get_context(gpu);

	/* Allocate data used for the PSP implementation */
	texture->data = malloc(sizeof(_pspgu_texture_t));
	if (!texture->data)
	{
		kk_log__fatal("Failed to allocate memory for texture.");
	}
	
	/* Construct */
	_pspgu_texture__construct((_pspgu_texture_t*)texture->data, ctx, img, width, height, width * height * sizeof(uint32_t));
}

//## static
static void pspgu_texture__destruct(gpu_texture_t* texture, gpu_t* gpu)
{
	/* Free GPU data */
	_pspgu_texture__destruct((_pspgu_texture_t*)texture->data);
	free(texture->data);
}
