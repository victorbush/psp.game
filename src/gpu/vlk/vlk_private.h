#ifndef VLK_PRIVATE_H
#define VLK_PRIVATE_H

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "platforms/glfw/glfw.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

utl_array_declare_type(_vlk_gpu_t);
utl_array_declare_type(VkPresentModeKHR);
utl_array_declare_type(VkSurfaceFormatKHR);
utl_array_declare_type(VkExtensionProperties);
utl_array_declare_type(VkQueueFamilyProperties);
utl_array_declare_type(VkPhysicalDevice);

/**
Queue family indices.
*/
typedef struct
{
	utl_array_t(uint32_t)	graphics_families;
	utl_array_t(uint32_t)	present_families;

} _vlk_gpu_qfi_type;

/**
Info about Vulkan physical devices.
*/
typedef struct
{
	/* Physical device handle - automatically destroyed by Vulkan instance */
	VkPhysicalDevice                handle;

	/*
	Create/destroy
	*/
	utl_array_t(VkPresentModeKHR)	avail_present_modes;		/* Available present modes */
	utl_array_t(VkSurfaceFormatKHR)	avail_surface_formats;		/* Available surface formats */
	utl_array_t(VkExtensionProperties) 
									ext_properties;				/* Device extension info */
	utl_array_t(VkQueueFamilyProperties)
									queue_family_props;			/* Queue family properties */
	_vlk_gpu_qfi_type				queue_family_indices;		/* Stores indices to certain types of queue families */

	/*
	Other
	*/
	VkPhysicalDeviceProperties		device_properties;
	VkPhysicalDeviceMemoryProperties	
									mem_properties;
	VkPresentModeKHR				optimal_present_mode;		/* Preferred presentation mode based of available modes for this GPU */
	VkSurfaceFormatKHR				optimal_surface_format;		/* The preferred surface we chose based on available formats for this GPU */
	VkPhysicalDeviceFeatures		supported_features;			/* Features supported by this device */

} _vlk_gpu_t;

/**
Vulkan context data.
*/
typedef struct
{
	boolean							enable_validation;

	/*
	Dependencies
	*/
	GLFWwindow*						window;

	/*
	Create/destroy
	*/
	VkDebugReportCallbackEXT		dbg_callback_hndl;
	_vlk_gpu_t						gpu;
	VkInstance						instance;
	VkSurfaceKHR					surface;

} _vlk_type;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk_dbg.c
-------------------------------------*/

/**
Creates the debug callbacks.
*/
void _vlk_dbg__create_dbg_callbacks(_vlk_type* vlk);


/**
Destroys the debug callbacks.
*/
void _vlk_dbg__destroy_dbg_callbacks(_vlk_type* vlk);

/*-------------------------------------
vlk_dbg.c
-------------------------------------*/

/**
Creates a physical device.
*/
void _vlk_gpu__init
	(
	_vlk_gpu_t*						gpu,				/* the GPU to initialize */
	const VkPhysicalDevice			physDeviceHandle,	/* Physical device handle for this GPU  */
	const VkSurfaceKHR				surfaceHandle		/* Used to query for surface capabilties */
	);

/**
Destroys a physical device.
*/
void _vlk_gpu__term(_vlk_gpu_t* gpu);

/*-------------------------------------
vlk_setup.c
-------------------------------------*/

/**
Creates the Vulkan instance.
*/
void _vlk_setup__create_instance(_vlk_type* vlk);

/**
Creates the surface.
*/
void _vlk_setup__create_surface(_vlk_type* vlk);

/**
Destroys the Vulkan instance.
*/
void _vlk_setup__destroy_instance(_vlk_type* vlk);

/**
Destroys the surface.
*/
void _vlk_setup__destroy_surface(_vlk_type* vlk);

#endif /* VLK_PRIVATE_H */