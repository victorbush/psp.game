/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>
#include <string.h>

#include "lua/lua_script.h"
#include "tests/tests.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/*-----------------------------------------------------
cancel_loop()
-----------------------------------------------------*/

static void cancel_loop__loop_in_progress__true_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11, [2] = 12 }");
	lua_script__push(&s, "table");
	lua_script__start_loop(&s);
	lua_script__next(&s);
	assert(TRUE == lua_script__cancel_loop(&s));
	assert(FALSE == lua_script__next(&s));

	lua_script__destruct(&s);
}

static void cancel_loop__stack_empty__false_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11, [2] = 12 }");
	lua_script__push(&s, "table");

	int val = 999;
	assert(FALSE == lua_script__cancel_loop(&s));
	assert(FALSE == lua_script__get_int(&s, &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_array_of_float()
-----------------------------------------------------*/

static void get_array_of_float__valid__array_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { 10.2, 11.3, 12.4 }");
	lua_script__push(&s, "table");

	float val[3];
	assert(TRUE == lua_script__get_array_of_float(&s, val, 3));
	assert(10.2f == val[0]);
	assert(11.3f == val[1]);
	assert(12.4f == val[2]);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_bool()
-----------------------------------------------------*/

static void get_bool__top__true_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = true, key2 = true }");
	lua_script__push(&s, "table.key1");

	boolean val = FALSE;
	assert(TRUE == lua_script__get_bool(&s, &val));
	assert(TRUE == val);

	lua_script__destruct(&s);
}

static void get_bool__top_empty__false_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	boolean val = TRUE;
	assert(FALSE == lua_script__get_bool(&s, &val));
	assert(FALSE == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_bool_var()
-----------------------------------------------------*/

static void get_bool_var__global_variable__bool_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	lua_script__execute_string(&s, "falseVar = false");
	lua_script__execute_string(&s, "trueVar = true");

	boolean val = TRUE;
	assert(TRUE == lua_script__get_bool_var(&s, "falseVar", &val));
	assert(FALSE == val);

	val = FALSE;
	assert(TRUE == lua_script__get_bool_var(&s, "trueVar", &val));
	assert(TRUE == val);

	lua_script__destruct(&s);
}

/*
table.nestedTable.key
*/
static void get_bool_var__nested_table__bool_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nestedTable = { key = true } }");

	boolean val = FALSE;
	assert(TRUE == lua_script__get_bool_var(&s, "table.nestedTable.key", &val));
	assert(TRUE == val);

	lua_script__destruct(&s);
}

static void get_bool_var__no_script__false_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	boolean val = TRUE;
	assert(FALSE == lua_script__get_bool_var(&s, "myvar", &val));
	assert(FALSE == val);

	lua_script__destruct(&s);
}

/*
table.key
*/
static void get_bool_var__table_dot__bool_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = true, key2 = true }");

	boolean val = FALSE;
	assert(TRUE == lua_script__get_bool_var(&s, "table.key1", &val));
	assert(TRUE == val);

	val = FALSE;
	assert(TRUE == lua_script__get_bool_var(&s, "table.key2", &val));
	assert(TRUE == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_float()
-----------------------------------------------------*/

static void get_float__top__float_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	lua_script__execute_string(&s, "table = { key1 = 4.34, key2 = 1.23 }");
	lua_script__push(&s, "table.key1");

	float val = 1.0f;
	assert(TRUE == lua_script__get_float(&s, &val));
	assert(4.34f == val);

	lua_script__destruct(&s);
}

static void get_float__top_empty__zero_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	float val = 1.0f;
	assert(FALSE == lua_script__get_float(&s, &val));
	assert(0.0f == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_float_var()
-----------------------------------------------------*/

static void get_float_var__global_variable__float_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "posVar = 2.1");
	lua_script__execute_string(&s, "negVar = -3.2");
	lua_script__execute_string(&s, "zeroVar = 0.0");

	float val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "posVar", &val));
	assert(2.1f == val);

	val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "negVar", &val));
	assert(-3.2f == val);

	val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "zeroVar", &val));
	assert(0.0f == val);

	lua_script__destruct(&s);
}

/*
table.nestedTable.key
*/
static void get_float_var__nested_table__float_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nestedTable = { key = 2.42312 } }");

	float val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "table.nestedTable.key", &val));
	assert(2.42312f == val);

	lua_script__destruct(&s);
}

static void get_float_var__no_script__zero_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	float val = 1.0f;
	assert(FALSE == lua_script__get_float_var(&s, "myvar", &val));
	assert(0.0f == val);

	lua_script__destruct(&s);
}

/*
table.key
*/
static void get_float_var__table_dot__float_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 4.34, key2 = 1.23 }");

	float val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "table.key1", &val));
	assert(4.34f == val);

	val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "table.key2", &val));
	assert(1.23f == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_int()
-----------------------------------------------------*/

static void get_int__top__int_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 2, key2 = 5 }");
	lua_script__push(&s, "table.key1");

	int val = 1;
	assert(TRUE == lua_script__get_int(&s, &val));
	assert(2 == val);

	lua_script__destruct(&s);
}

static void get_int__top_empty__zero_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	int val = 1;
	assert(FALSE == lua_script__get_int(&s, &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_int_var()
-----------------------------------------------------*/

static void get_int_var__get_int__int_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "posVar = 2");
	lua_script__execute_string(&s, "negVar = -3");
	lua_script__execute_string(&s, "zeroVar = 0");
	
	int val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "posVar", &val));
	assert(2 == val);

	val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "negVar", &val));
	assert(-3 == val);

	val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "zeroVar", &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*
table.nestedTable.key
*/
static void get_int_var__nested_table__int_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nestedTable = { key = 2312 } }");

	int val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "table.nestedTable.key", &val));
	assert(2312 == val);

	lua_script__destruct(&s);
}

static void get_int_var__no_script__zero_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	int val = 1;
	assert(FALSE == lua_script__get_int_var(&s, "myvar", &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*
table.key
*/
static void get_int_var__table_dot__int_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 2, key2 = 5 }");

	int val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "table.key1", &val));
	assert(2 == val);

	val = 1;
	assert(TRUE == lua_script__get_int_var(&s, "table.key2", &val));
	assert(5 == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_key()
-----------------------------------------------------*/

#define VERIFY_GET_KEY(expected_val, expected_return) \
	do { \
	char val[100]; \
	val[0] = 'a'; \
	assert(expected_return == lua_script__get_key(&s, val, sizeof(val))); \
	assert(!strncmp(val, expected_val, sizeof(val))); \
	} while(0)

static void get_key__only_one_pushed__fail()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "emptyStr = \"\"");
	lua_script__push(&s, "emptyStr");

	VERIFY_GET_KEY("", FALSE);

	lua_script__destruct(&s);
}

static void get_key__stack_empty__fail()
{
	lua_script_t s;
	lua_script__construct(&s);

	VERIFY_GET_KEY("", FALSE);

	lua_script__destruct(&s);
}

static void get_key__two_pushed__success()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11,  [2] = 12, [3] = 13 }");
	lua_script__push(&s, "table");

	/* NOTE: Using indices in the table since key ordering is not reliable */

	lua_script__start_loop(&s);
	lua_script__next(&s);
	VERIFY_GET_KEY("0", TRUE);

	lua_script__next(&s);
	VERIFY_GET_KEY("1", TRUE);

	lua_script__next(&s);
	VERIFY_GET_KEY("2", TRUE);

	lua_script__destruct(&s);
}

#undef VERIFY_GET_KEY

/*-----------------------------------------------------
get_string()
-----------------------------------------------------*/

static void get_string__top__string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = \"String 1\", key2 = \"String 2\" }");
	lua_script__push(&s, "table.key1");

	char val[100];
	val[0] = 'a';
	assert(TRUE == lua_script__get_string(&s, val, sizeof(val)));
	assert(!strncmp(val, "String 1", sizeof(val)));

	lua_script__destruct(&s);
}

static void get_string__top_empty__empty_string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	char val[100];
	val[0] = 'a';
	assert(FALSE == lua_script__get_string(&s, val, sizeof(val)));
	assert(!strncmp(val, "", sizeof(val)));

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
get_string_var()
-----------------------------------------------------*/

#define VERIFY_GET_STRING_VAR(var, expected_val, expected_return) \
	do { \
	char val[100]; \
	val[0] = 'a'; \
	assert(expected_return == lua_script__get_string_var(&s, var, val, sizeof(val))); \
	assert(!strncmp(val, expected_val, sizeof(val))); \
	} while(0)

static void get_string_var__global_variable__string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	lua_script__execute_string(&s, "emptyStr = \"\"");
	lua_script__execute_string(&s, "str = \"Hello, world!\"");
	VERIFY_GET_STRING_VAR("emptyStr", "", TRUE);
	VERIFY_GET_STRING_VAR("str", "Hello, world!", TRUE);

	lua_script__destruct(&s);
}

/*
table.nestedTable.key
*/
static void get_string_var__nested_table__string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);

	lua_script__execute_string(&s, "table = { nestedTable = { key = \"Hello, world!\" } }");
	VERIFY_GET_STRING_VAR("table.nestedTable.key", "Hello, world!", TRUE);

	lua_script__destruct(&s);
}

static void get_string_var__no_script__empty_string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	VERIFY_GET_STRING_VAR("myvar", "", FALSE);

	lua_script__destruct(&s);
}

/*
table.key
*/
static void get_string_var__table_dot__string_returned()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = \"String 1\", key2 = \"String 2\" }");
	VERIFY_GET_STRING_VAR("table.key1", "String 1", TRUE);
	VERIFY_GET_STRING_VAR("table.key2", "String 2", TRUE);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
next()
-----------------------------------------------------*/

static void next__empty_stack__nothing_pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11, [2] = 12 }");
	assert(FALSE == lua_script__next(&s));

	int val = 1;
	assert(FALSE == lua_script__get_int(&s, &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
pop()
-----------------------------------------------------*/

static void pop__nothing_to_pop__not_popped()
{
	lua_script_t s;
	lua_script__construct(&s);
	assert(0 == lua_script__pop(&s, 1));

	lua_script__destruct(&s);
}

static void pop__try_to_pop_more_than_able__valid_popped()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 1, key2 = 2 }");
	lua_script__push(&s, "table");
	assert(1 == lua_script__pop(&s, 5));

	lua_script__destruct(&s);
}

static void pop__something_to_pop__popped()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 1, key2 = 2 }");
	lua_script__push(&s, "table");
	assert(1 == lua_script__pop(&s, 1));

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
pop_all()
-----------------------------------------------------*/

static void pop_all__empty_stack__nothing_popped()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 1, key2 = 2 }");
	assert(0 == lua_script__pop_all(&s));

	lua_script__destruct(&s);
}

static void pop_all__populated_stack__all_popped()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 1, key2 = 2 }");
	lua_script__push(&s, "table.key1");
	assert(2 == lua_script__pop_all(&s));

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
push()
-----------------------------------------------------*/

static void push__global_table__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 1, key2 = 2 }");

	assert(1 == lua_script__push(&s, "table"));

	int val = 2;
	assert(TRUE == lua_script__get_int_var(&s, "key1", &val));
	assert(1 == val);

	val = 3;
	assert(TRUE == lua_script__get_int_var(&s, "key2", &val));
	assert(2 == val);

	lua_script__destruct(&s);
}

static void push__global_var__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "val = 1");
	assert(1 == lua_script__push(&s, "val"));

	lua_script__destruct(&s);
}

static void push__nested_table_one_push__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nest = { val = 2.3 } }");

	assert(1 == lua_script__push(&s, "table"));
	assert(1 == lua_script__push(&s, "nest"));

	float val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "val", &val));
	assert(2.3f == val);

	lua_script__destruct(&s);
}

static void push__nested_table_two_push__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nest = { val = 2.3 } }");

	assert(2 == lua_script__push(&s, "table.nest"));

	float val = 1.0f;
	assert(TRUE == lua_script__get_float_var(&s, "val", &val));
	assert(2.3f == val);

	lua_script__destruct(&s);
}

static void push__nested_var__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nest = { val = 2.3 } }");
	assert(3 == lua_script__push(&s, "table.nest.val"));

	lua_script__destruct(&s);
}

static void push__not_found__not_pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nest = { val = 2.3 } }");
	assert(0 == lua_script__push(&s, "notfound"));

	lua_script__destruct(&s);
}

static void push__not_found_nested__not_pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { nest = { val = 2.3 } }");
	assert(0 == lua_script__push(&s, "table.notfound"));

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
start_loop()
-----------------------------------------------------*/

static void start_loop__empty_stack__nothing_pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11, [2] = 12 }");
	assert(FALSE == lua_script__start_loop(&s));

	int val = 2;
	assert(FALSE == lua_script__get_int(&s, &val));
	assert(0 == val);

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
start_loop() / next()
-----------------------------------------------------*/

static void start_loop_next__index_keys_mix__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, key1 = 11, [2] = 12 }");
	assert(TRUE == lua_script__push(&s, "table"));

	/* NOTE: When using keys in tables (instead of all indices), the ordering
				is not guaranteed. */

	assert(TRUE == lua_script__start_loop(&s));
	assert(TRUE == lua_script__next(&s));
	assert(TRUE == lua_script__next(&s));
	assert(TRUE == lua_script__next(&s));

	assert(FALSE == lua_script__next(&s));	/* Should be end of array */
	assert(1 == lua_script__pop(&s, 5));	/* Should only be one element left on stack */

	lua_script__destruct(&s);
}

static void start_loop_next__index_only__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { [0] = 10, [1] = 11, [2] = 12 }");
	assert(TRUE == lua_script__push(&s, "table"));

	assert(TRUE == lua_script__start_loop(&s));
	assert(TRUE == lua_script__next(&s));
	{
		int val = 2;
		assert(TRUE == lua_script__get_int(&s, &val));
		assert(10 == val);
	}

	assert(TRUE == lua_script__next(&s));
	{
		int val = 2;
		assert(TRUE == lua_script__get_int(&s, &val));
		assert(11 == val);
	}

	assert(TRUE == lua_script__next(&s));
	{
		int val = 2;
		assert(TRUE == lua_script__get_int(&s, &val));
		assert(12 == val);
	}

	assert(FALSE == lua_script__next(&s));	/* Should be end of array */
	assert(1 == lua_script__pop(&s, 5));	/* Should only be one element left on stack */

	lua_script__destruct(&s);
}

static void start_loop_next__keys_only__pushed()
{
	lua_script_t s;
	lua_script__construct(&s);
	lua_script__execute_string(&s, "table = { key1 = 10, key2 = 11, key3 = 12 }");
	assert(TRUE == lua_script__push(&s, "table"));

	/* NOTE: When using keys in tables (instead of all indices), the ordering
				is not guaranteed. */

	assert(TRUE == lua_script__start_loop(&s));
	assert(TRUE == lua_script__next(&s));
	assert(TRUE == lua_script__next(&s));
	assert(TRUE == lua_script__next(&s));

	assert(FALSE == lua_script__next(&s));	/* Should be end of array */
	assert(1 == lua_script__pop(&s, 5));	/* Should only be one element left on stack */

	lua_script__destruct(&s);
}

/*-----------------------------------------------------
Other tests
-----------------------------------------------------*/

static void other__for_loop__loop_executed()
{
	lua_script_t s;
	lua_script__construct(&s);
	assert(TRUE == lua_script__execute_string(&s, "val = 0 \n for i = 0, 6, 1 do val = i end"));
	{
		int val = 2;
		assert(TRUE == lua_script__get_int_var(&s, "val", &val));
		assert(6 == val);
	}

	lua_script__destruct(&s);
}

/*=========================================================
TEST MAIN
=========================================================*/

void lua_script_tests()
{
	RUN_TEST_CASE(cancel_loop__loop_in_progress__true_returned);
	RUN_TEST_CASE(cancel_loop__stack_empty__false_returned);
	RUN_TEST_CASE(get_array_of_float__valid__array_returned);
	RUN_TEST_CASE(get_bool__top__true_returned);
	RUN_TEST_CASE(get_bool__top_empty__false_returned);
	RUN_TEST_CASE(get_bool_var__global_variable__bool_returned);
	RUN_TEST_CASE(get_bool_var__nested_table__bool_returned);
	RUN_TEST_CASE(get_bool_var__no_script__false_returned);
	RUN_TEST_CASE(get_bool_var__table_dot__bool_returned);
	RUN_TEST_CASE(get_float__top__float_returned);
	RUN_TEST_CASE(get_float__top_empty__zero_returned);
	RUN_TEST_CASE(get_float_var__global_variable__float_returned);
	RUN_TEST_CASE(get_float_var__nested_table__float_returned);
	RUN_TEST_CASE(get_float_var__no_script__zero_returned);
	RUN_TEST_CASE(get_float_var__table_dot__float_returned);
	RUN_TEST_CASE(get_int__top__int_returned);
	RUN_TEST_CASE(get_int__top_empty__zero_returned);
	RUN_TEST_CASE(get_int_var__get_int__int_returned);
	RUN_TEST_CASE(get_int_var__nested_table__int_returned);
	RUN_TEST_CASE(get_int_var__table_dot__int_returned);
	RUN_TEST_CASE(get_key__only_one_pushed__fail);
	RUN_TEST_CASE(get_key__stack_empty__fail);
	RUN_TEST_CASE(get_key__two_pushed__success);
	RUN_TEST_CASE(get_string__top__string_returned);
	RUN_TEST_CASE(get_string__top_empty__empty_string_returned);
	RUN_TEST_CASE(get_string_var__global_variable__string_returned);
	RUN_TEST_CASE(get_string_var__nested_table__string_returned);
	RUN_TEST_CASE(get_string_var__no_script__empty_string_returned);
	RUN_TEST_CASE(get_string_var__table_dot__string_returned);
	RUN_TEST_CASE(next__empty_stack__nothing_pushed);
	RUN_TEST_CASE(pop__nothing_to_pop__not_popped);
	RUN_TEST_CASE(pop__try_to_pop_more_than_able__valid_popped);
	RUN_TEST_CASE(pop__something_to_pop__popped);
	RUN_TEST_CASE(pop_all__empty_stack__nothing_popped);
	RUN_TEST_CASE(pop_all__populated_stack__all_popped);
	RUN_TEST_CASE(push__global_table__pushed);
	RUN_TEST_CASE(push__global_var__pushed);
	RUN_TEST_CASE(push__nested_table_one_push__pushed);
	RUN_TEST_CASE(push__nested_table_two_push__pushed);
	RUN_TEST_CASE(push__nested_var__pushed);
	RUN_TEST_CASE(push__not_found__not_pushed);
	RUN_TEST_CASE(push__not_found_nested__not_pushed);
	RUN_TEST_CASE(start_loop__empty_stack__nothing_pushed);
	RUN_TEST_CASE(start_loop_next__index_keys_mix__pushed);
	RUN_TEST_CASE(start_loop_next__index_only__pushed);
	RUN_TEST_CASE(start_loop_next__keys_only__pushed);
	RUN_TEST_CASE(other__for_loop__loop_executed);
}