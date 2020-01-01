/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void platform__construct(platform_t* platform, platform_intf_t* intf)
{
	clear_struct(platform);
	platform->intf = intf;

	platform->intf->construct(platform, intf);
}

void platform__destruct(platform_t* platform)
{
	platform->intf->destruct(platform);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void platform__run(platform_t* platform)
{
	platform->intf->run(platform);
}