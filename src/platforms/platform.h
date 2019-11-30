#ifndef PLATFORM_H
#define PLATFORM_H

/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#ifdef JETZ_CONFIG_PLATFORM_PSP
#include <psptypes.h>
#endif

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#include <stdio.h>
#include "thirdparty/cglm/include/cglm/vec3.h"
#include "thirdparty/cglm/include/cglm/vec4.h"
#include "thirdparty/cglm/include/cglm/mat4.h"
#endif

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

#ifdef JETZ_CONFIG_PLATFORM_PSP
typedef ScePspFVector2 vec2_t;
typedef ScePspFVector3 vec3_t;
typedef ScePspFVector4 vec4_t;

typedef ScePspIVector2 vec2i_t;
typedef ScePspIVector3 vec3i_t;
typedef ScePspIVector4 vec4i_t;
#endif

#ifdef JETZ_CONFIG_PLATFORM_GLFW
typedef struct {
	float x; float y;
} vec2_t;
typedef struct {
	float x; float y; float z;
} vec3_t;
typedef struct {
	float x; float y; float z; float w;
} vec4_t;

typedef struct {
	int x; int y;
} vec2i_t;
typedef struct {
	int x; int y; int z;
} vec3i_t;
typedef struct {
	int x; int y; int z; int w;
} vec4i_t;

typedef struct
{
	vec4_t x;
	vec4_t y;
	vec4_t z;
	vec4_t w;
} mat4_t;
#endif

/*=========================================================
FUNCTIONS
=========================================================*/

#define FATAL(msg) assert(FALSE)

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#define LOG_ERROR(msg) printf(msg)
#endif

#define DBG_ASSERT(condition, msg) assert(condition)
#define DBG_ASSERT_ALWAYS(msg) assert(FALSE)

#endif /* PLATFORM_H */