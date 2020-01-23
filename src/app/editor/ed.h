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

#include "app/editor/ed_undo.h"
#include "ecs/ecs.h"
#include "engine/camera.h"
#include "engine/world.h"
#include "platform/platform_window.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ED__MAX_FILENAME_SIZE 256
#define ED__UI__OPEN_FILE_DIALOG_NUM_FILES	32
#define ED__UNDO_BUFFER_NUM 128

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
	/*
	Create/destroy
	*/
	camera_t			camera;
	platform_window_t	window;
	world_t				world;

	/*
	Other
	*/
	float				frame_time;			/* Frame timestamp (in seconds) */
	float				frame_delta_time;	/* Time between current frame and last frame (in seconds) */
	boolean				should_exit;		/* Should the app exit? */
											/* File name of the current world file. */
	char				world_file_name[ED__MAX_FILENAME_SIZE];
	boolean				world_is_changing;
	boolean				world_is_open;		/* Is a world file currently loaded? */
	
	boolean				camera_is_moving;

	_ed_ui_open_file_dialog_t	open_file_dialog;


	entity_id_t			selected_entity;

	_ed_undo_t			undo_buffer;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ed.public.h"

#endif /* ED_H */