#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "thirdparty/lua/lua.h"
#include "thirdparty/lua/lualib.h"
#include "thirdparty/lua/lauxlib.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	lua_State*					state;

} lua_script_t;

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a Lua script context.

@param lua The Lua script context.
*/
void lua_script__construct(lua_script_t* lua);

/**
Destructs a Lua script context.

@param lua The Lua script context.
*/
void lua_script__destruct(lua_script_t* lua);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Cancels loop iteration. Must be preceeded by either 
start_loop() or next().

@param lua The Lua script context.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__cancel_loop(lua_script_t* lua);

/**
Gets the value from the top of the stack as a boolean.

@param lua The Lua script context.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_bool(lua_script_t* lua, boolean* out__val);

/**
Gets the value from the specified variable as a boolean.

@param lua The Lua script context.
@param variable The name or path of the variable to get. Must be a null-terminated string.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_bool_var(lua_script_t* lua, const char* variable, boolean* out__val);

/**
Executes a script file.

@param lua The Lua script context.
@param file_path The file to execute.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__execute_file(lua_script_t* lua, const char* file_path);

/**
Executes a script from a string.

@param lua The Lua script context.
@param str The string to execute. Must be a null-termined string.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__execute_string(lua_script_t* lua, const char* str);

/**
Gets the value from the top of the stack as a float.

@param lua The Lua script context.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_float(lua_script_t* lua, float* out__val);

/**
Gets the value from the specified variable as a float.

@param lua The Lua script context.
@param variable The name or path of the variable to get. Must be a null-terminated string.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_float_var(lua_script_t* lua, const char* variable, float* out__val);

/**
Gets the value from the top of the stack as a int.

@param lua The Lua script context.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_int(lua_script_t* lua, int* out__val);

/**
Gets the value from the specified variable as a int.

@param lua The Lua script context.
@param variable The name or path of the variable to get. Must be a null-terminated string.
@param out__val Location to put to retrieved value.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_int_var(lua_script_t* lua, const char* variable, int* out__val);

/**

*/
boolean lua_script__get_key(lua_script_t* lua, char* out__key, int max_key_size);

/**
Gets the value from the top of the stack as a string.

@param lua The Lua script context.
@param out__val Location to put to retrieved value.
@param max_str_size The size of the output buffer.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_string(lua_script_t* lua, char* out__str, int max_str_size);

/**
Gets the value from the specified variable as a string.

@param lua The Lua script context.
@param variable The name or path of the variable to get. Must be a null-terminated string.
@param out__val Location to put to retrieved value.
@param max_str_size The size of the output buffer.
@returns TRUE if successful, FALSE otherwise.
*/
boolean lua_script__get_string_var(lua_script_t* lua, const char* variable, char* out__str, int max_str_size);

/**
Pops previous value and pops previous key. If a next key is present in the table,
push the next key and then pushes the next value.

Must be preceeded by either start_loop() or next().
*/
boolean lua_script__next(lua_script_t* lua);

/**

*/
uint32_t lua_script__pop(lua_script_t* lua, uint32_t num);

/**

*/
uint32_t lua_script__pop_all(lua_script_t* lua);

/**
Pushes the specified variable path onto the stack. Nested variables are supported
using the period separator.

Examples:
	variableName
	tableName
	tableName.key
	tableName.nestedTable.key

@param lua The lua script context.
@param variable The name of the variable to push. This must be a
null terminated string.
@param variable The length of the variable string.
@return The number of elements pushed.
*/
uint32_t lua_script__push(lua_script_t* lua, const char* variable);

/**
Setup iteration through table. Pushes nil twice. Expects a table
to be on top of stack. Use next() to iterate through values.

@param lua The lua script context.
*/
boolean lua_script__start_loop(lua_script_t* lua);

#endif /* LUA_SCRIPT_H */