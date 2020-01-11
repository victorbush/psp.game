/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/editor/ed.h"
#include "ecs/ecs.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "gpu/gpu.h"
#include "platform/platform.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/dirent/dirent.h"

#include "autogen/ed.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ed__construct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	/* Setup ECS */
	ecs__construct(&ed->ecs);

	/* Create window */
	platform_window__construct(&ed->window, g_platform, g_gpu, SCREEN_WIDTH, SCREEN_HEIGHT);

	/* Setup Camera */
	camera__construct(&ed->camera);

	// TODO
	world__construct(&ed->world, &ed->ecs, "worlds/world.lua");
}

void ed__destruct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	gpu__wait_idle(g_gpu);

	world__destruct(&ed->world);
	camera__destruct(&ed->camera);
	platform_window__destruct(&ed->window, g_platform, g_gpu);
	ecs__destruct(&ed->ecs);

	/* Free context memory */
	free(app->intf->context);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void ed__init_app_intf(app_intf_t* intf)
{
	clear_struct(intf);

	/* Allocate context memory */
	intf->context = malloc(sizeof(_ed_t));
	clear_struct((_ed_t*)intf->context);
	intf->__construct = ed__construct;
	intf->__destruct = ed__destruct;
	intf->__run_frame = ed__run_frame;
}

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

	//player_system__run(&ed->ecs);

	if (ed->world_file_name[0])
	{
		geo__render(&ed->world.geo, &ed->window.gpu_window, frame);
		render_system__run(&ed->ecs, &ed->window.gpu_window, frame);
	}
	

	ui_show_main_menu(ed);

	if (ed->open_file_dialog.state != DIALOG_CLOSED)
	{
		if (ui_show_open_file_dialog(&ed->open_file_dialog, ed) == DIALOG_RESULT_OPEN)
		{
			/* Close existing file if needed */
			if (ed->world_file_name[0])
			{
				world__destruct(&ed->world);
			}

			/* Load new world */
			sprintf_s(ed->world_file_name, sizeof(ed->world_file_name), "worlds//%s", ed->open_file_dialog.file_names[ed->open_file_dialog.selected_index]);
			world__construct(&ed->world, &ed->ecs, ed->world_file_name);
		}
	}


	imgui_end_frame(&ed->window.gpu_window, frame);

	/* End frame */
	gpu_window__end_frame(&ed->window.gpu_window, frame);
}

_ed_t* _ed__from_base(app_t* app)
{
	return (_ed_t*)app->intf->context;
}

void _ed__open_world(_ed_t* ed, const char* world_file)
{
	/* Close existing world if needed */
	if (ed->world_is_open)
	{
		world__destruct(&ed->world);
		ed->world_is_open = FALSE;
	}

	/* Try open world */
	world__construct(&ed->world, &ed->ecs, world_file);
}

//## static
static void imgui_begin_frame(float delta_time, float width, float height)
//##
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
//##
{
	/* End imgui frame */
	igEndFrame();
	igRender();
	ImDrawData* draw_data = igGetDrawData();
	gpu_window__render_imgui(window, frame, draw_data);
}

//## static
static void ui_show_main_menu(_ed_t* ed)
//##
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
			{} // TODO

			igEndMenu();
		}

		igEndMainMenuBar();
	}
}

//## static
static _ed_dialog_result_t ui_show_open_file_dialog(_ed_ui_open_file_dialog_t* dialog, _ed_t* ed)
//##
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