/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/*
Want to keep these inline when possible. Need to have extern declarations in the source
file to keep compiler happen since this is in a static library.
*/

extern void kk_math_cross(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest);
extern float kk_math_deg(float rad);
extern void kk_math_lookat(kk_vec3_t* eye, kk_vec3_t* center, kk_vec3_t* up, kk_mat4_t* dest);
extern void kk_math_perspective(float fovy, float aspect, float near_val, float far_val, kk_mat4_t* dest);
extern float kk_math_quat_angle(kk_vec4_t* q);
extern void kk_math_quat_axis(kk_vec4_t* q, kk_vec3_t* dest);
extern void kk_math_quat_mat4(kk_vec4_t* q, kk_mat4_t* dest);
extern void kk_math_quat_mul(kk_vec4_t* p, kk_vec4_t* q, kk_vec4_t* dest);
extern void kk_math_quat_normalize(kk_vec4_t* q);
extern void kk_math_quat_rotatev(kk_vec4_t* q, kk_vec3_t* v, kk_vec3_t* dest);
extern float kk_math_rad(float deg);
extern void kk_math_vec3_add(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest);
extern void kk_math_vec3_copy(kk_vec3_t* a, kk_vec3_t* dest);
extern void kk_math_vec3_sub(kk_vec3_t* a, kk_vec3_t* b, kk_vec3_t* dest);
extern void kk_math_vec3_scale(kk_vec3_t* v, float s, kk_vec3_t* dest);
