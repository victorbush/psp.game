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

typedef void (*app_run_frame_func)(app_t* app);

struct app_intf_s
{
	app_run_frame_func			__run_frame;
};

struct app_s
{
	app_intf_t*					intf;				/* Interface that implements application functions. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a application.

@param app The application context to construct.
@param intf The application interface to use.
*/
void app__construct(app_t* app, app_intf_t* intf);

/**
Destructs a application.

@param app The application context to destruct.
*/
void app__destruct(app_t* app);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* APP_H */