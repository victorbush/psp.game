/*=========================================================
INCLUDES
=========================================================*/

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <psprtc.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include "common.h"
#include "engine/engine.h"
#include "gpu/gpu.h"
#include "gpu/psp/psp_gpu.h"
#include "platform/platform.h"

PSP_MODULE_INFO("Jetz PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	uint64_t 		last_tick;		/* Tick value from last frame */
	uint64_t		current_tick;	/* Tick value from current frame */

} psp_platform_t;


/*=========================================================
VARIABLES
=========================================================*/

static engine_t 		s_engine;
static gpu_t			s_gpu;
static platform_t		s_platform;
static psp_platform_t	s_platform_psp;
static int 				s_exit_pending = 0;

/*=========================================================
DECLARATIONS
=========================================================*/

/** Initializes the engine and platform objects. */
static void _init_engine();

/** Platfor callback to get frame delta time. */
static uint32_t _platform_get_time(platform_t* platform);

/*=========================================================
FUNCTIONS
=========================================================*/

static int running()
{
	return !s_exit_pending;
}

static int exit_callback(int arg1, int arg2, void *common)
{
	s_exit_pending = 1;
	return 0;
}

static int callback_thread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

static int setup_callbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

static void _init_engine()
{
	/* Setup the GPU */
	psp_gpu__init(&s_gpu);

	/* Setup the platform */
	clear_struct(&s_platform);
	clear_struct(&s_platform_psp);
	s_platform.context = (void*)&s_platform_psp;
	s_platform.get_time = &_platform_get_time;

	/* Construct the engine */
	engine__construct(&s_engine, &s_gpu, &s_platform);
}

int main(int argc, char* argv[])
{
	pspDebugScreenInit();
	pspDebugScreenClear();
	pspDebugScreenSetXY(0, 0);

	setup_callbacks();

	_init_engine();


uint32_t sz =	sceGeEdramGetSize();


    while(running())
	{
		engine__run_frame(&s_engine);



		/*

		switch (game state)
		{

			case IN_GAME:

			case IN_MENU:

			case LOADING_MENU:

			case LOADING_GAME:






			void in_game()
			{
				input_system_run();

				player_system_run();

				physics_system_run();
			}
		}


		*/

		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1); 

		if (pad.Buttons & PSP_CTRL_SQUARE){
				s_exit_pending = 1;
			}

		s_platform.keydown__camera_forward = pad.Buttons & PSP_CTRL_UP;
		s_platform.keydown__camera_backward = pad.Buttons & PSP_CTRL_DOWN;
		s_platform.keydown__camera_strafe_left = pad.Buttons & PSP_CTRL_LEFT;
		s_platform.keydown__camera_strafe_right = pad.Buttons & PSP_CTRL_RIGHT;

		//pspDebugScreenClear();


	}

	engine__destruct(&s_engine);

	sceKernelExitGame();
	return 0;
}




uint32_t _platform_get_time(platform_t* platform)
{
	psp_platform_t* ctx = (psp_platform_t*)platform->context;

	int tick_res = sceRtcGetTickResolution();

	if (tick_res == 0)
	{
		pspDebugScreenPrintf("LOL");
		tick_res = 1;
	}

	sceRtcGetCurrentTick(&ctx->current_tick);

	double time_span = ((ctx->current_tick - ctx->last_tick)) / (float)tick_res;

	ctx->last_tick = ctx->current_tick;

//uint32_t val = (uint32_t)time_span;

	//pspDebugScreenClear();
	//pspDebugScreenPrintf("%f", time_span);
	//		pspDebugScreenPrintf("fps: %d.%03d (%dMB/s)",(int)curr_fps,(int)((curr_fps-(int)curr_fps) * 1000.0f),transfer_rate);

	return (uint32_t)(time_span * 1000);
}
