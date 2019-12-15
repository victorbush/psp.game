/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "tests/tests.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void lua_script_tests();
void utl_array_tests();
void utl_ringbuf_tests();

void main()
{
	RUN_TEST(lua_script_tests);
	RUN_TEST(utl_array_tests);
	RUN_TEST(utl_ringbuf_tests);

	printf("Press enter to continue...\n");
	int not_used = getchar();
}