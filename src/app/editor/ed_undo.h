#ifndef ED_UNDO_H
#define ED_UNDO_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/editor/ed_undo_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/**
Types of values for an undo command.
*/
enum _ed_undo_cmd_val_type_e
{
	ED_UNDO_CMD_TYPE_BOOL,
	ED_UNDO_CMD_TYPE_FLOAT,
	ED_UNDO_CMD_TYPE_INT32,
	ED_UNDO_CMD_TYPE_UINT32,		
	ED_UNDO_CMD_TYPE_VEC2,
	ED_UNDO_CMD_TYPE_VEC3,
	ED_UNDO_CMD_TYPE_CUSTOM_AUTOFREE,	/* Old and new value pointers are automatically free when removed from the undo buffer. */

	ED_UNDO_CMD_TYPE__COUNT
};

/**
Union of different command values.
*/
union _ed_undo_cmd_val_u
{
	boolean		bool_val;
	float		float_val;
	int32_t		int32_val;
	uint32_t	uint32_val;
	kk_vec2_t	vec2_val;
	kk_vec3_t	vec3_val;
	void*		custom_val;
};

/**
An editor command used to track command history for undo/redo.
*/
struct _ed_undo_cmd_s
{
	_ed_undo_cmd_val_t		old_value;
	_ed_undo_cmd_val_t		new_value;
	
	void*					context;		/* Context pointer sent to undo func callback. */
	_ed_undo_cmd_val_type_t	type;			/* The type of data value in this command. */
	_ed_undo_func			undo_func;		/* The function that undo/redo calls. */
};

/**
Undo buffer.
*/
struct _ed_undo_s
{
	_ed_undo_cmd_t*		undo_buffer;
	utl_ringbuf_t		undo_buffer_meta;

	uint32_t			undo_idx;			/* Index of the next undo command. */
	uint32_t			redo_idx;			/* Index of the next undo command. */
	uint32_t			num_redos_left;		/* Number of available redo operations remaining. */
	uint32_t			num_undos_left;		/* Number of available undo operations remaining. */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ed_undo.internal.h"

#endif /* ED_UNDO_H */