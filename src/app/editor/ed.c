/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/editor/ed.h"
#include "app/editor/ed_cmd.h"
#include "app/editor/ed_undo.h"
#include "app/editor/ed_ui_open_file_dialog.h"
#include "app/editor/ed_ui_properties.h"
#include "ecs/ecs.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_static_model.h"
#include "platform/platform.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/rxi_map/src/map.h"
#include "utl/utl.h"

// TODO ?
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/models/vlk_static_model.h"

#include "autogen/ed.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
void ed__construct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);
	ed->selected_entity = ECS_INVALID_ID;

	/* Create window */
	platform_window__construct(&ed->window, g_platform, g_gpu, 800, 600);
	platform_window__set_user_data(&ed->window, (void*)ed);
	platform_window__set_on_mouse_button_callback(&ed->window, window_on_mouse_button);
	platform_window__set_on_mouse_move_callback(&ed->window, window_on_mouse_move);
	platform_window__set_on_window_close_callback(&ed->window, window_on_close);
	
	/* Setup Camera */
	kk_camera__construct(&ed->camera);

	/* Setup undo buffer */
	_ed_undo__construct(&ed->undo_buffer, ED__UNDO_BUFFER_NUM);

	/* Setup UI components */
	_ed_ui_open_file_dialog__construct(&ed->open_file_dialog, ed);
	_ed_ui_properties__construct(&ed->properties_dialog, ed);
}

//## public
void ed__destruct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	gpu__wait_idle(g_gpu);

	/* Close world */
	_ed__close_world(ed);
	
	/* Cleanup undo buffer*/
	_ed_undo__destruct(&ed->undo_buffer);

	/* Cleanup UI components */
	_ed_ui_properties__destruct(&ed->properties_dialog);
	_ed_ui_open_file_dialog__destruct(&ed->open_file_dialog);



	kk_camera__destruct(&ed->camera);
	platform_window__destruct(&ed->window, g_platform, g_gpu);

	/* Free context memory */
	free(app->intf->context);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void ed__init_app_intf(app_intf_t* intf)
{
	clear_struct(intf);

	/* Allocate context memory */
	intf->context = malloc(sizeof(_ed_t));
	clear_struct((_ed_t*)intf->context);
	intf->__construct = ed__construct;
	intf->__destruct = ed__destruct;
	intf->__run_frame = ed__run_frame;
	intf->__should_exit = ed__should_exit;
}

//## public
void ed__run_frame(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	/* Get frame time delta */
	ed->frame_delta_time = g_platform->get_delta_time(g_platform);

	/* Begin frame */
	gpu_frame_t* frame = gpu_window__begin_frame(&ed->window.gpu_window, &ed->camera, ed->frame_delta_time);

	imgui_begin_frame(ed->frame_delta_time, (float)ed->window.gpu_window.width, (float)ed->window.gpu_window.height);


	if (ed->world_is_open)
	{
		geo__render(&ed->world.geo, &ed->window.gpu_window, frame);
		run_render_system(ed, &ed->window.gpu_window, frame);
	}
	

	ui_process_main_menu(ed);
	_ed_ui_properties__think(&ed->properties_dialog);
	_ed_ui_open_file_dialog__think(&ed->open_file_dialog);




	imgui_end_frame(&ed->window.gpu_window, frame);

	/* End frame */
	gpu_window__end_frame(&ed->window.gpu_window, frame);


	if (ed->world_is_changing)
	{
		_ed__open_world(ed, ed->world_file_name);
	}
}

//## public
boolean ed__should_exit(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);
	return (ed->should_exit);
}

//## public
_ed_t* _ed__from_base(app_t* app)
{
	return (_ed_t*)app->intf->context;
}

//## public
void _ed__close_world(_ed_t* ed)
{
	// TODO ? Reset undo buffer???

	/* Close existing world if open */
	if (!ed->world_is_open)
	{
		return;
	}

	gpu__wait_idle(g_gpu);
	kk_world__destruct(&ed->world);
	ed->world_is_open = FALSE;
}

//## public
void _ed__open_world(_ed_t* ed, const char* world_file)
{
	/* Close existing world if needed */
	_ed__close_world(ed);

	/* Try open world */
	kk_world__construct(&ed->world, world_file);
	ed->world_is_open = TRUE;
	ed->world_is_changing = FALSE;
}

//## static
static void imgui_begin_frame(float delta_time, float width, float height)
{
	ImGuiIO* io = igGetIO();

	// Setup low-level inputs, e.g. on Win32: calling GetKeyboardState(), or write to those fields from your Windows message handlers, etc.
	// (In the examples/ app this is usually done within the ImGui_ImplXXX_NewFrame() function from one of the demo Platform bindings)
	io->DeltaTime = delta_time;              // set the time elapsed since the previous frame (in seconds)
	io->DisplaySize.x = width;				// set the current display width
	io->DisplaySize.y = height;             // set the current display height here
	
	// Call NewFrame(), after this point you can use ImGui::* functions anytime
	// (So you want to try calling NewFrame() as early as you can in your mainloop to be able to use imgui everywhere)
	igNewFrame();
}

//## static
static void imgui_end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
	/* End imgui frame */
	igEndFrame();
	igRender();
	ImDrawData* draw_data = igGetDrawData();
	gpu_window__render_imgui(window, frame, draw_data);
}

//## static
static void run_render_system(_ed_t* ed, gpu_window_t* window, gpu_frame_t* frame)
{
	ecs_t* ecs = &ed->world.ecs;
	ecs_static_model_t* sm;
	ecs_transform_t* transform;
	uint32_t i;

	for (i = 0; i < ecs->next_free_id; ++i)
	{
		sm = &ecs->static_model_comp[i];
		transform = &ecs->transform_comp[i];

		/* Find entities with static model and transform */
		if (!sm->base.is_used || !transform->base.is_used)
		{
			continue;
		}

		/* Make sure model is loaded */
		if (!sm->model)
		{
			kk_log__fatal("Static model does not have a model assigned.");
		}

		/* Render the model */
		gpu_static_model__render(sm->model, g_gpu, window, frame, sm->material, transform);

		/* Convert entity id into an RGBA color */
		kk_vec4_t color;
		utl_unpack_rgba_float(i, color);

		/* Render picker buffer */
		vlk_static_model__render_to_picker_buffer(sm->model, g_gpu, window, frame, color, transform);
	}
}

//## static
static void ui_process_main_menu(_ed_t* ed)
{
	if (igBeginMainMenuBar())
	{
		/* 
		File menu 
		*/
		if (igBeginMenu("File", TRUE))
		{
			if (igMenuItemBool("Open", NULL, FALSE, TRUE))
			{
				ed->open_file_dialog.state = DIALOG_OPENING;
			}

			if (igMenuItemBool("Save", NULL, FALSE, ed->world_is_open))
			{
				kk_world__export_lua(&ed->world, ed->world_file_name);
			}

			if (igMenuItemBool("Exit", NULL, FALSE, TRUE))
			{
				ed->should_exit = TRUE;
			}

			igEndMenu();
		}

		/*
		Edit menu
		*/
		if (igBeginMenu("Edit", TRUE))
		{
			if (igMenuItemBool("Undo", "Ctrl+Z", FALSE, _ed_undo__can_undo(&ed->undo_buffer)))
			{
				_ed_undo__undo(&ed->undo_buffer);
			}

			if (igMenuItemBool("Redo", "Ctrl+Y", FALSE, _ed_undo__can_redo(&ed->undo_buffer)))
			{
				_ed_undo__redo(&ed->undo_buffer);
			}

			igEndMenu();
		}

		igEndMainMenuBar();
	}
}

//## static
static void window_on_close(platform_window_t* window)
{
	_ed_t* ed = (_ed_t*)platform_window__get_user_data(window);
	ed->should_exit = TRUE;
}

//## static
static void window_on_mouse_button
	(
	platform_window_t*			window,
	platform_mouse_button_t		button,
	platform_input_key_action_t action
	)
{
	_ed_t* ed = (_ed_t*)platform_window__get_user_data(window);

	if (!ed->camera_is_moving && action == KEY_ACTION_RELEASE && button == MOUSE_BUTTON_LEFT)
	{
		gpu_frame_t* frame = &window->gpu_window.frames[window->gpu_window.frame_idx];
		ed->selected_entity = vlk_window__get_picker_id(&window->gpu_window, frame, window->mouse_x, window->mouse_y);
	}
}

//## static
static void window_on_mouse_move(platform_window_t* window)
{
	_ed_t* ed = (_ed_t*)platform_window__get_user_data(window);
	
	float move_sen = 0.05f;
	float rot_sen = 0.30f;

	float x = window->mouse_x;
	float y = window->mouse_y;
	float prev_x = window->mouse_x_prev;
	float prev_y = window->mouse_y_prev;

	bool lmb = window->mouse_down[MOUSE_BUTTON_LEFT];
	bool rmb = window->mouse_down[MOUSE_BUTTON_RIGHT];

	/* RIGHT + LEFT mouse buttons */
	if (rmb && lmb)
	{
		float vert_delta = ((float)y - prev_y) * move_sen * -1.0f;
		float horiz_delta = ((float)x - prev_x) * move_sen;

		kk_camera__pan(&ed->camera, vert_delta, horiz_delta);
		ed->camera_is_moving = TRUE;
	}
	/* RIGHT mouse button */
	else if (rmb)
	{
		// Calc mouse change
		float rot_delta_x = ((float)y - prev_y) * rot_sen * -1.0f;
		float rot_delta_y = ((float)x - prev_x) * rot_sen * -1.0f;
		
		kk_camera__rot_x(&ed->camera, rot_delta_x);
		kk_camera__rot_y(&ed->camera, rot_delta_y);
		ed->camera_is_moving = TRUE;
	}
	/* LEFT mouse button */
	else if (lmb)
	{
		// Calc mouse change
		float rot_delta_y = ((float)x - prev_x) * rot_sen * -1.0f;
		float move_delta = ((float)y - prev_y) * move_sen * -1.0f;

		kk_camera__rot_y(&ed->camera, rot_delta_y);
		kk_camera__move(&ed->camera, move_delta);
		ed->camera_is_moving = TRUE;
	}
	else
	{
		ed->camera_is_moving = FALSE;
	}
}