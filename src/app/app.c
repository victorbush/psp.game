/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "app/app.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a application.

@param app The application context to construct.
@param intf The application interface to use.
*/
void app__construct(app_t* app, app_intf_t* intf) 
{
	clear_struct(app);
	app->intf = intf;

	/* Construct */
	app->intf->__construct(app);
}

/**
Destructs a application.

@param app The application context to destruct.
*/
void app__destruct(app_t* app)
{
	app->intf->__destruct(app);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Executes the next frame.
*/
void app__run_frame(app_t* app)
{
	app->intf->__run_frame(app);
}