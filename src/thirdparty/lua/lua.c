#if _MSC_VER >= 1910
	#pragma comment (lib, "../../src/thirdparty/lua/lib-win64-vc15/lua53.lib")
#else
	#error Need to add Lua libraries for other Visual Studio versions.
#endif