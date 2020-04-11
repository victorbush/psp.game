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

#include <stdio.h>

#include "common.h"
#include "app/app.h"
#include "app/game/jetz.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/pspgu/pspgu.h"
#include "platform/platform.h"

#include "autogen/psp_main.static.h"

PSP_MODULE_INFO("Jetz PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

/*=========================================================
CONSTANTS
=========================================================*/

static const char*		LOG_FILE_NAME = "log.txt";

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

app_t*						g_app;
gpu_t*						g_gpu;
kk_log_t*					g_log;
platform_t*					g_platform;

static app_t				s_app;
static app_intf_t			s_app_intf;
static gpu_t				s_gpu;
static gpu_intf_t			s_gpu_intf;
static kk_log_t				s_log;
static platform_t			s_platform;
static psp_platform_t		s_platform_psp;

static int 					s_exit_pending = 0;

/*=========================================================
FUNCTIONS
=========================================================*/

//## static
static boolean is_running()
{
	return !s_exit_pending;
}

//## static
static int exit_callback(int arg1, int arg2, void *common)
{
	s_exit_pending = 1;
	return 0;
}

//## static
static int callback_thread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

//## static
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

int main(int argc, char* argv[])
{
	pspDebugScreenInit();
	pspDebugScreenClear();
	pspDebugScreenSetXY(0, 0);

	setup_callbacks();

	startup();


//uint32_t sz =	sceGeEdramGetSize();


    while(is_running())
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

	shutdown();

	sceKernelExitGame();
	return 0;
}

//## static
/** Logs a message to a log file. */
static void log_to_file(log_t* log, const char* msg)
{
	/* Append to log file */
	FILE* f = fopen(LOG_FILE_NAME, "a");
	if (!f)
	{
		return;
	}

	fprintf(f, msg);
	fclose(f);
}

//## static
/** Platform callback to get frame delta time. */
static uint32_t platform_get_time(platform_t* platform)
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

//## static
/** Loads a file. */
static boolean platform_load_file(const char* filename, boolean binary, long* out__size, void** out__buffer)
{
	FILE* f;

	/* Open the file */
	if (binary)
	{
		f = fopen(filename, "rb");
	}
	else
	{
		f = fopen(filename, "r");
	}

	/* Make sure file was opened */
	if (!f)
	{
		log__error("Failed to open file.");
		return FALSE;
	}

	/* Get file length */
	fseek(f, 0, SEEK_END);
	*out__size = ftell(f);

	/* Alloc buffer */
	*out__buffer = malloc(*out__size);
	if (!*out__buffer)
	{
		log__error("Failed to allocate file buffer.");
		return FALSE;
	}
	
	/* Read file into memory */
	fseek(f, 0, SEEK_SET);
	fread(*out__buffer, *out__size, 1, f);

	/* Close file */
	fclose(f);

	/* Success */
	return TRUE;
}

//## static
/** Destructs the engine and platform objects. */
static void shutdown()
{
	engine__destruct(&s_engine);

	log__destruct(g_log);
}

//## static
/** Initializes the engine and platform objects. */
static void startup()
{
	/*
	Setup logging 
	*/
	g_log = &s_log;
	kk_log__construct(g_log);

	/* Wipe log file and setup logging target */
	FILE* f = fopen(LOG_FILE_NAME, "w");
	if (f)
	{
		fclose(f);

		kk_log__register_target(g_log, log_to_file);
		kk_log__dbg("Logging initialized.");
	}

	/*
	Setup platform 
	*/
	clear_struct(&s_platform);
	clear_struct(&s_platform_psp);
	g_platform = &s_platform;
	g_platform->context = (void*)&s_platform_psp;
	g_platform->get_time = &platform_get_time;
	g_platform->load_file = &platform_load_file;
	g_platform->window__construct = &psp_window__construct;
	g_platform->window__destruct = &psp_window__destruct;

	/*
	Setup GPU 
	*/
	g_gpu = &s_gpu;
	pspgu__init_gpu_intf(&s_gpu_intf);
	gpu__construct(g_gpu, &s_gpu_intf);

	/*
	Construct app 
	*/
	g_app = &s_app;
	jetz__init_app_intf(&s_app_intf);
	app__construct(&s_app, &s_app_intf);
}

//## static
static void psp_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
{

}

//## static
void psp_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
{

}