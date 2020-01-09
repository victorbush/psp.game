#ifndef GLFW_H
#define GLFW_H

#define SRC_DIR	__FILE__"/../../.."

/*=========================================================
DECLARATIONS
=========================================================*/

#include "platform/platform_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "log/log.h"

/*-----------------------------------------------------
GLFW
-----------------------------------------------------*/

/*
Define GLFW renderer
*/
#define GLFW_INCLUDE_VULKAN

/*
Include correct header file
*/
#if _WIN64
#include "thirdparty/glfw/glfw-3.3.bin.win64/include/GLFW/glfw3.h"
#else
#error Unknown platform.
#endif

/*
Link lib
*/
#if _WIN64

#if _MSC_VER >= 1900
#pragma comment(lib, SRC_DIR"/thirdparty/glfw/glfw-3.3.bin.win64/lib-vc2015/glfw3.lib")
#else
#error Only Visual Studio 2015 and up are supported for 64-bit builds.
#endif

#else
#error Unknown platform.
#endif

/*=========================================================
FUNCTIONS
=========================================================*/

float glfw__get_time(platform_t* platform);

boolean glfw__load_file(const char* filename, boolean binary, long* out__size, void** out__buffer);

void glfw__log_to_stdout(log_t* log, const char* msg);

#endif /* GLFW_H */