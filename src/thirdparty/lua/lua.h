#ifndef THIRDPARTY_LUA_H
#define THIRDPARTY_LUA_H

#if JETZ_CONFIG_PLATFORM_GLFW || JETZ_CONFIG_PLATFORM_TESTS
	#include "thirdparty/lua/windows-5.3.5/lua.h"
	#include "thirdparty/lua/windows-5.3.5/lualib.h"
	#include "thirdparty/lua/windows-5.3.5/lauxlib.h"
#elif JETZ_CONFIG_PLATFORM_PSP
	#include "thirdparty/lua/psp-5.3.4/lua.h"
	#include "thirdparty/lua/psp-5.3.4/lualib.h"
	#include "thirdparty/lua/psp-5.3.4/lauxlib.h"
#else
	#error Unknown platform.
#endif

#endif /* THIRDPARTY_LUA_H */
