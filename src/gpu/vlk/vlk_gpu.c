/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>
#include <vulkan/vulkan.h>

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/**
Chooses the best swap chain presentation mode from list of available modes.
*/
static VkPresentModeKHR choose_present_mode
	(
	utl_array_t(VkPresentModeKHR)*	avail_modes		/* modes to choose from         */
	);

/**
choose_surface_format
*/
static VkSurfaceFormatKHR choose_surface_format
	(
	utl_array_t(VkSurfaceFormatKHR)*	avail_formats		/* formats to choose from       */
	);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_gpu__create
*/
void _vlk_gpu__init
	(
	_vlk_gpu_t*						gpu,				/* the GPU to initialize */
	const VkPhysicalDevice			physical_device,	/* Physical device handle for this GPU  */
	const VkSurfaceKHR				surface				/* Used to query for surface capabilties */
	)
{
	clear_struct(gpu);
	
	/*
	Store device handle for this GPU. Don't store a surface handle,
	the surface here is a temporary one.
	*/
	gpu->handle = physical_device;

	/*-----------------------------------------------------

	Device extensions

	-----------------------------------------------------*/

	/* get number of extensions */
	uint32_t num_ext = 0;
	vkEnumerateDeviceExtensionProperties(gpu->handle, NULL, &num_ext, NULL);

	/* get list of extensions */
	utl_array_resize(&gpu->ext_properties, num_ext);
	vkEnumerateDeviceExtensionProperties(gpu->handle, NULL, &num_ext, gpu->ext_properties.data);

	/*-----------------------------------------------------

	Device features

	-----------------------------------------------------*/

	vkGetPhysicalDeviceFeatures(gpu->handle, &gpu->supported_features);

	/*-----------------------------------------------------

	Device properties

	-----------------------------------------------------*/

	vkGetPhysicalDeviceProperties(gpu->handle, &gpu->device_properties);

	/*-----------------------------------------------------

	Surface related attributes

	-----------------------------------------------------*/

	/*
	Surface formats
	*/
	uint32_t num_format;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->handle, surface, &num_format, NULL);

	if (num_format != 0)
	{
		utl_array_resize(&gpu->avail_surface_formats, num_format);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->handle, surface, &num_format, gpu->avail_surface_formats.data);
	}

	/*
	Presentation modes
	*/
	uint32_t num_present_mode;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->handle, surface, &num_present_mode, NULL);

	if (num_present_mode != 0)
	{
		utl_array_resize(&gpu->avail_present_modes, num_present_mode);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->handle, surface, &num_present_mode, gpu->avail_present_modes.data);
	}

	/*
	* Based on device capabilities, choose the ideal formats to use
	*/
	gpu->optimal_surface_format = choose_surface_format(&gpu->avail_surface_formats);
	gpu->optimal_present_mode = choose_present_mode(&gpu->avail_present_modes);

	/*-----------------------------------------------------

	Queue families

	-----------------------------------------------------*/

	/* get number of queue families */
	uint32_t num_queue_fam = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu->handle, &num_queue_fam, NULL);

	/* get list of queue families */
	utl_array_resize(&gpu->queue_family_props, num_queue_fam);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu->handle, &num_queue_fam, gpu->queue_family_props.data);

	/* determine what things each family supports */
	for (uint32_t i = 0; i < gpu->queue_family_props.count; ++i)
	{
		VkQueueFamilyProperties family = gpu->queue_family_props.data[i];

		/* check # queue */
		if (family.queueCount == 0)
		{
			/* no queues (??) */
			continue;
		}

		/* check for presentation support */
		VkBool32 presentationSupport = FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu->handle, i, surface, &presentationSupport);

		if (presentationSupport)
		{
			utl_array_push(&gpu->queue_family_indices.present_families, i);
		}

		/* check for graphics support */
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			utl_array_push(&gpu->queue_family_indices.graphics_families, i);
		}
	}

	/*-----------------------------------------------------

	Memory properties

	-----------------------------------------------------*/

	vkGetPhysicalDeviceMemoryProperties(gpu->handle, &gpu->mem_properties);
}

/**
_vlk_gpu__destroy
*/
void _vlk_gpu__term(_vlk_gpu_t* gpu)
{
	utl_array_destroy(&gpu->avail_present_modes);
	utl_array_destroy(&gpu->avail_surface_formats);
	utl_array_destroy(&gpu->ext_properties);
	utl_array_destroy(&gpu->queue_family_props);

	utl_array_destroy(&gpu->queue_family_indices.graphics_families);
	utl_array_destroy(&gpu->queue_family_indices.present_families);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_gpu__find_memory_type_idx
*/
uint32_t _vlk_gpu__find_memory_type_idx
	(
	_vlk_gpu_t*						gpu,
	uint32_t						type_filter, 
	VkMemoryPropertyFlags			properties
	)
{
	for (uint32_t i = 0; i < gpu->mem_properties.memoryTypeCount; i++)
	{
		if ((type_filter & (1 << i)) && (gpu->mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	log__fatal("Failed to find suitable memory type.");
	return 0;
}

/**
_vlk_gpu__get_depth_format
*/
VkFormat _vlk_gpu__get_depth_format(_vlk_gpu_t* gpu)
{
	VkFormat format;
	utl_array_create(VkFormat, candidates);
	utl_array_push(&candidates, VK_FORMAT_D32_SFLOAT);
	utl_array_push(&candidates, VK_FORMAT_D32_SFLOAT_S8_UINT);
	utl_array_push(&candidates, VK_FORMAT_D24_UNORM_S8_UINT);

	format = _vlk_gpu__find_supported_format(
		gpu,
		&candidates,
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	utl_array_destroy(&candidates);
	return format;
}

/**
_vlk_gpu__find_supported_format
*/
VkFormat _vlk_gpu__find_supported_format
	(
	_vlk_gpu_t*						gpu,				/* the GPU to initialize */
	utl_array_t(VkFormat)*			candidates,
	VkImageTiling					tiling,
	VkFormatFeatureFlags			features
	)
{
	uint32_t i;

	for (i = 0; i < candidates->count; ++i)
	{
		VkFormat format = candidates->data[i];
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(gpu->handle, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR
			&& (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL
			&& (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	log__fatal("Failed to find supported format.");
	return VK_FORMAT_R32G32B32A32_UINT;
}

/**
_vlk_gpu__query_surface_capabilties
*/
VkResult _vlk_gpu__query_surface_capabilties
    (
	_vlk_gpu_t*						gpu,
    VkSurfaceKHR					surface,			/* The surface to get capabilties for */
    VkSurfaceCapabilitiesKHR*		capabilties			/* Output - the surface capabilties */
    )
{
    return vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->handle, surface, capabilties);
}

/**
choose_present_mode
*/
static VkPresentModeKHR choose_present_mode
	(
	utl_array_t(VkPresentModeKHR)*	avail_modes		/* modes to choose from         */
	)
{
	uint32_t i;

	/* desired mode is mailbox mode (allows triple buffering) */
	const VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;

	/* see if desired mode is available */
	for (i = 0; i < avail_modes->count; ++i)
	{
		VkPresentModeKHR mode = avail_modes->data[i];

		if (mode == desiredMode)
		{
			return desiredMode;
		}
	}

	/* default to FIFO since always available */
	return VK_PRESENT_MODE_FIFO_KHR;
}

/**
choose_surface_format
*/
static VkSurfaceFormatKHR choose_surface_format
	(
	utl_array_t(VkSurfaceFormatKHR)*	avail_formats		/* formats to choose from       */
	)
{
	uint32_t i;
	VkSurfaceFormatKHR result;

	/* if undefined format, then any format can be chosen */
	if (avail_formats->count == 1
		&& avail_formats->data[0].format == VK_FORMAT_UNDEFINED)
	{
		result.format = VK_FORMAT_B8G8R8A8_UNORM;
		result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		return (result);
	}

	/* if formats are restricted, look for the preferred format */
	for (i = 0; i < avail_formats->count; ++i)
	{
		VkSurfaceFormatKHR format = avail_formats->data[i];

		if (format.format == VK_FORMAT_B8G8R8A8_UNORM
			&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	/* if desired format not found, just use the first available format */
	return avail_formats->data[0];
}