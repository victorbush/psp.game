/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#include "tests/tests.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

static void test_utl_array_create()
{
	utl_array_create(string, string_array);

	assert(string_array.count == 0);
	assert(string_array.data == NULL);
	assert(string_array.max == 0);
}

static void test_utl_array_destroy()
{
	utl_array_create(int, int_array);
	utl_array_push(&int_array, 1);

	utl_array_destroy(&int_array);
	assert(int_array.count == 0);
	assert(int_array.data == NULL);
	assert(int_array.max == 0);
}

static void test_utl_array_resize()
{
	utl_array_create(int, int_array);

	/* resize array to hold 5 elements */
	utl_array_resize(&int_array, 5);

	assert(int_array.max == 5);
	assert(int_array.data != NULL);
	assert(int_array.count == 0);

	/* add items to array */
	utl_array_push(&int_array, 10);
	utl_array_push(&int_array, 11);
	utl_array_push(&int_array, 12);

	assert(int_array.max == 5);
	assert(int_array.data != NULL);
	assert(int_array.count == 3);

	/* resize array to hold 0 elements */
	utl_array_resize(&int_array, 0);

	assert(int_array.max == 0);
	assert(int_array.data == NULL);
	assert(int_array.count == 0);

	/* cleanup */
	utl_array_destroy(&int_array);
}

static void test_utl_array_push()
{
	utl_array_create(int, int_array);

	/* push on empty */
	utl_array_push(&int_array, 10);
	assert(int_array.count == 1);
	assert(int_array.max == 1);
	assert(int_array.data != NULL);

	/* push again */
	utl_array_push(&int_array, 11);
	assert(int_array.count == 2);
	assert(int_array.max == 2);

	/* push again */
	utl_array_push(&int_array, 12);
	assert(int_array.count == 3);
	assert(int_array.max == 4);

	/* push again - no realloc needed yet */
	utl_array_push(&int_array, 13);
	assert(int_array.count == 4);
	assert(int_array.max == 4);

	/* push again - realloc needed */
	utl_array_push(&int_array, 14);
	assert(int_array.count == 5);
	assert(int_array.max == 8);

	/* cleanup */
	utl_array_destroy(&int_array);
}

void utl_array_tests()
{
	RUN_TEST_CASE(test_utl_array_create);
	RUN_TEST_CASE(test_utl_array_destroy);
	RUN_TEST_CASE(test_utl_array_resize);
	RUN_TEST_CASE(test_utl_array_push);
}