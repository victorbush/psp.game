/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/editor/ed.h"
#include "app/editor/ed_undo.h"
#include "ecs/ecs.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "engine/camera.h"
#include "gpu/gpu.h"
#include "gpu/gpu_static_model.h"
#include "platform/platform.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/dirent/dirent.h"
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

	/* Setup ECS */
	ecs__construct(&ed->ecs);

	/* Create window */
	platform_window__construct(&ed->window, g_platform, g_gpu, 800, 600);
	platform_window__set_user_data(&ed->window, (void*)ed);
	platform_window__set_on_mouse_button_callback(&ed->window, window_on_mouse_button);
	platform_window__set_on_mouse_move_callback(&ed->window, window_on_mouse_move);
	platform_window__set_on_window_close_callback(&ed->window, window_on_close);
	
	/* Setup Camera */
	camera__construct(&ed->camera);

	/* Setup undo buffer */
	_ed_undo__construct(&ed->undo_buffer, ED__UNDO_BUFFER_NUM);
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

	camera__destruct(&ed->camera);
	platform_window__destruct(&ed->window, g_platform, g_gpu);
	ecs__destruct(&ed->ecs);

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
	float last_time = ed->frame_time;
	ed->frame_time = g_platform->get_time(g_platform);
	ed->frame_delta_time = ed->frame_time - last_time; // TODO : Rollover?

	/* Begin frame */
	gpu_frame_t* frame = gpu_window__begin_frame(&ed->window.gpu_window, &ed->camera, ed->frame_delta_time);

	imgui_begin_frame(ed->frame_delta_time, (float)ed->window.gpu_window.width, (float)ed->window.gpu_window.height);


	if (ed->world_is_open)
	{
		geo__render(&ed->world.geo, &ed->window.gpu_window, frame);
		run_render_system(ed, &ed->window.gpu_window, frame);
	}
	

	ui_process_main_menu(ed);
	ui_process_properties_pane(ed);

	if (ed->open_file_dialog.state != DIALOG_CLOSED)
	{
		if (ui_show_open_file_dialog(&ed->open_file_dialog, ed) == DIALOG_RESULT_OPEN)
		{
			/* Load new world */
			sprintf_s(ed->world_file_name, sizeof(ed->world_file_name), "worlds//%s", ed->open_file_dialog.file_names[ed->open_file_dialog.selected_index]);
			ed->world_is_changing = TRUE;
		}
	}


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
	/* Close existing world if open */
	if (!ed->world_is_open)
	{
		return;
	}

	gpu__wait_idle(g_gpu);
	world__destruct(&ed->world);
	ed->world_is_open = FALSE;
}

//## public
void _ed__open_world(_ed_t* ed, const char* world_file)
{
	/* Close existing world if needed */
	_ed__close_world(ed);

	/* Try open world */
	world__construct(&ed->world, &ed->ecs, world_file);
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
	ecs_t* ecs = &ed->ecs;
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
			log__fatal("Static model does not have a model assigned.");
		}

		/* Render the model */
		gpu_static_model__render(sm->model, g_gpu, window, frame, sm->material, transform);

		/* Convert entity id into an RGBA color */
		vec4_t color;
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
		/* File menu */
		if (igBeginMenu("File", TRUE))
		{
			if (igMenuItemBool("Open", NULL, FALSE, TRUE))
			{
				ed->open_file_dialog.state = DIALOG_OPENING;
			}

			if (igMenuItemBool("Exit", NULL, FALSE, TRUE))
			{
				ed->should_exit = TRUE;
			}

			igEndMenu();
		}

		igEndMainMenuBar();
	}
}

//## static
static void ui_process_properties_pane(_ed_t* ed)
{
	if (igBegin("Properties", NULL, 0))
	{
		if (ed->selected_entity == ECS_INVALID_ID)
		{
			igEnd();
			return;
		}

		ecs_t* ecs = &ed->ecs;

		igColumns(2, NULL, FALSE);
		igText("Id");
		igNextColumn();

		char id[12];
		_snprintf_s(id, sizeof(id), _TRUNCATE, "%i", ed->selected_entity);
		igText(id);
		
		igNextColumn();

		const char* key;
		map_iter_t iter = map_iter(&ecs->component_registry);
		while ((key = map_next(&ecs->component_registry, &iter)))
		{
			comp_intf_t** cached_comp = (gpu_static_model_t**)map_get(&ecs->component_registry, key);
			if (!cached_comp)
			{
				continue;
			}

			/* Dereference double pointer for simplicity */
			comp_intf_t* comp = *cached_comp;

			// TODO : Check if selected entity has this component
			igColumns(1, NULL, FALSE);
			igText(comp->name);
			//igCollapsingHeader(comp->name, 0);

			/* Make sure component supports enumerating property info */
			if (!comp->get_property)
			{
				continue;
			}

			uint32_t prop_idx = 0;
			ecs_component_prop_t prop_info;

			while (comp->get_property(ecs, ed->selected_entity, prop_idx, &prop_info))
			{
				switch (prop_info.type)
				{
				case ECS_COMPONENT_PROP_TYPE_VEC3:
					if (igInputFloat3(prop_info.name, (float*)prop_info.value, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
					{
						printf("hi");
					}

					break;
				}

				prop_idx++;
			}
		}

		igEnd();
	}
}

//## static
static _ed_dialog_result_t ui_show_open_file_dialog(_ed_ui_open_file_dialog_t* dialog, _ed_t* ed)
{
	_ed_dialog_result_t result = DIALOG_RESULT_IN_PROGRESS;

	if (dialog->state == DIALOG_OPENING)
	{
		dialog->num_files = 0;
		dialog->selected_index = 0;

		/* Get list of files in the worlds directory */
		DIR* dir;
		struct dirent* ent;

		if ((dir = opendir("worlds\\")) != NULL) 
		{
			int i = 0;
			while ((ent = readdir(dir)) != NULL && i < ED__UI__OPEN_FILE_DIALOG_NUM_FILES)
			{
				/* Skip if not a file */
				if (ent->d_type != DT_REG)
				{
					continue;
				}

				/* Copy filename to buffer */
				strncpy_s(dialog->file_names[i], ED__MAX_FILENAME_SIZE, ent->d_name, ED__MAX_FILENAME_SIZE - 1);
			}

			dialog->num_files = i + 1;
			closedir(dir);
		}
		else 
		{
			/* Could not open directory */
			log__error("Failed to open directory.");
		}

		/* Dialog opened */
		dialog->state = DIALOG_OPENED;
	}

	/* Render dialog */
	igOpenPopup("Open File");
	if (igBeginPopupModal("Open File", NULL, 0))
	{
		/* List files in directory */
		igPushItemWidth(-1.0f);
		igListBoxHeaderInt("Files", 10, -1);
		for (int i = 0; i < dialog->num_files; ++i)
		{
			ImVec2 size = { 0.0f, 0.0f };
			if (igSelectable(dialog->file_names[i], dialog->selected_index == i, 0, size))
			{
				dialog->selected_index = i;
			}
		}
		igListBoxFooter();

		/* Show Open/Cancel buttons */
		ImVec2 buttonSize;
		buttonSize.x = 80;
		buttonSize.y = 0;
		
		/* Open button - only show if a file is selected */
		if (dialog->num_files > 0)
		{
			if (igButton("Open", buttonSize))
			{
				dialog->state = DIALOG_CLOSED;
				result = DIALOG_RESULT_OPEN;
				strncpy_s(ed->world_file_name, ED__MAX_FILENAME_SIZE, dialog->file_names[dialog->selected_index], ED__MAX_FILENAME_SIZE - 1);
			}
		}

		igSameLine(0, -1);
		
		/* Close button */
		if (igButton("Cancel", buttonSize))
		{
			dialog->state = DIALOG_CLOSED;
			result = DIALOG_RESULT_CANCEL;
			igCloseCurrentPopup();
		}
	}
	igEndPopup();

	return result;
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

		camera__pan(&ed->camera, vert_delta, horiz_delta);
		ed->camera_is_moving = TRUE;
	}
	/* RIGHT mouse button */
	else if (rmb)
	{
		// Calc mouse change
		float rot_delta_x = ((float)y - prev_y) * rot_sen * -1.0f;
		float rot_delta_y = ((float)x - prev_x) * rot_sen * -1.0f;
		
		camera__rot_x(&ed->camera, rot_delta_x);
		camera__rot_y(&ed->camera, rot_delta_y);
		ed->camera_is_moving = TRUE;
	}
	/* LEFT mouse button */
	else if (lmb)
	{
		// Calc mouse change
		float rot_delta_y = ((float)x - prev_x) * rot_sen * -1.0f;
		float move_delta = ((float)y - prev_y) * move_sen * -1.0f;

		camera__rot_y(&ed->camera, rot_delta_y);
		camera__move(&ed->camera, move_delta);
		ed->camera_is_moving = TRUE;
	}
	else
	{
		ed->camera_is_moving = FALSE;
	}
}