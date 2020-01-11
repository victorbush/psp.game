#ifndef APP_H
#define APP_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/app_.h"

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef void (*app_construct_func)(app_t* app);
typedef void (*app_destruct_func)(app_t* app);
typedef void (*app_run_frame_func)(app_t* app);
typedef boolean (*app_should_exit_func)(app_t* app);

struct app_intf_s
{
	void*						context;			/* App context memory */

	app_construct_func			__construct;
	app_destruct_func			__destruct;
	app_run_frame_func			__run_frame;
	app_should_exit_func		__should_exit;
};

struct app_s
{
	app_intf_t*					intf;				/* Interface that implements application functions. */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/app.public.h"

#endif /* APP_H */