#ifndef VLK_PRIVATE_PIPELINES_H
#define VLK_PRIVATE_PIPELINES_H

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/**
Plane pipeline.
*/
typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*						dev;					/* logical device */

} _vlk_plane_pipeline_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk_plane_pipeline.c
-------------------------------------*/

/**
Initializes a plane pipeline.
*/
void _vlk_plane_pipeline__init
	(
	_vlk_plane_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkPipelineLayout				layout,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	);

/**
_vlk_plane_pipeline__term
*/
void _vlk_plane_pipeline__term(_vlk_plane_pipeline_t* pipeline);

#endif /* VLK_PRIVATE_PIPELINES_H */