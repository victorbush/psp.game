/*=========================================================
Math utilties and data types 
=========================================================*/

#ifndef KK_MATH_H
#define KK_MATH_H

/*=========================================================
INCLUDES
=========================================================*/

#include <math.h>
#include <stdint.h>

#ifdef JETZ_CONFIG_PLATFORM_PSP
#include <psptypes.h>
#endif

#include "common.h"
#include "thirdparty/cglm/include/cglm/cglm.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define KK_PIf GLM_PIf

/*=========================================================
TYPES
=========================================================*/

/*-------------------------------------
PSP types
-------------------------------------*/
#ifdef JETZ_CONFIG_PLATFORM_PSP
typedef ScePspFVector2 kk_vec2_t;
typedef ScePspFVector3 kk_vec3_t;
typedef ScePspFVector4 kk_vec4_t;

typedef ScePspIVector2 kk_vec2i_t;
typedef ScePspIVector3 kk_vec3i_t;
typedef ScePspIVector4 kk_vec4i_t;

typedef ScePspSVector2 kk_vec2i16_t;
typedef ScePspSVector3 kk_vec3i16_t;
typedef ScePspSVector4 kk_vec4i16_t;

typedef ScePspFMatrix4 kk_mat4_t;

#else
/*-------------------------------------
Default types
-------------------------------------*/
typedef struct {
	float x; float y;
} kk_vec2_t;
typedef struct {
	float x; float y; float z;
} kk_vec3_t;
typedef struct {
	float x; float y; float z; float w;
} kk_vec4_t;

typedef struct {
	int32_t x; int32_t y;
} kk_vec2i_t;
typedef struct {
	int32_t x; int32_t y; int32_t z;
} kk_vec3i_t;
typedef struct {
	int32_t x; int32_t y; int32_t z; int32_t w;
} kk_vec4i_t;

typedef struct {
	int16_t x; int16_t y;
} kk_vec2i16_t;
typedef struct {
	int16_t x; int16_t y; int16_t z;
} kk_vec3i16_t;
typedef struct {
	int16_t x; int16_t y; int16_t z; int16_t w;
} kk_vec4i16_t;

typedef struct
{
	kk_vec4_t x;
	kk_vec4_t y;
	kk_vec4_t z;
	kk_vec4_t w;
} kk_mat4_t;
#endif

/*=========================================================
FUNCTIONS
=========================================================*/

/*
GLM wrappers
*/

KK_INLINE
void kk_math_cross(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest)
{
	glm_vec3_cross((float*)a, (float*)b, (float*)dest);
}

KK_INLINE
float kk_math_deg(float rad)
{
	return glm_deg(rad);
}

KK_INLINE
void kk_math_lookat(kk_vec3_t* eye, kk_vec3_t* center, kk_vec3_t* up, kk_mat4_t* dest)
{
	glm_lookat((float*)eye, (float*)center, (float*)up, (vec4*)dest);
}

KK_INLINE
void kk_math_perspective(float fovy, float aspect, float near_val, float far_val, kk_mat4_t* dest)
{
	glm_perspective(fovy, aspect, near_val, far_val, (vec4*)dest);
}

KK_INLINE
float kk_math_quat_angle(kk_vec4_t* q)
{
	return glm_quat_angle((float*)q);
}

KK_INLINE
void kk_math_quat_axis(kk_vec4_t* q, kk_vec3_t* dest)
{
	glm_quat_axis((float*)q, (float*)dest);
}

KK_INLINE
void kk_math_quat_mat4(kk_vec4_t* q, kk_mat4_t* dest)
{
	glm_quat_mat4((float*)q, (float*)dest);
}

KK_INLINE
void kk_math_quat_mul(kk_vec4_t* p, kk_vec4_t* q, kk_vec4_t* dest)
{
	glm_quat_mul((float*)p, (float*)q, (float*)dest);
}

KK_INLINE
void kk_math_quat_normalize(kk_vec4_t* q)
{
	glm_quat_normalize((float*)q);
}

KK_INLINE
void kk_math_quat_rotatev(kk_vec4_t* q, kk_vec3_t* v, kk_vec3_t* dest)
{
	glm_quat_rotatev((float*)q, (float*)v, (float*)dest);
}

KK_INLINE
float kk_math_rad(float deg)
{
	return glm_rad(deg);
}

KK_INLINE
void kk_math_vec3_add(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest)
{
	glm_vec3_add((float*)a, (float*)b, (float*)dest);
}

KK_INLINE
void kk_math_vec3_copy(kk_vec3_t* a, kk_vec3_t* dest)
{
	glm_vec3_copy((float*)a, (float*)dest);
}

KK_INLINE
void kk_math_vec3_sub(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest)
{
	glm_vec3_sub((float*)a, (float*)b, (float*)dest);
}

KK_INLINE
void kk_math_vec3_scale(kk_vec3_t* v, float s, kk_vec3_t* dest)
{
	glm_vec3_scale((float*)v, s, (float*)dest);
}

#endif /* KK_MATH_H */