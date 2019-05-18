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
	int x; int y; int z; int w;
} vec2i_t;
typedef struct {
	int x; int y; int z; int w;
} vec3i_t;
typedef struct {
	int x; int y; int z; int w;
} vec4i_t;
#endif

/*=========================================================
FUNCTIONS
=========================================================*/

#define FATAL(msg) assert(FALSE)

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#define LOG_ERROR(msg) printf(msg)
#endif

#endif /* PLATFORM_H */