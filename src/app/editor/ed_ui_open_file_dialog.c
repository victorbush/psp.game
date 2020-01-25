/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/editor/ed.h"
#include "app/editor/ed_ui_open_file_dialog.h"
#include "log/log.h"
#include "thirdparty/dirent/dirent.h"

#include "autogen/ed_ui_open_file_dialog.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## internal
void _ed_ui_open_file_dialog__construct(_ed_ui_open_file_dialog_t* dialog, _ed_t* ed)
{
	clear_struct(dialog);
	dialog->ed = ed;
}

//## internal
void _ed_ui_open_file_dialog__destruct(_ed_ui_open_file_dialog_t* dialog)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _ed_ui_open_file_dialog__think(_ed_ui_open_file_dialog_t* dialog)
{
	if (dialog->state == DIALOG_CLOSED)
	{
		return;
	}

	if (show_dialog(dialog) == DIALOG_RESULT_OPEN)
	{
		/* Load new world */
		sprintf_s(dialog->ed->world_file_name, sizeof(dialog->ed->world_file_name), "worlds//%s", dialog->file_names[dialog->selected_index]);
		dialog->ed->world_is_changing = TRUE;
	}
}

//## static
static _ed_dialog_result_t show_dialog(_ed_ui_open_file_dialog_t* dialog)
{
	_ed_t* ed = dialog->ed;
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
			while ((ent = readdir(dir)) != NULL && i < cnt_of_array(dialog->file_names))
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
