/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#include "app/editor/ed_undo.h"
#include "tests/tests.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	uint32_t	old_val;
	uint32_t	new_val;

} test_context_t;

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

static void uint32_cmd(void* context, _ed_undo_cmd_val_t old_val, _ed_undo_cmd_val_t new_val, _ed_undo_cmd_val_type_t type)
{
	test_context_t* ctx = (test_context_t*)context;
	ctx->old_val = old_val.uint32_val;
	ctx->new_val = new_val.uint32_val;
	assert(type == ED_UNDO_CMD_TYPE_UINT32);
}

static void test_redo()
{
	/* Setup test undo buffer */
	_ed_undo_t undo;
	_ed_undo__construct(&undo, 5);

	/* Setup a test context */
	test_context_t ctx;
	clear_struct(&ctx);
	ctx.old_val = 20;
	ctx.new_val = 30;

	/* Nothing to reo */
	_ed_undo__redo(&undo);
	assert(ctx.old_val == 20);
	assert(ctx.new_val == 30);

	/* One thing to redo */
	_ed_undo__redo(&undo);
	assert(ctx.old_val == 20);
	assert(ctx.new_val == 30);

	/* Fill buffer exactly, undo everything, redo everything */
	_ed_undo__create_uint32(&undo, &ctx, 10, 11, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 11, 12, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 12, 13, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 13, 14, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 14, 15, uint32_cmd);

	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 11);
	assert(ctx.new_val == 10);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 10);
	assert(ctx.new_val == 11);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 11);
	assert(ctx.new_val == 12);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 12);
	assert(ctx.new_val == 13);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 13);
	assert(ctx.new_val == 14);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 14);
	assert(ctx.new_val == 15);

	/* Fill buffer to wrap around, undo everything, redo everything */
	_ed_undo__create_uint32(&undo, &ctx, 30, 31, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 31, 32, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 32, 33, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 33, 34, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 34, 35, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 35, 36, uint32_cmd);

	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 32);
	assert(ctx.new_val == 31);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 31);
	assert(ctx.new_val == 32);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 32);
	assert(ctx.new_val == 33);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 33);
	assert(ctx.new_val == 34);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 34);
	assert(ctx.new_val == 35);

	_ed_undo__redo(&undo);
	assert(ctx.old_val == 35);
	assert(ctx.new_val == 36);
}

static void test_undo()
{
	/* Setup test undo buffer */
	_ed_undo_t undo;
	_ed_undo__construct(&undo, 5);

	/* Setup a test context */
	test_context_t ctx;
	clear_struct(&ctx);
	ctx.old_val = 20;
	ctx.new_val = 30;

	/* Nothing to undo */
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 20);
	assert(ctx.new_val == 30);

	/* One thing to undo */
	_ed_undo__create_uint32(&undo, &ctx, 1, 2, uint32_cmd);
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 2);
	assert(ctx.new_val == 1);

	/* Nothing to undo */
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 2);
	assert(ctx.new_val == 1);

	/* Fill buffer exactly (5 times) */
	_ed_undo__create_uint32(&undo, &ctx, 10, 11, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 11, 12, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 12, 13, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 13, 14, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 14, 15, uint32_cmd);

	/* Undo everything */
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 15);
	assert(ctx.new_val == 14);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 14);
	assert(ctx.new_val == 13);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 13);
	assert(ctx.new_val == 12);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 12);
	assert(ctx.new_val == 11);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 11);
	assert(ctx.new_val == 10);

	/* Fill buffer until wrap around (6 times) */
	_ed_undo__create_uint32(&undo, &ctx, 10, 11, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 11, 12, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 12, 13, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 13, 14, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 14, 15, uint32_cmd);
	_ed_undo__create_uint32(&undo, &ctx, 15, 16, uint32_cmd);

	/* Undo everything */
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 16);
	assert(ctx.new_val == 15);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 15);
	assert(ctx.new_val == 14);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 14);
	assert(ctx.new_val == 13);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 13);
	assert(ctx.new_val == 12);

	_ed_undo__undo(&undo);
	assert(ctx.old_val == 12);
	assert(ctx.new_val == 11);

	/* Nothing to undo */
	_ed_undo__undo(&undo);
	assert(ctx.old_val == 12);
	assert(ctx.new_val == 11);

	/* Cleanup */
	_ed_undo__destruct(&undo);
}

void ed_undo_tests()
{
	RUN_TEST_CASE(test_redo);
	RUN_TEST_CASE(test_undo);
}