#ifndef VLK_PRV_PLANE_H
#define VLK_PRV_PLANE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "platforms/platform.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*
NOTE: memory alignment is determined by biggest member in structure.
		Easiest thing is to just sort items in struct by size.

This struct must match layout as defined in shader.

For info about push_constant in shader see: https://github.com/KhronosGroup/GLSL/blob/master/extensions/khr/GL_KHR_vulkan_glsl.txt
For info about std430 layout packing rules: https://stackoverflow.com/questions/29531237/memory-allocation-with-std430-qualifier
*/

typedef struct
{
	float			ModelMatrix[16];	/* 16 * 4 = 64 bytes */
	vec2_t			Anchor;
	float			Height;
	float			Width;
} vlk_plane_push_constant_vertex;

typedef struct
{
	vec3_t			Color;
} vlk_plane_push_constant_fragment;

typedef struct
{
	vlk_plane_push_constant_vertex		Vertex;
	vlk_plane_push_constant_fragment	Fragment;
} vlk_plane_push_constant;

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* VLK_PRV_PLANE_H */