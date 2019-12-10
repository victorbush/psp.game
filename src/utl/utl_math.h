/*=========================================================
Math utilties and data types 
=========================================================*/

#ifndef UTL_MATH_H
#define UTL_MATH_H

/*=========================================================
INCLUDES
=========================================================*/

#include <stdint.h>

#ifdef JETZ_CONFIG_PLATFORM_PSP
#include <psptypes.h>
#endif

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#include "thirdparty/cglm/include/cglm/vec3.h"
#include "thirdparty/cglm/include/cglm/vec4.h"
#include "thirdparty/cglm/include/cglm/mat4.h"
#endif

/*=========================================================
TYPES
=========================================================*/

/*-------------------------------------
PSP types
-------------------------------------*/
#ifdef JETZ_CONFIG_PLATFORM_PSP
typedef ScePspFVector2 vec2_t;
typedef ScePspFVector3 vec3_t;
typedef ScePspFVector4 vec4_t;

typedef ScePspIVector2 vec2i_t;
typedef ScePspIVector3 vec3i_t;
typedef ScePspIVector4 vec4i_t;

typedef ScePspSVector2 vec2i16_t;
typedef ScePspSVector3 vec3i16_t;
typedef ScePspSVector4 vec4i16_t;

typedef ScePspFMatrix4 mat4_t;
#endif

/*-------------------------------------
GLFW types
-------------------------------------*/
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
	int32_t x; int32_t y;
} vec2i_t;
typedef struct {
	int32_t x; int32_t y; int32_t z;
} vec3i_t;
typedef struct {
	int32_t x; int32_t y; int32_t z; int32_t w;
} vec4i_t;

typedef struct {
	int16_t x; int16_t y;
} vec2i16_t;
typedef struct {
	int16_t x; int16_t y; int16_t z;
} vec3i16_t;
typedef struct {
	int16_t x; int16_t y; int16_t z; int16_t w;
} vec4i16_t;

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

#endif /* UTL_MATH_H */