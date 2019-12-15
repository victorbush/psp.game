/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "lua/lua_script.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

#define LOG_ERROR(msg)

/** Logs an error. */
static void log_error(lua_script_t* lua, const char* msg);

/** Pushes a single variable on the stack. */
static boolean push_single(lua_script_t* lua, const char* variable);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void lua_script__construct(lua_script_t* lua)
{
	clear_struct(lua);

	/* Create Lua state */
	lua->state = luaL_newstate();
	if (!lua->state)
	{
		FATAL("Failed to create Lua state.");
	}

	/* Open libs */
	luaL_openlibs(lua->state);
}

void lua_script__destruct(lua_script_t* lua)
{
	if (lua->state)
	{
		lua_close(lua->state);
	}
}

/*=========================================================
FUNCTIONS
=========================================================*/

boolean lua_script__cancel_loop(lua_script_t* lua)
{
	/* The a table interation must be in progress:
		[ 0]
		[-1] -- value
		[-2] -- key
		[-3] -- table
	*/
	if (lua_gettop(lua->state) < 3)
	{
		/* Unexpected state */
		return FALSE;
	}

	/* Pop the key/value and leave table */
	lua_pop(lua->state, 2);
	return TRUE;
}

boolean lua_script__execute_file(lua_script_t* lua, const char* file_path)
{
	/* Load and execute script file */
	if (luaL_dofile(lua->state, file_path))
	{
		//_log->Error("Failed to execute script '" + filePath + "'.");
		log_error(lua, "Failed to execute script.");
		return FALSE;
	}

	return TRUE;
}

boolean lua_script__execute_string(lua_script_t* lua, const char* str)
{
	/* Load and execute script from string */
	if (luaL_dostring(lua->state, str))
	{
		//log_error(lua, "Failed to execute script string '" + str + "'.");
		log_error(lua, "Failed to execute script string.");
		return FALSE;
	}

	return TRUE;
}

boolean lua_script__get_bool(lua_script_t* lua, boolean* out__val)
{
	if (!lua_isboolean(lua->state, -1))
	{
		log_error(lua, "Not a boolean.");
		*out__val = FALSE;
		return FALSE;
	}

	/* Get the value */
	*out__val = (boolean)lua_toboolean(lua->state, -1);
	return TRUE;
}

boolean lua_script__get_bool_var(lua_script_t* lua, const char* variable, boolean* out__val)
{
	uint32_t pushed = lua_script__push(lua, variable);
	if (!pushed)
	{
		*out__val = FALSE;
		FALSE;
	}

	boolean result = lua_script__get_bool(lua, out__val);

	lua_script__pop(lua, pushed);
	return result;
}

boolean lua_script__get_float(lua_script_t* lua, float* out__val)
{
	if (!lua_isnumber(lua->state, -1))
	{
		log_error(lua, "Not a number.");
		*out__val = 0.0f;
		return FALSE;
	}

	/* Get the value */
	*out__val = (float)lua_tonumber(lua->state, -1);
	return TRUE;
}

boolean lua_script__get_float_var(lua_script_t* lua, const char* variable, float* out__val)
{
	uint32_t pushed = lua_script__push(lua, variable);
	if (!pushed)
	{
		*out__val = 0.0f;
		return FALSE;
	}

	auto result = lua_script__get_float(lua, out__val);

	lua_script__pop(lua, pushed);
	return result;
}

boolean lua_script__get_int(lua_script_t* lua, int* out__val)
{
	if (!lua_isnumber(lua->state, -1))
	{
		log_error(lua, "Not a number.");
		*out__val = 0;
		return FALSE;
	}

	/* Get the value */
	*out__val = (int)lua_tonumber(lua->state, -1);
	return TRUE;
}

boolean lua_script__get_int_var(lua_script_t* lua, const char* variable, int* out__val)
{
	uint32_t pushed = lua_script__push(lua, variable);
	if (!pushed)
	{
		*out__val = 0;
		return FALSE;
	}

	boolean result = lua_script__get_int(lua, out__val);

	lua_script__pop(lua, pushed);
	return result;
}

boolean lua_script__get_key(lua_script_t* lua, char* out__key, int max_val_size)
{
	/* Need to have stack like this:
	[ 0]
	[-1] -- value
	[-2] -- key
	*/
	if (lua_gettop(lua->state) < 2)
	{
		strncpy_s(out__key, max_val_size, "", max_val_size - 1);
		return FALSE;
	}

	/* Push key to top */
	lua_pushvalue(lua->state, -2);

	/* Convert to string */
	char* key = lua_tostring(lua->state, -1);
	strncpy_s(out__key, max_val_size, key, max_val_size - 1);

	/* Pop the value */
	lua_pop(lua->state, 1);

	return TRUE;
}

boolean lua_script__get_string(lua_script_t* lua, char* out__str, int max_str_size)
{
	if (!lua_isstring(lua->state, -1))
	{
		log_error(lua, "Not a number.");
		strncpy_s(out__str, max_str_size, "", max_str_size - 1);
		return FALSE;
	}

	/* Get the value */
	char* str = lua_tostring(lua->state, -1);
	strncpy_s(out__str, max_str_size, str, max_str_size - 1);
	return TRUE;
}

boolean lua_script__get_string_var(lua_script_t* lua, const char* variable, char* out__str, int max_str_size)
{
	uint32_t pushed = lua_script__push(lua, variable);
	if (!pushed)
	{
		strncpy_s(out__str, max_str_size, "", max_str_size - 1);
		return FALSE;
	}

	boolean result = lua_script__get_string(lua, out__str, max_str_size);

	lua_script__pop(lua, pushed);
	return result;
}

boolean lua_script__next(lua_script_t* lua)
{
	/* Need to have stack like this:
		[ 0]
		[-1] -- previous value
		[-2] -- previous key
		[-3] -- table
	*/
	if (lua_gettop(lua->state) < 3)
	{
		return FALSE;
	}

	/* Pop the previous value */
	lua_pop(lua->state, 1);

	/* Pops previous key, replaces with next key. Then pushes value, if available.
		Result after call if next key is available:
		[ 0]
		[-1] -- value
		[-2] -- key
		[-3] -- table

		Result after call if next key not available:
		[ 0]
		[-1] -- table
	*/
	return lua_next(lua->state, -2);
}

uint32_t lua_script__pop(lua_script_t* lua, uint32_t num)
{
	int pop = (int)num;
	int top = lua_gettop(lua->state);

	if (top == 0)
	{
		/* Already at top */
		return 0;
	}

	if (pop > top)
	{
		/* Don't pop more than there actually are */
		pop = top;
	}

	lua_pop(lua->state, pop);
	return (uint32_t)pop;
}

uint32_t lua_script__pop_all(lua_script_t* lua)
{
	int top = lua_gettop(lua->state);
	lua_pop(lua->state, top);
	return top;
}

uint32_t lua_script__push(lua_script_t* lua, const char* variable)
{
	int num_pushed = 0;
	int var_idx = 0;
	int variable_len = strlen(variable);
	char* var = malloc(variable_len + 1);
	if (!var)
	{
		FATAL("Failed to allocated memory.");
	}

	for (int i = 0; i < variable_len; ++i)
	{
		boolean is_dot = variable[i] == '.';

		if (!is_dot)
		{
			/* Part of variable name */
			var[var_idx++] = variable[i];
		}

		if (is_dot || (i + 1) == variable_len)
		{
			/* At the end of a variable name, push it */
			var[var_idx] = 0;

			if (!push_single(lua, var))
			{
				/* Variable not found */
				lua_script__pop(lua, num_pushed);
				free(var);
				return 0;
			}

			num_pushed++;
			var_idx = 0;
		}
	}
	
	/* Success */
	free(var);
	return num_pushed;
}

boolean lua_script__start_loop(lua_script_t* lua)
{
	/* The table/array to iterate must already be on stack:
		[ 0]
		[-1] -- table
	*/
	if (lua_gettop(lua->state) == 0)
	{
		/* Nothing on stack, so nothing to push */
		return FALSE;
	}

	/* Push nil as initial key - this tells lua_next() to start at beginning */
	lua_pushnil(lua->state);

	/* Push nil as initial value */
	lua_pushnil(lua->state);

	return TRUE;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

static void log_error(lua_script_t* lua, const char* msg)
{
	/*lua_Debug d;
	lua_getinfo(lua->state, )*/
	// TODO
	LOG_ERROR(msg);
}

static boolean push_single(lua_script_t* lua, const char* variable)
{
	if (lua_gettop(lua->state) == 0)
	{
		/* Nothing below, find global variable */
		lua_getglobal(lua->state, variable);
	}
	else
	{
		/* Something already pushed, find a field */
		lua_getfield(lua->state, -1, variable);
	}

	/* Check if found */
	if (lua_isnil(lua->state, -1))
	{
		/* Not found */
		lua_script__pop(lua, 1);
		return FALSE;
	}

	/* Success */
	return TRUE;
}
