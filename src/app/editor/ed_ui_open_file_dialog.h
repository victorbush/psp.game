#ifndef ED_UI_OPEN_FILE_DIALOG_H
#define ED_UI_OPEN_FILE_DIALOG_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/editor/ed_.h"
#include "app/editor/ed_ui_open_file_dialog_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ED_UI_OPEN_FILE_DIALOG__MAX_NUM_FILES	32

/*=========================================================
TYPES
=========================================================*/

struct _ed_ui_open_file_dialog_s
{
	_ed_t*				ed;

											/* The list of filenames currently shown in the dialog. */
	char				file_names[ED_UI_OPEN_FILE_DIALOG__MAX_NUM_FILES][256];
	boolean				is_visible;			/* Is the dialog visible? */
	int					num_files;			/* The number of files in the file_names array. */
	int					selected_index;		/* The index of the selected file in the file_names array. */
	_ed_dialog_state_t	state;				/* The dialog state. */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ed_ui_open_file_dialog.internal.h"

#endif /* ED_UI_OPEN_FILE_DIALOG_H */