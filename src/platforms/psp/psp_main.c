/*=========================================================
INCLUDES
=========================================================*/

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspkernel.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include "engine/engine.h"
#include "gpu/gpu.h"
#include "gpu/psp/psp_gpu.h"
#include "platforms/common.h"

PSP_MODULE_INFO("Jetz PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

/*=========================================================
VARIABLES
=========================================================*/

static engine_type 		s_engine;
static int 				s_exit_pending = 0;

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

int main(int argc, char* argv[])
{
    pspDebugScreenInit();
    pspDebugScreenClear();
	pspDebugScreenSetXY(0, 0);

	setup_callbacks();

	engine_init(&s_engine);
	psp_gpu_init_intf(&s_engine.gpu);
	s_engine.gpu.init(&s_engine.gpu);


uint32_t sz =	sceGeEdramGetSize();


    while(running())
	{
		engine_run_frame(&s_engine);



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

		//pspDebugScreenClear();


	}

	s_engine.gpu.term(&s_engine.gpu);

	sceKernelExitGame();
	return 0;
}
