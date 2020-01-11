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

//## public
/**
Constructs an application.

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

//## public
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

//## public
/**
Executes the next frame.

@param app The app context.
*/
void app__run_frame(app_t* app)
{
	app->intf->__run_frame(app);
}

//## public
/**
Checks if the app should exit.

@param app The app context.
@return A value indicating if the app is being exited.
*/
boolean app__should_exit(app_t* app)
{
	return app->intf->__should_exit(app);
}