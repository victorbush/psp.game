/*=========================================================
Main entry point for the editor application.

Uses GLFW and Vulkan.
=========================================================*/

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "app/app.h"
#include "log/log.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

platform_t*						g_platform;

static app_t					s_app;
static app_intf_t				s_app_intf;
static platform_t				s_platform;

/*=========================================================
FUNCTIONS
=========================================================*/

int main(int argc, char* argv[])
{
	startup();
	platform__run();
	shutdown();
}

static void app_intf__run_frame()
{

}

static void shutdown()
{
	platform__destruct(&s_platform);
	app__destruct(&s_app);
}

static void startup()
{
	/*
	Setup app interface
	*/
	clear_struct(&s_app_intf);
	s_app_intf.__run_frame = app_intf__run_frame;

	app__construct(&s_app, &s_app_intf);

	/*
	Platform
	*/
	glfw__init_intf(&s_platform_intf);
	platform__construct(&s_platform, &s_platform_intf);
}
