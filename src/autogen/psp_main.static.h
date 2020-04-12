/*=========================================================
This file is automatically generated. Do not edit manually.
=========================================================*/

static boolean is_running()
;

static int exit_callback(int arg1, int arg2, void *common)
;

static int callback_thread(SceSize args, void *argp)
;

static int setup_callbacks(void)
;

/** Logs a message to a log file. */
static void log_to_file(kk_log_t* log, const char* msg)
;

/** Platform callback to get frame delta time. */
static uint32_t platform_get_time(platform_t* platform)
;

/** Loads a file. */
static boolean platform_load_file(const char* filename, boolean binary, long* out__size, void** out__buffer)
;

/**
Shuts down up the app.
*/
static void shutdown()
;

/**
Sets up the app.
*/
static void startup()
;

static void psp_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
;

void psp_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
;