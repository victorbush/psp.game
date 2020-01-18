/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/editor/ed_undo.h"
#include "log/log.h"
#include "utl/utl_ringbuf.h"

#include "autogen/ed_undo.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## internal
void _ed_undo__construct(_ed_undo_t* undo, uint32_t max_undos)
{
	clear_struct(undo);
	
	/* Allocate the undo buffer memory */
	undo->undo_buffer = malloc(sizeof(_ed_undo_cmd_t) * max_undos);
	if (!undo->undo_buffer)
	{
		log__fatal("Failed to allocate memory.");
	}

	/* Init undo ringbuffer metadata */
	utl_ringbuf_init(&undo->undo_buffer_meta, max_undos);
}

//## internal
void _ed_undo__destruct(_ed_undo_t* undo)
{
	/* Cleanup undo buffer and free any custom data pointers */
	while (!utl_ringbuf_is_empty(&undo->undo_buffer_meta))
	{
		uint32_t idx = utl_ringbuf_dequeue(&undo->undo_buffer_meta);
		free_if_need(undo, idx);
	}

	/* Free undo buffer */
	free(undo->undo_buffer);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _ed_undo__create_bool(_ed_undo_t* undo, void* context, boolean old_value, boolean new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.bool_val = old_value;
	cmd->new_value.bool_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_BOOL;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__create_float(_ed_undo_t* undo, void* context, float old_value, float new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.float_val = old_value;
	cmd->new_value.float_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_FLOAT;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__create_int32(_ed_undo_t* undo, void* context, int32_t old_value, int32_t new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.int32_val = old_value;
	cmd->new_value.int32_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_INT32;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__create_uint32(_ed_undo_t* undo, void* context, uint32_t old_value, uint32_t new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.uint32_val = old_value;
	cmd->new_value.uint32_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_UINT32;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__create_vec2(_ed_undo_t* undo, void* context, vec2_t old_value, vec2_t new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.vec2_val = old_value;
	cmd->new_value.vec2_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_VEC2;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__create_vec3(_ed_undo_t* undo, void* context, vec3_t old_value, vec3_t new_value, _ed_undo_func undo_func)
{
	_ed_undo_cmd_t* cmd = create(undo);
	cmd->old_value.vec3_val = old_value;
	cmd->new_value.vec3_val = new_value;
	cmd->type = ED_UNDO_CMD_TYPE_VEC3;
	cmd->undo_func = undo_func;
	cmd->context = context;
}

//## internal
void _ed_undo__redo(_ed_undo_t* undo)
{
	/* Check if anything to redo */
	if (undo->num_redos_left == 0)
	{
		return;
	}

	/* Execute the next redo command - keep old and new values as original since we are redoing */
	_ed_undo_cmd_t* cmd = &undo->undo_buffer[undo->redo_idx];
	cmd->undo_func(cmd->context, cmd->old_value, cmd->new_value, cmd->type);

	/* Update undo index */
	undo->num_undos_left = min(undo->num_undos_left + 1, undo->undo_buffer_meta.max_items);
	undo->undo_idx = undo->redo_idx;

	/* Update redo index */
	undo->num_redos_left--;
	undo->redo_idx = utl_ringbuf_calc_next_idx(&undo->undo_buffer_meta, undo->redo_idx);
}

//## internal
void _ed_undo__undo(_ed_undo_t* undo)
{
	/* Check if anything to undo */
	if (undo->num_undos_left == 0)
	{
		return;
	}

	/* Execute the next undo command - swap old and new values since we are undoing */
	_ed_undo_cmd_t* cmd = &undo->undo_buffer[undo->undo_idx];
	cmd->undo_func(cmd->context, cmd->new_value, cmd->old_value, cmd->type);

	/* Update redo index */
	undo->num_redos_left = min(undo->num_redos_left + 1, undo->undo_buffer_meta.max_items);
	undo->redo_idx = undo->undo_idx;

	/* Update undo index */
	undo->num_undos_left--;
	undo->undo_idx = utl_ringbuf_calc_prev_idx(&undo->undo_buffer_meta, undo->undo_idx);
}

//## static
static _ed_undo_cmd_t* create(_ed_undo_t* undo)
{
	/* Free any redo commands */
	while (undo->num_redos_left)
	{
		uint32_t idx = utl_ringbuf_dequeue_front(&undo->undo_buffer_meta);
		free_if_need(undo, idx);
		undo->num_redos_left--;
	}

	/* If buffer is full, remove the oldest entry */
	if (utl_ringbuf_is_full(&undo->undo_buffer_meta))
	{
		uint32_t idx = utl_ringbuf_dequeue(&undo->undo_buffer_meta);
		free_if_need(undo, idx);
	}

	/* Allocate an undo command and return it */
	uint32_t idx = utl_ringbuf_enqueue(&undo->undo_buffer_meta);
	
	undo->undo_idx = idx;
	undo->redo_idx = idx;
	undo->num_redos_left = 0;
	undo->num_undos_left = min(undo->num_undos_left + 1, undo->undo_buffer_meta.max_items);
	
	return &undo->undo_buffer[idx];
}

//## static
static void free_if_need(_ed_undo_t* undo, uint32_t idx)
{
	if (undo->undo_buffer[idx].type == ED_UNDO_CMD_TYPE_CUSTOM_AUTOFREE)
	{
		free(undo->undo_buffer[idx].old_value.custom_val);
		free(undo->undo_buffer[idx].new_value.custom_val);
	}
}
