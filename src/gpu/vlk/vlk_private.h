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
CONSTANTS
=========================================================*/

#define NUM_FRAMES 2

/*=========================================================
TYPES
=========================================================*/

typedef uint8_t _vlk_frame_status_t;
enum
{
	_VLK_FRAME_STATUS_VALID,
	_VLK_FRAME_STATUS_SWAPCHAIN_OUT_OF_DATE,
};

typedef struct _vlk_gpu_s _vlk_gpu_t;

utl_array_declare_type(_vlk_gpu_t);
utl_array_declare_type(VkDeviceQueueCreateInfo);
utl_array_declare_type(VkExtensionProperties);
utl_array_declare_type(VkFormat);
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

/**
Frame information
*/
typedef struct
{
	uint32_t						frame_idx;
	uint32_t						image_idx;
	double							delta_time;
	_vlk_frame_status_t				status;

	uint32_t						width;
	uint32_t						height;

} _vlk_frame_t;

/**
Vulkan logical device
*/
typedef struct
{
	/*
	* Dependencies
	*/
	_vlk_gpu_t*						gpu;

	/*
	* Create/destroy
	*/
	VkCommandPool                   command_pool;
	VkDevice                        handle;					/* Handle for the logical device */
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
Vulkan swapchain
*/
typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*						dev;					/* logical device */
	_vlk_gpu_t*						gpu;					/* phsyical device */
	VkSurfaceKHR					surface;				/* surface handle */
	GLFWwindow*						window;					/* window the surface will present to */

	/*
	Create/destroy
	*/
	VkCommandBuffer					cmd_bufs[NUM_FRAMES];	/* implicitly destroy by command pools */
	VkImage							depth_images[NUM_FRAMES];
	VkImageView						depth_image_views[NUM_FRAMES];
	VkDeviceMemory					depth_image_memory[NUM_FRAMES];	/* GPU memory for depth images */
	VkFramebuffer					frame_bufs[NUM_FRAMES];	/* swapchain framebuffers */
	VkSwapchainKHR					handle;					/* swapchain handle */
	VkImage							images[NUM_FRAMES];		/* swapchain images */
	VkImageView						image_views[NUM_FRAMES];/* swapchain image views */

	VkFence							in_flight_fences[NUM_FRAMES];
	VkSemaphore						image_avail_semaphores[NUM_FRAMES];
	VkSemaphore						render_finished_semaphores[NUM_FRAMES];

	/*
	Swapchain properties
	*/
	VkExtent2D						extent;
	VkPresentModeKHR				present_mode;
	VkRenderPass					render_pass;
	VkSurfaceFormatKHR				surface_format;

	/*
	Other
	*/
	uint8_t							frame_idx;				/* the current frame index */
	double							last_time;				/* time the previous frame was started */

	_vlk_frame_t					frame;					/* the current frame */

} _vlk_swapchain_t;

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
	_vlk_dev_t						dev;					/* logical device */
	VkDebugReportCallbackEXT		dbg_callback_hndl;
	_vlk_gpu_t						gpu;					/* physical device */
	VkInstance						instance;
	VkSurfaceKHR					surface;
	_vlk_swapchain_t				swapchain;

	utl_array_t(string) 			req_dev_ext;		/* required device extensions */
	utl_array_t(string) 			req_inst_ext;		/* required instance extensions */
	utl_array_t(string) 			req_inst_layers;	/* required instance layers */

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

/**
Destroys a logical device.
*/
void _vlk_device__term
	(
	_vlk_dev_t*						dev
	);

/**
Begins recording of a one-time command buffer.
*/
VkCommandBuffer _vlk_device__begin_one_time_cmd_buf(_vlk_dev_t* dev);

/**
Ends recording of one-time command buffer and submits it to the queue.
*/
void _vlk_device__end_one_time_cmd_buf(_vlk_dev_t* dev, VkCommandBuffer cmd_buf);

/**
_vlk_device__transition_image_layout
*/
void _vlk_device__transition_image_layout
	(
	_vlk_dev_t*						dev, 
	VkImage							image, 
	VkFormat						format, 
	VkImageLayout					old_layout,
	VkImageLayout					new_layout
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

/**
Finds index of memory type with the desired properties.
*/
uint32_t _vlk_gpu__find_memory_type
	(
	_vlk_gpu_t*						gpu,
	uint32_t						type_filter, 
	VkMemoryPropertyFlags			properties
	);

/**
Gets the depth buffer format.
*/
VkFormat _vlk_gpu__get_depth_format(_vlk_gpu_t* gpu);

/**
Finds a supported surface format in a list of candidates for this device.
*/
VkFormat _vlk_gpu__find_supported_format
	(
	_vlk_gpu_t*						gpu,				/* the GPU to initialize */
	utl_array_t(VkFormat)*			candidates,
	VkImageTiling					tiling,
	VkFormatFeatureFlags			features
	);

/**
Query device for current surface capabilities. Some of the capabilties don't
change, but the surface extent does. This is just a wrapper around the 
Vulkan API call.
*/
VkResult _vlk_gpu__query_surface_capabilties
    (
	_vlk_gpu_t*						gpu,
    VkSurfaceKHR					surface,			/* The surface to get capabilties for */
    VkSurfaceCapabilitiesKHR*		capabilties			/* Output - the surface capabilties */
    );

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
Creates lists of required device extensions, instance layers, etc.
*/
void _vlk_setup__create_requirement_lists(_vlk_t* vlk);

/**
Creates the surface.
*/
void _vlk_setup__create_surface(_vlk_t* vlk);

/**
Creates the swapchain.
*/
void _vlk_setup__create_swapchain(_vlk_t* vlk);

/**
Destroys devices.
*/
void _vlk_setup__destroy_device(_vlk_t* vlk);

/**
Destroys the Vulkan instance.
*/
void _vlk_setup__destroy_instance(_vlk_t* vlk);

/**
Destroys lists of required device extensions, instance layers, etc.
*/
void _vlk_setup__destroy_requirement_lists(_vlk_t* vlk);

/**
Destroys the surface.
*/
void _vlk_setup__destroy_surface(_vlk_t* vlk);

/**
Destroys the swapchain.
*/
void _vlk_setup__destroy_swapchain(_vlk_t* vlk);

/*-------------------------------------
vlk_swapchain.c
-------------------------------------*/

/**
Initializes a swapchain.
*/
void _vlk_swapchain__init
	(
	_vlk_swapchain_t*				swap,			/* swapchain to init */
	_vlk_dev_t*						dev,            /* Logical device the surface is tied to    */
	VkSurfaceKHR                    surface,
	GLFWwindow                      *window         /* Window the surface will present to       */
	);

/**
Destroys a swapchain.
*/
void _vlk_swapchain__term(_vlk_swapchain_t* swap);

/**
Begins the next frame.
*/
void _vlk_swapchain__begin_frame(_vlk_swapchain_t* swap);

/**
Ends the specified frame.
*/
void _vlk_swapchain__end_frame(_vlk_swapchain_t* swap);

/**
Gets the command buffer for the specified frame.
*/
VkCommandBuffer _vlk_swapchain__get_cmd_buf(_vlk_swapchain_t* swap, _vlk_frame_t *frame);

/**
Gets the swapchain extent.
*/
VkExtent2D _vlk_swapchain__get_extent(_vlk_swapchain_t* swap);

/**
Gets the render pass handle.
*/
VkRenderPass _vlk_swapchain__get_render_pass(_vlk_swapchain_t* swap);

/**
Flags swap chain to resize itself.
*/
void _vlk_swapchain__recreate(_vlk_swapchain_t* swap, int width, int height);

#endif /* VLK_PRIVATE_H */