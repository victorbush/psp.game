#ifndef VLK_MATERIAL_H
#define VLK_MATERIAL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/vlk/vlk_material_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "vlk_prv.h"

/*=========================================================
TYPES
=========================================================*/

/**
Vulkan-specific material data.
*/
struct _vlk_material_s
{
	/*
	Dependencies
	*/
	_vlk_texture_t*				diffuse_texture;

	/*
	Create/destroy
	*/
	_vlk_buffer_t				buffer;		/* The buffer that stores the material data on the GPU */
};

/**
Defines the layout used for the material UBO. Must match layout in the fragment shader.
*/
struct _vlk_material_ubo_s
{
	kk_vec4_t					ambient;
	kk_vec4_t					diffuse;
	kk_vec4_t					specular;
	bool32_t					has_diffuse_texture;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/vlk_material.public.h"

#endif /* VLK_MATERIAL_H */