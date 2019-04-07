/*=========================================================
INCLUDES
=========================================================*/

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspkernel.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include "gpu.h"

PSP_MODULE_INFO("Comanche PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

/*=========================================================
VARIABLES
=========================================================*/

static int s_exit_pending = 0;

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
	setup_callbacks();

	gpu_init();

uint32_t sz =	sceGeEdramGetSize();
		pspDebugScreenPrintf("Size: %i\n", sz);


    while(running())
	{


		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1); 

		if (pad.Buttons & PSP_CTRL_SQUARE){
				s_exit_pending = 1;
			}

		//pspDebugScreenClear();

		gpu_begin_frame();

		TER_render();

		gpu_end_frame();
    }

	gpu_term();

    sceKernelExitGame();
	return 0;
}
