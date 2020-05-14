/*=========================================================
INCLUDES
=========================================================*/

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#include "common.h"
#include "engine/kk_camera.h"
#include "gpu/gpu_window.h"
#include "gpu/pspgu/pspgu.h"
#include "gpu/pspgu/pspgu_window.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
void pspgu_window__construct(
	gpu_window_t* window,
	gpu_t* gpu,
	uint32_t width,
	uint32_t height)
{
}

//## public
void pspgu_window__destruct(gpu_window_t* window, gpu_t* gpu)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void pspgu_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, kk_camera_t* cam)
{
	/* Get context */
	_pspgu_t* ctx = _pspgu__get_context(window->gpu);

	sceGuStart(GU_DIRECT, ctx->display_list);

	/* Clear screen */
	sceGuClearColor(0xff550033);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

	/* Setup projection matrix */
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(45.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

	/* Setup view matrix */
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	ScePspFVector3 look_at;
	look_at.x = cam->pos.x + cam->dir.x;
	look_at.y = cam->pos.y + cam->dir.y;
	look_at.z = cam->pos.z + cam->dir.z;

	sceGumLookAt(&cam->pos, &look_at, &cam->up);
}

//## public
void pspgu_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

//## public
void pspgu_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data)
{
}

//## public
void pspgu_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
{
}
