#ifndef VLK_PRIVATE_H
#define VLK_PRIVATE_H

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vma.h"
#include "platforms/glfw/glfw.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct _vlk_gpu_s _vlk_gpu_t;

utl_array_declare_type(_vlk_gpu_t);
utl_array_declare_type(VkExtensionProperties);
utl_array_declare_type(VkPhysicalDevice);
utl_array_declare_type(VkPresentModeKHR);
utl_array_declare_type(VkQueueFamilyProperties);
utl_array_declare_type(VkSurfaceFormatKHR);

/**
Queue family indices.
*/
typedef struct
{
	utl_array_t(uint32_t)	graphics_families;
	utl_array_t(uint32_t)	present_families;

} _vlk_gpu_qfi_t;

/**
Info about Vulkan physical devices.
*/
struct _vlk_gpu_s
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
	_vlk_gpu_qfi_t					queue_family_indices;		/* Stores indices to certain types of queue families */

	/*
	Other
	*/
	VkPhysicalDeviceProperties		device_properties;
	VkPhysicalDeviceMemoryProperties	
									mem_properties;
	VkPresentModeKHR				optimal_present_mode;		/* Preferred presentation mode based of available modes for this GPU */
	VkSurfaceFormatKHR				optimal_surface_format;		/* The preferred surface we chose based on available formats for this GPU */
	VkPhysicalDeviceFeatures		supported_features;			/* Features supported by this device */
};

typedef struct
{
	/*
	* Dependencies
	*/
	_vlk_gpu_t*						gpu;

	/*
	* Create/destroy
	*/
	VmaAllocator                    allocator;
	VkCommandPool                   command_pool;
	VkDevice                        device;
	VkSampler						texture_sampler;
	utl_array_t(uint32_t)           used_queue_families;	/* Unique set of queue family indices used by this device */

	/*
	* Queues and families
	*/
	int                             gfx_family_idx;
	VkQueue                         gfx_queue;
	int                             present_family_idx;
	VkQueue                         present_queue;

} _vlk_dev_t;

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

} _vlk_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk_dbg.c
-------------------------------------*/

/**
Creates the debug callbacks.
*/
void _vlk_dbg__create_dbg_callbacks(_vlk_t* vlk);

/**
Destroys the debug callbacks.
*/
void _vlk_dbg__destroy_dbg_callbacks(_vlk_t* vlk);

/*-------------------------------------
vlk_device.c
-------------------------------------*/

/**
Creates a logical device.
*/
void _vlk_device__init
(
	_vlk_t*							vlk,				/* context */
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	_vlk_gpu_t*						gpu,				/* physical device used by the logical device */
	utl_array_t(string)*			req_dev_ext,		/* required device extensions */
	utl_array_t(string)*			req_inst_layers		/* required instance layers */
);

/*-------------------------------------
vlk_gpu.c
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
Selects a physical device and creates the logcial device.
*/
void _vlk_setup__create_device(_vlk_t* vlk);

/**
Creates the Vulkan instance.
*/
void _vlk_setup__create_instance(_vlk_t* vlk);

/**
Creates the surface.
*/
void _vlk_setup__create_surface(_vlk_t* vlk);

/**
Destroys devices.
*/
void _vlk_setup__destroy_device(_vlk_t* vlk);

/**
Destroys the Vulkan instance.
*/
void _vlk_setup__destroy_instance(_vlk_t* vlk);

/**
Destroys the surface.
*/
void _vlk_setup__destroy_surface(_vlk_t* vlk);

#endif /* VLK_PRIVATE_H */