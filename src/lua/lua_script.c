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

/** 
Copies a string from source to destination. 

@param dst The destination string. 
@param src The source string.
@param dst_size The size of the destination buffer (including null terminator).
*/
static void copy_string(char* dst, const char* src, int dst_size);

/** Callback for getting a float value. */
boolean get_array_float_value(lua_script_t* lua, void* out__val, int item_size);

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

boolean lua_script__get_array
	(
	lua_script_t*					lua,  
	lua_script__get_value_func_t	get_value_callback,
	void*							out__array, 
	int								item_size, 
	int								num_items
	)
{
	/* A table with N values should be on top of stack */
	if (!lua_istable(lua->state, -1))
	{
		log_error(lua, "Expected table on top of stack.");
		return FALSE;
	}

	/* Begin iterating through table */
	if (!lua_script__start_loop(lua))
	{
		return FALSE;
	}

	for (size_t i = 0; i < num_items; ++i)
	{
		if (!lua_script__next(lua))
		{
			/* Expected another float value */
			log_error(lua, "Fewer values in table than expected.");
			return FALSE;
		}

		/* Try to get value */
		if (!get_value_callback(lua, (unsigned char*)out__array + (item_size * i), item_size))
		{
			/* Expected a value */
			return FALSE;
		}
	}

	/* Expect this to be the end of the table */
	if (lua_script__next(lua))
	{
		// Pop off key/value
		lua_script__pop(lua, 2);

		log_error(lua, "More values in table than expected.");
		return FALSE;
	}

	/* Success */
	return TRUE;
}

boolean lua_script__get_array_of_float(lua_script_t* lua, float* out__val, int num_items)
{
	return lua_script__get_array(lua, get_array_float_value, out__val, sizeof(float), num_items);
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
		copy_string(out__key, "", max_val_size);
		return FALSE;
	}

	/* Push key to top */
	lua_pushvalue(lua->state, -2);

	/* Convert to string */
	const char* key = lua_tostring(lua->state, -1);
	copy_string(out__key, key, max_val_size);

	/* Pop the value */
	lua_pop(lua->state, 1);

	return TRUE;
}

boolean lua_script__get_string(lua_script_t* lua, char* out__str, int max_str_size)
{
	if (!lua_isstring(lua->state, -1))
	{
		log_error(lua, "Not a number.");
		copy_string(out__str, "", max_str_size);
		return FALSE;
	}

	/* Get the value */
	const char* str = lua_tostring(lua->state, -1);
	copy_string(out__str, str, max_str_size);
	return TRUE;
}

boolean lua_script__get_string_var(lua_script_t* lua, const char* variable, char* out__str, int max_str_size)
{
	uint32_t pushed = lua_script__push(lua, variable);
	if (!pushed)
	{
		copy_string(out__str, "", max_str_size);
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
	size_t variable_len = strlen(variable);
	char* var = malloc(variable_len + 1);
	if (!var)
	{
		FATAL("Failed to allocated memory.");
	}

	for (size_t i = 0; i < variable_len; ++i)
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
STATIC FUNCTIONS
=============================================================================*/

static void copy_string(char* dst, const char* src, int dst_size)
{
	/* 
	Calc max string length available in the destination buffer.
	Takes the null terminator into account. Cast to long first to prevent 
	possible arithmetic overflow.
	*/
	int str_len = max(0, (((long)dst_size) - 1));

	/* Do the copy */
	strncpy_s(dst, dst_size, src, str_len);
};

boolean get_array_float_value(lua_script_t* lua, void* out__val, int item_size)
{
	if (sizeof(float) != item_size)
	{
		FATAL("Item size does not match float size.");
	}

	float val = 0.0f;
	boolean result = lua_script__get_float(lua, &val);
	memcpy(out__val, &val, item_size);
	return result;
}

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
