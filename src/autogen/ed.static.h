
static void imgui_begin_frame(float delta_time, float width, float height)
;

static void imgui_end_frame(gpu_window_t* window, gpu_frame_t* frame)
;

static void run_render_system(_ed_t* ed, gpu_window_t* window, gpu_frame_t* frame)
;

static void ui_process_main_menu(_ed_t* ed)
;

static void ui_process_properties_pane(_ed_t* ed)
;

static _ed_dialog_result_t ui_show_open_file_dialog(_ed_ui_open_file_dialog_t* dialog, _ed_t* ed)
;

static void window_on_close(platform_window_t* window)
;

static void window_on_mouse_button
	(
	platform_window_t*			window,
	platform_mouse_button_t		button,
	platform_input_key_action_t action
	)
;

static void window_on_mouse_move(platform_window_t* window)
;
