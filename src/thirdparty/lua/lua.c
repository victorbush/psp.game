#if JETZ_CONFIG_PLATFORM_GLFW || JETZ_CONFIG_PLATFORM_TESTS
	#if _MSC_VER >= 1910
		#pragma comment (lib, "../../src/thirdparty/lua/windows-5.3.5/lib-win64-vc15/lua53.lib")
	#else
		#error Need to add Lua libraries for other Visual Studio versions.
	#endif
#elif JETZ_CONFIG_PLATFORM_PSP
	// Lib included in makefile
#else
	#error Unknown platform.
#endif