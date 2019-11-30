#ifndef VLK_PRV_H
#define VLK_PRV_H

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "platforms/glfw/glfw.h"
#include "thirdparty/vma/vma.h"
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
typedef struct _vlk_dev_s _vlk_dev_t;
typedef struct _vlk_mem_alloc_s _vlk_mem_alloc_t;
typedef struct _vlk_mem_pool_s _vlk_mem_pool_t;

utl_array_declare_type(_vlk_gpu_t);
utl_array_declare_type(_vlk_mem_alloc_t);
utl_array_declare_type(_vlk_mem_pool_t);
utl_array_declare_type(VkDeviceSize);
utl_array_declare_type(VkDeviceQueueCreateInfo);
utl_array_declare_type(VkExtensionProperties);
utl_array_declare_type(VkFormat);
utl_array_declare_type(VkPhysicalDevice);
utl_array_declare_type(VkPresentModeKHR);
utl_array_declare_type(VkQueueFamilyProperties);
utl_array_declare_type(VkSurfaceFormatKHR);

typedef struct
{
	VmaAllocation					allocation;
	VkBufferUsageFlags				buffer_usage;	/* how the buffer is used */
	_vlk_dev_t*						dev;			/* logical device */
	VkBuffer						handle;			/* Vulkan buffer handle */
	VmaMemoryUsage					memory_usage;	/* how the underlying memory is used */
	VkDeviceSize					size;			/* the size of the buffer */

} _vlk_buffer_t;

typedef struct
{
	_vlk_buffer_t			buffer;					/* the buffer that backs the array */
	VkDeviceSize			element_size;			/* the size of an element in the buffer */
	VkDeviceSize			element_size_padded;	/* the size of an element plus any padding required for alignment */
	uint32_t				num_elements;			/* the number of elements in the buffer */

} _vlk_buffer_array_t;

/*
Per-view descriptor set data.
*/
typedef struct
{
	float							view[16];
	float							proj[16];
	vec3_t							camera_pos;

} _vlk_per_view_ubo_t;

typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*						dev;

	/*
	Create/destroy
	*/
	VkDescriptorSetLayout			handle;
	VkDescriptorPool				pool_handle;

} _vlk_per_view_layout_t;

/**
Plane pipeline.
*/
typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*						dev;					/* logical device */
	VkRenderPass					render_pass;

	/*
	Create/destroy
	*/
	_vlk_buffer_t					index_buffer;
	VkPipeline						handle;
	VkPipelineLayout				layout;
	_vlk_buffer_t					vertex_buffer;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_plane_pipeline_t;

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
struct _vlk_dev_s
{
	/*
	Dependencies
	*/
	_vlk_gpu_t*						gpu;

	/*
	Create/destroy
	*/
	VmaAllocator					allocator;
	VkCommandPool					command_pool;
	VkDevice						handle;					/* Handle for the logical device */
	VkSampler						texture_sampler;
	utl_array_t(uint32_t)			used_queue_families;	/* Unique set of queue family indices used by this device */

	_vlk_per_view_layout_t			per_view_layout;

	/*
	Queues and families
	*/
	int								gfx_family_idx;
	VkQueue							gfx_queue;
	int								present_family_idx;
	VkQueue							present_queue;
};

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
	VmaAllocation					depth_image_allocations[NUM_FRAMES];
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

	_vlk_plane_pipeline_t			plane_pipeline;

	utl_array_t(string) 			req_dev_ext;		/* required device extensions */
	utl_array_t(string) 			req_inst_ext;		/* required instance extensions */
	utl_array_t(string) 			req_inst_layers;	/* required instance layers */

} _vlk_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk_buffer.c
-------------------------------------*/

/**
Initializes the buffer and allocates any required memory.
*/
void _vlk_buffer__construct
	(
	_vlk_buffer_t*					buffer,
	_vlk_dev_t*						device,
	VkDeviceSize					size,
	VkBufferUsageFlags				buffer_usage,
	VmaMemoryUsage					memory_usage
	);

/**
Destroys the buffer and frees any allocated memory.
*/
void _vlk_buffer__destruct(_vlk_buffer_t* buffer);

/**
Builds a VkDescriptorBufferInfo struct for this buffer.
*/
VkDescriptorBufferInfo _vlk_buffer__get_buffer_info(_vlk_buffer_t* buffer);

/**
Updates the data in the buffer.
*/
void _vlk_buffer__update(_vlk_buffer_t* buffer, void* data, VkDeviceSize offset, VkDeviceSize size);

/*-------------------------------------
vlk_buffer_array.c
-------------------------------------*/

/**
Initializes the buffer and allocates any required memory.
*/
void _vlk_buffer_array__construct
	(
	_vlk_buffer_array_t*			buffer,
	_vlk_dev_t*						device,
	VkDeviceSize					element_size,
	uint32_t						num_elements,
	VkBufferUsageFlags				buffer_usage,
	VmaMemoryUsage					memory_usage
	);

/**
Destroys the buffer and frees any allocated memory.
*/
void _vlk_buffer_array__destruct(_vlk_buffer_array_t* buffer);

/**
Updates data for an element in the buffer.
*/
void _vlk_buffer_array__update(_vlk_buffer_array_t* buffer, void* data, uint32_t index);

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
void _vlk_device__construct
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
void _vlk_device__destruct
	(
	_vlk_dev_t*						dev
	);

/**
Begins recording of a one-time command buffer.
*/
VkCommandBuffer _vlk_device__begin_one_time_cmd_buf(_vlk_dev_t* dev);

/**
Creates a shader module.
*/
VkShaderModule _vlk_device__create_shader(_vlk_dev_t* dev, const char* file);

/**
Destroys a shader module.
*/
void _vlk_device__destroy_shader(_vlk_dev_t* dev, VkShaderModule shader);

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
uint32_t _vlk_gpu__find_memory_type_idx
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
vlk_per_view_layout.c
-------------------------------------*/

/**
Initializes the per-view descriptor set layout.
*/
void _vlk_per_view_layout__construct
(
	_vlk_per_view_layout_t* layout,
	_vlk_dev_t* device
);

/**
Destroys the per-view descriptor set layout.
*/
void _vlk_per_view_layout__destruct(_vlk_per_view_layout_t* layout);

/*-------------------------------------
vlk_plane_pipeline.c
-------------------------------------*/

/**
Initializes a plane pipeline.
*/
void _vlk_plane_pipeline__construct
(
	_vlk_plane_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
);

/**
Destructs a plane pipeline.
*/
void _vlk_plane_pipeline__destruct(_vlk_plane_pipeline_t* pipeline);

/**
Binds the pipeline for use.
*/
void _vlk_plane_pipeline__bind(_vlk_plane_pipeline_t* pipeline, VkCommandBuffer cmd);

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
Creates pipelines.
*/
void _vlk_setup__create_pipelines(_vlk_t* vlk);

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
Destroys pipelines.
*/
void _vlk_setup__destroy_pipelines(_vlk_t* vlk);

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