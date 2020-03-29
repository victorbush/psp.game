/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "engine/kk_log.h"
#include "tests/tests.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

kk_log_t* g_log;
static kk_log_t s_log;

/*=========================================================
FUNCTIONS
=========================================================*/

void ed_undo_tests();
void lua_script_tests();
void utl_array_tests();
void utl_ringbuf_tests();

void main()
{
	g_log = &s_log;
	kk_log__construct(g_log);

	RUN_TEST(ed_undo_tests);
	RUN_TEST(lua_script_tests);
	RUN_TEST(utl_array_tests);
	RUN_TEST(utl_ringbuf_tests);

	printf("Press enter to continue...\n");
	int not_used = getchar();
}