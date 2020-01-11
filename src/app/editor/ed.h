#ifndef ED_H
#define ED_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/app_.h"
#include "app/editor/ed_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "engine/camera.h"
#include "engine/world.h"
#include "platform/platform_window.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ED__MAX_FILENAME_SIZE 256
#define ED__UI__OPEN_FILE_DIALOG_NUM_FILES	32

/*=========================================================
TYPES
=========================================================*/

enum _ed_dialog_state_e
{
	DIALOG_CLOSED,
	DIALOG_OPENING,
	DIALOG_OPENED
};

enum _ed_dialog_result_e
{
	DIALOG_RESULT_IN_PROGRESS,	/* Dialog is still open */
	DIALOG_RESULT_CANCEL,
	DIALOG_RESULT_OK,
	DIALOG_RESULT_OPEN,
	DIALOG_RESULT_SAVE,
};

/**
Open file dialog.
*/
struct _ed_ui_open_file_dialog_s
{
	char				file_names[ED__UI__OPEN_FILE_DIALOG_NUM_FILES][256];
	int					num_files;
	int					selected_index;
	_ed_dialog_state_t	state;
};

/**
Editor.
*/
struct _ed_s
{
	camera_t			camera;
	ecs_t				ecs;
	platform_window_t	window;
	world_t				world;
	char				world_file_name[ED__MAX_FILENAME_SIZE];
	boolean				world_is_open;

	float				frame_time;			/* Frame timestamp (in seconds) */
	float				frame_delta_time;	/* Time between current frame and last frame (in seconds) */

	_ed_ui_open_file_dialog_t	open_file_dialog;


};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ed__construct(app_t* app);

void ed__destruct(app_t* app);

/*=========================================================
FUNCTIONS
=========================================================*/

void ed__init_app_intf(app_intf_t* intf);

void ed__run_frame(app_t* app);

_ed_t* _ed__from_base(app_t* app);

#endif /* ED_H */