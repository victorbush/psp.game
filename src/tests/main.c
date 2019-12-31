/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "log/log.h"
#include "tests/tests.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

log_t* g_log;
static log_t s_log;

/*=========================================================
FUNCTIONS
=========================================================*/

void lua_script_tests();
void utl_array_tests();
void utl_ringbuf_tests();

void main()
{
	g_log = &s_log;
	log__construct(g_log);

	RUN_TEST(lua_script_tests);
	RUN_TEST(utl_array_tests);
	RUN_TEST(utl_ringbuf_tests);

	printf("Press enter to continue...\n");
	int not_used = getchar();
}