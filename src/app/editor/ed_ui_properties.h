#ifndef ED_UI_PROPERTIES_H
#define ED_UI_PROPERTIES_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/editor/ed_.h"
#include "app/editor/ed_ui_properties_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct _ed_ui_properties_s
{
	_ed_t*		ed;

	boolean		is_visible;		/* Is the properties pane visible? */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ed_ui_properties.internal.h"

#endif /* ED_UI_PROPERTIES_H */