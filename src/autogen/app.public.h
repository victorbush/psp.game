
/**
Constructs an application.

@param app The application context to construct.
@param intf The application interface to use.
*/
void app__construct(app_t* app, app_intf_t* intf) 
;

/**
Destructs a application.

@param app The application context to destruct.
*/
void app__destruct(app_t* app)
;

/**
Executes the next frame.

@param app The app context.
*/
void app__run_frame(app_t* app)
;

/**
Checks if the app should exit.

@param app The app context.
@return A value indicating if the app is being exited.
*/
boolean app__should_exit(app_t* app)
;
