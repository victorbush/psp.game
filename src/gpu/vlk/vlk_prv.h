#ifndef VLK_PRV_H
#define VLK_PRV_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/components/ecs_transform_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_window_.h"
#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "common.h"
#include "engine/kk_camera.h"
#include "engine/kk_math.h"
#include "gpu/gpu_anim_model.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_texture.h"
#include "gpu/gpu_static_model.h"
#include "gpu/vlk/vlk.h"
#include "platform/glfw/glfw.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/rxi_map/src/map.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define MAX_NUM_MATERIALS	100

/*=========================================================
TYPES
=========================================================*/

typedef uint8_t _vlk_frame_status_t;
enum
{
	_VLK_FRAME_STATUS_VALID,
	_VLK_FRAME_STATUS_SWAPCHAIN_OUT_OF_DATE,
};

typedef struct _vlk_anim_mesh_s _vlk_anim_mesh_t;
typedef struct _vlk_buffer_s _vlk_buffer_t;
typedef struct _vlk_gpu_s _vlk_gpu_t;
typedef struct _vlk_dev_s _vlk_dev_t;
typedef struct _vlk_mem_alloc_s _vlk_mem_alloc_t;
typedef struct _vlk_mem_pool_s _vlk_mem_pool_t;
typedef struct _vlk_static_mesh_s _vlk_static_mesh_t;

utl_array_declare_type(_vlk_anim_mesh_t);
utl_array_declare_type(_vlk_buffer_t);
utl_array_declare_type(_vlk_gpu_t);
utl_array_declare_type(_vlk_mem_alloc_t);
utl_array_declare_type(_vlk_mem_pool_t);
utl_array_declare_type(_vlk_static_mesh_t);
utl_array_declare_type(VkDeviceSize);
utl_array_declare_type(VkDeviceQueueCreateInfo);
utl_array_declare_type(VkExtensionProperties);
utl_array_declare_type(VkFormat);
utl_array_declare_type(VkPhysicalDevice);
utl_array_declare_type(VkPresentModeKHR);
utl_array_declare_type(VkQueueFamilyProperties);
utl_array_declare_type(VkSurfaceFormatKHR);

/*-------------------------------------
Buffers
-------------------------------------*/

struct _vlk_buffer_s
{
	VmaAllocation					allocation;
	VkBufferUsageFlags				buffer_usage;	/* how the buffer is used */
	_vlk_dev_t*						dev;			/* logical device */
	VkBuffer						handle;			/* Vulkan buffer handle */
	VmaMemoryUsage					memory_usage;	/* how the underlying memory is used */
	VkDeviceSize					size;			/* the size of the buffer */

};

typedef struct
{
	_vlk_buffer_t			buffer;					/* the buffer that backs the array */
	VkDeviceSize			element_size;			/* the size of an element in the buffer */
	VkDeviceSize			element_size_padded;	/* the size of an element plus any padding required for alignment */
	uint32_t				num_elements;			/* the number of elements in the buffer */

} _vlk_buffer_array_t;

/*-------------------------------------
Descriptor sets and layouts
-------------------------------------*/

typedef struct
{
	kk_vec3_t					ambient;
	kk_vec3_t					diffuse;
	kk_vec3_t					specular;

} _vlk_material_ubo_t;

/**
Per-view data as laid out in a UBO.
*/
typedef struct
{
	kk_mat4_t					view;
	kk_mat4_t					proj;
	kk_vec3_t					camera_pos;

} _vlk_per_view_ubo_t;

/**
Descriptor set layout for material data.
*/
typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*					dev;

	/*
	Create/destroy
	*/
	VkDescriptorSetLayout		handle;
	VkDescriptorPool			pool_handle;

} _vlk_descriptor_layout_t;

/**
Descriptor set for per-view data.
*/
typedef struct
{
	/*
	Dependencies
	*/
	_vlk_descriptor_layout_t*	layout;

	/*
	Create/destroy
	*/
	_vlk_buffer_t				buffers[NUM_FRAMES];
	VkDescriptorSet				sets[NUM_FRAMES];

} _vlk_descriptor_set_t;

/*-------------------------------------
Pipelines
-------------------------------------*/

/*
Standard vertex data.
*/
// TODO ???? - we've got anim_mesh_vertex, static_mesh_vertex, what do we need?
typedef struct
{
	kk_vec3_t				pos;
	kk_vec3_t				normal;
	kk_vec2_t				tex;

} _vlk_vertex_t;

/**
Base pipeline data.
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
	VkPipeline						handle;
	VkPipelineLayout				layout;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_pipeline_t;

/**
MD5 model pipeline.
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
	VkPipeline						handle;
	VkPipelineLayout				layout;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_md5_pipeline_t;

typedef struct
{
	kk_mat4_t			model_matrix;	/* 16 * 4 = 64 bytes */

} _vlk_md5_push_constant_vertex_t;

typedef struct
{
	_vlk_md5_push_constant_vertex_t	vertex;

} _vlk_md5_push_constant_t;

/**
OBJ model pipeline.
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
	VkPipeline						handle;
	VkPipelineLayout				layout;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_obj_pipeline_t;

typedef struct
{
	kk_mat4_t			model_matrix;	/* 16 * 4 = 64 bytes */

} _vlk_obj_push_constant_vertex_t;

typedef struct
{
	_vlk_obj_push_constant_vertex_t	vertex;

} _vlk_obj_push_constant_t;

typedef struct
{
	kk_vec3_t		pos;
	kk_vec2_t		tex_coord;

} _vlk_plane_vertex_t;

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
	VkPipeline						handle;
	VkPipelineLayout				layout;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_plane_pipeline_t;

/*
NOTE: memory alignment is determined by biggest member in structure.
		Easiest thing is to just sort items in struct by size.

This struct must match layout as defined in shader.

For info about push_constant in shader see: https://github.com/KhronosGroup/GLSL/blob/master/extensions/khr/GL_KHR_vulkan_glsl.txt
For info about std430 layout packing rules: https://stackoverflow.com/questions/29531237/memory-allocation-with-std430-qualifier
*/

typedef struct
{
	kk_mat4_t			model_matrix;	/* 16 * 4 = 64 bytes */

} _vlk_plane_push_constant_vertex_t;

typedef struct
{
	_vlk_plane_push_constant_vertex_t	vertex;

} _vlk_plane_push_constant_t;


typedef struct
{
	kk_mat4_t			model_matrix;	/* 16 * 4 = 64 bytes */

} _vlk_picker_push_constant_vertex_t;

typedef struct
{
	kk_vec4_t			id_color;	/* The id value to use as the color to render this item on the picker buffer. */

} _vlk_picker_push_constant_frag_t;

typedef struct
{
	_vlk_picker_push_constant_vertex_t	vertex;
	_vlk_picker_push_constant_frag_t	frag;

} _vlk_picker_push_constant_t;

/*-------------------------------------
Models
-------------------------------------*/

typedef struct
{
	kk_vec3_t				pos;
	kk_vec3_t				normal;
	kk_vec2_t				tex;

} _vlk_anim_mesh_vertex_t;

struct _vlk_anim_mesh_s
{
	/*
	Dependencies
	*/
	const md5_mesh_t*		md5;

	/*
	Create/destroy
	*/
	_vlk_buffer_t			vertex_buffer;
	_vlk_buffer_t			index_buffer;

	/*
	Other
	*/
	uint32_t				num_indices;
};

typedef struct
{
	/*
	Dependencies
	*/
	const md5_model_t*		md5;

	/*
	Create/destroy
	*/
	utl_array_t(_vlk_anim_mesh_t)		meshes;

} _vlk_anim_model_t;

typedef struct
{
	kk_vec3_t				pos;
	kk_vec3_t				normal;
	kk_vec2_t				tex;

} _vlk_static_mesh_vertex_t;

struct _vlk_static_mesh_s
{
	/*
	Create/destroy
	*/
	_vlk_buffer_t			vertex_buffer;
	_vlk_buffer_t			index_buffer;

	/*
	Other
	*/
	uint32_t				num_indices;
};

typedef struct
{
	/*
	Create/destroy
	*/
	utl_array_t(_vlk_static_mesh_t)		meshes;		/* List of meshes the comprise the model. */

} _vlk_static_model_t;

/*-------------------------------------
Other
-------------------------------------*/

typedef struct
{
	/*
	Create/destroy
	*/
	_vlk_buffer_t				index_buffer;
	_vlk_buffer_t				vertex_buffer;

} _vlk_plane_t;

typedef struct
{
	/*
	Dependencies
	*/
	_vlk_dev_t*					dev;
	VkFormat					format;

	/*
	Create/destroy
	*/
	VkImage						image;
	VmaAllocation				image_allocation;
	VkImageView					image_view;

	/*
	Other
	*/
	VkDescriptorImageInfo		image_info;

} _vlk_texture_t;

typedef struct
{
	void*						data;		/* pointer to texture data */
	uint32_t					height;		/* height of texture in pixels */
	size_t						size;		/* size of texture data in bytes */
	uint32_t					width;		/* width of texture in pixels */

} _vlk_texture_create_info_t;

typedef struct
{
	/*
	Dependencies
	*/
	_vlk_texture_t*				diffuse_texture;

	/*
	Create/destroy
	*/
	_vlk_descriptor_set_t		descriptor_set;

} _vlk_material_t;

/**
imgui pipeline.
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
	VkDescriptorPool				descriptor_pool;
	VkDescriptorSetLayout			descriptor_layout;
	VkDescriptorSet					descriptor_sets[NUM_FRAMES];
	_vlk_texture_t					font_texture;
	VkSampler						font_texture_sampler;
	VkPipeline						handle;
	VkPipelineLayout				layout;

	utl_array_t(_vlk_buffer_t)		index_buffers;
	utl_array_t(uint32_t)			index_buffer_sizes;
	utl_array_t(char)				index_buffer_is_init;
	utl_array_t(_vlk_buffer_t)		vertex_buffers;
	utl_array_t(uint32_t)			vertex_buffer_sizes;
	utl_array_t(char)				vertex_buffer_is_init;

	/*
	Other
	*/
	VkExtent2D						extent;

} _vlk_imgui_pipeline_t;

/**
Queue family indices.
*/
typedef struct
{
	utl_array_t(uint32_t)		graphics_families;
	utl_array_t(uint32_t)		present_families;

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
	gpu_frame_t*					base;

	VkCommandBuffer					cmd_buf;		/* command buffer */
	VkCommandBuffer					picker_cmd_buf;
	uint32_t						frame_idx;
	uint32_t						image_idx;
	double							delta_time;
	//_vlk_frame_status_t				status;

	//uint32_t						width;
	//uint32_t						height;

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

	VkRenderPass					render_pass;
	VkRenderPass					picker_render_pass;

	_vlk_descriptor_layout_t		material_layout;
	_vlk_descriptor_layout_t		per_view_layout;

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

	VkCommandBuffer					picker_cmd_bufs[NUM_FRAMES];	/* implicitly destroy by command pools */
	VkFramebuffer					picker_frame_bufs[NUM_FRAMES];	/* picker render pass framebuffers */
	VkImage							picker_images[NUM_FRAMES];
	VmaAllocation					picker_image_allocations[NUM_FRAMES];
	VkImageView						picker_image_views[NUM_FRAMES];

	VkFence							in_flight_fences[NUM_FRAMES];
	VkSemaphore						image_avail_semaphores[NUM_FRAMES];
	VkSemaphore						render_finished_semaphores[NUM_FRAMES];

	/*
	Swapchain properties
	*/
	VkExtent2D						extent;
	VkPresentModeKHR				present_mode;
	VkSurfaceFormatKHR				surface_format;

	/*
	Other
	*/
	double							last_time;				/* time the previous frame was started */

} _vlk_swapchain_t;

typedef struct
{
	gpu_window_t*					base;

	/*
	Create/destroy
	*/
	_vlk_descriptor_set_t			per_view_set;
	VkSurfaceKHR					surface;
	_vlk_swapchain_t				swapchain;

	_vlk_imgui_pipeline_t			imgui_pipeline;
	_vlk_md5_pipeline_t				md5_pipeline;
	_vlk_obj_pipeline_t				obj_pipeline;
	_vlk_plane_pipeline_t			plane_pipeline;
	_vlk_pipeline_t					picker_pipeline;

} _vlk_window_t;

/**
Vulkan context data.
*/
typedef struct
{
	boolean							enable_validation;

	/*
	Dependencies
	*/
	vlk_create_surface_func			create_surface_func;
	vlk_create_temp_surface_func	create_temp_surface_func;

	/*
	Create/destroy
	*/
	_vlk_dev_t						dev;					/* logical device */
	VkDebugReportCallbackEXT		dbg_callback_hndl;
	_vlk_gpu_t						gpu;					/* physical device */
	VkInstance						instance;

	utl_array_ptr_t(char) 			req_dev_ext;		/* required device extensions */
	utl_array_ptr_t(char) 			req_inst_ext;		/* required instance extensions */
	utl_array_ptr_t(char) 			req_inst_layers;	/* required instance layers */

} _vlk_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk.c
-------------------------------------*/

/**
Gets the Vulkan context implementation memory from the GPU context.

@param The GPU context.
@returns The Vulkan context.
*/
_vlk_t* _vlk__from_base(gpu_t* gpu);

/*-------------------------------------
vlk_anim_mesh.c
-------------------------------------*/

/**
Constructs an animated mesh.
*/
void _vlk_anim_mesh__construct
	(
	_vlk_anim_mesh_t*			mesh,
	_vlk_dev_t*					device,
	const md5_mesh_t*			md5
	);

/**
Destructs an animated mesh.
*/
void _vlk_anim_mesh__destruct(_vlk_anim_mesh_t* mesh);

/**
Computes vertex positions for the mesh using the given skeleton.
*/
void _vlk_anim_mesh__prepare
	(
	_vlk_anim_mesh_t*			mesh,
	const md5_joint_t*			md5_skeleton
	);

/**
Renders an animated mesh. The appropriate pipeline must already be bound.
*/
void _vlk_anim_mesh__render
	(
	_vlk_anim_mesh_t*			mesh,			/* The mesh to render. */
	VkCommandBuffer				cmd,			/* The command buffer. */
	const ecs_transform_t*		transform
	);

/*-------------------------------------
vlk_anim_model.c
-------------------------------------*/

/**
Constructs an animated model.
*/
void _vlk_anim_model__construct
	(
	_vlk_anim_model_t*			model,
	_vlk_dev_t*					device,
	const md5_model_t*			md5
	);

/**
Destructs an animated model.
*/
void _vlk_anim_model__destruct(_vlk_anim_model_t* model);

/**
Renders an animated model. The appropriate pipeline must already be bound.
*/
void _vlk_anim_model__render
	(
	_vlk_anim_model_t*			model,
	VkCommandBuffer				cmd,
	ecs_transform_t*			transform
	);

/*-------------------------------------
vlk_buffer.c
-------------------------------------*/

/**
Initializes the buffer and allocates any required memory.
*/
void _vlk_buffer__construct
	(
	_vlk_buffer_t*				buffer,
	_vlk_dev_t*					device,
	VkDeviceSize				size,
	VkBufferUsageFlags			buffer_usage,
	VmaMemoryUsage				memory_usage
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
	_vlk_buffer_array_t*		buffer,
	_vlk_dev_t*					device,
	VkDeviceSize				element_size,
	uint32_t					num_elements,
	VkBufferUsageFlags			buffer_usage,
	VmaMemoryUsage				memory_usage
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
	utl_array_ptr_t(char)*			req_dev_ext,		/* required device extensions */
	utl_array_ptr_t(char)*			req_inst_layers		/* required instance layers */
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
Copies a buffer to an image.
*/
void _vlk_device__copy_buffer_to_img_now
	(
	_vlk_dev_t*				dev,
	VkBuffer				buffer, 
	VkImage					image, 
	uint32_t				width, 
	uint32_t				height
	);

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
vlk_frame.c
-------------------------------------*/

_vlk_frame_t* _vlk_frame__from_base(gpu_frame_t* frame);

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
vlk_imgui_pipeline.c
-------------------------------------*/

void _vlk_imgui_pipeline__construct
	(
	_vlk_imgui_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	);

void _vlk_imgui_pipeline__destruct(_vlk_imgui_pipeline_t* pipeline);

void _vlk_imgui_pipeline__bind(_vlk_imgui_pipeline_t* pipeline, VkCommandBuffer cmd);

void _vlk_imgui_pipeline__render(_vlk_imgui_pipeline_t* pipeline, _vlk_frame_t* frame, ImDrawData* draw_data);

/*-------------------------------------
vlk_material.c
-------------------------------------*/

/**
Constructs a material.
*/
void _vlk_material__construct
	(
	_vlk_material_t*			material,
	_vlk_material_ubo_t*		ubo,
	_vlk_descriptor_layout_t*	layout,
	_vlk_texture_t*				diffuse_texture
	);

/**
Destructs a material.
*/
void _vlk_material__destruct(_vlk_material_t* material);

/*-------------------------------------
vlk_material_layout.c
-------------------------------------*/

/**
Initializes the material descriptor set layout.
*/
void _vlk_material_layout__construct
	(
	_vlk_descriptor_layout_t*	layout,
	_vlk_dev_t*					device
	);

/**
Destroys the material descriptor set layout.
*/
void _vlk_material_layout__destruct(_vlk_descriptor_layout_t* layout);

/*-------------------------------------
vlk_material_set.c
-------------------------------------*/

/**
Constructs a material descriptor set.
*/
void _vlk_material_set__construct
	(
	_vlk_descriptor_set_t*		set,
	_vlk_descriptor_layout_t*	layout,
	_vlk_material_ubo_t*		ubo,
	_vlk_texture_t*				diffuse_texture
	);

/**
Destructs a material descriptor set.
*/
void _vlk_material_set__destruct(_vlk_descriptor_set_t* set);

/**
Binds the descriptor set for the specified frame.
*/
void _vlk_material_set__bind
	(
	_vlk_descriptor_set_t*			set,
	_vlk_frame_t*					frame,
	VkPipelineLayout				pipelineLayout
	);

/*-------------------------------------
vlk_md5_pipeline.c
-------------------------------------*/

/**
Constructs the MD5 model pipeline.
*/
void _vlk_md5_pipeline__construct
	(
	_vlk_md5_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	);

/**
Destructs the MD5 model pipeline.
*/
void _vlk_md5_pipeline__destruct(_vlk_md5_pipeline_t* pipeline);

/**
Binds the MD5 model pipeline.
*/
void _vlk_md5_pipeline__bind(_vlk_md5_pipeline_t* pipeline, VkCommandBuffer cmd);

/*-------------------------------------
vlk_obj_pipeline.c
-------------------------------------*/

/**
Constructs the OBJ model pipeline.
*/
void _vlk_obj_pipeline__construct
	(
	_vlk_obj_pipeline_t*			pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	);

/**
Destructs the OBJ model pipeline.
*/
void _vlk_obj_pipeline__destruct(_vlk_obj_pipeline_t* pipeline);

/**
Binds the OBJ model pipeline.
*/
void _vlk_obj_pipeline__bind(_vlk_obj_pipeline_t* pipeline, VkCommandBuffer cmd);

/*-------------------------------------
vlk_per_view_layout.c
-------------------------------------*/

/**
Initializes the per-view descriptor set layout.
*/
void _vlk_per_view_layout__construct
	(
	_vlk_descriptor_layout_t*	layout,
	_vlk_dev_t*					device
	);

/**
Destroys the per-view descriptor set layout.
*/
void _vlk_per_view_layout__destruct(_vlk_descriptor_layout_t* layout);

/*-------------------------------------
vlk_per_view_set.c
-------------------------------------*/

/**
Constructs a per-view descriptor set.
*/
void _vlk_per_view_set__construct
	(
	_vlk_descriptor_set_t*		set,
	_vlk_descriptor_layout_t*	layout
	);

/**
Destructs a per-view descriptor set.
*/
void _vlk_per_view_set__destruct(_vlk_descriptor_set_t* set);

/**
Binds the descriptor set for the specified frame.
*/
void _vlk_per_view_set__bind
	(
	_vlk_descriptor_set_t*			set,
	VkCommandBuffer					cmd_buf,
	_vlk_frame_t*					frame,
	VkPipelineLayout				pipelineLayout
	);

/**
Updates data in the per-view UBO for the specified frame.
*/
void _vlk_per_view_set__update
	(
	_vlk_descriptor_set_t*			set,
	_vlk_frame_t*					frame,
	kk_camera_t*					camera,
	VkExtent2D						extent
	);

/*-------------------------------------
vlk_plane.c
-------------------------------------*/

/**
Constructs a plane.
*/
void _vlk_plane__construct
	(
	_vlk_plane_t*					plane,
	_vlk_dev_t*						device
	);

/**
Destructs a plane.
*/
void _vlk_plane__destruct(_vlk_plane_t* plane);

/**
Renders a plane.
*/
void _vlk_plane__render
	(
	_vlk_plane_t*				plane,
	VkCommandBuffer				cmd
	);

/**
Updates the vertices for the plane.
*/
void _vlk_plane__update_verts(_vlk_plane_t* plane, const kk_vec3_t verts[4]);

/*-------------------------------------
vlk_picker_pipeline.c
-------------------------------------*/

void _vlk_picker_pipeline__construct
	(
	_vlk_pipeline_t*				pipeline,
	_vlk_dev_t*						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	);

void _vlk_picker_pipeline__destruct(_vlk_pipeline_t* pipeline);

void _vlk_picker_pipeline__bind(_vlk_pipeline_t* pipeline, VkCommandBuffer cmd);

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
Creates lists of required device extensions, instance layers, etc.
*/
void _vlk_setup__create_requirement_lists(_vlk_t* vlk);

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

/*-------------------------------------
vlk_static_mesh.c
-------------------------------------*/

/**
Constructs a static mesh.
*/
void _vlk_static_mesh__construct
	(
	_vlk_static_mesh_t*			mesh,
	_vlk_dev_t*					device,
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	);

/**
Destructs a static mesh.
*/
void _vlk_static_mesh__destruct(_vlk_static_mesh_t* mesh);

/**
Renders a static mesh. The appropriate pipeline must already be bound.
*/
void _vlk_static_mesh__render
	(
	_vlk_static_mesh_t*			mesh,			/* The mesh to render. */
	VkCommandBuffer				cmd				/* The command buffer. */
	);

/*-------------------------------------
vlk_static_model.c
-------------------------------------*/

/**
Constructs a static model.
*/
void _vlk_static_model__construct
	(
	_vlk_static_model_t*		model,
	_vlk_dev_t*					device,
	const tinyobj_t*			obj
	);

/**
Destructs a static model.
*/
void _vlk_static_model__destruct(_vlk_static_model_t* model);

/**
Renders a static model. The appropriate pipeline must already be bound.
*/
void _vlk_static_model__render
	(
	_vlk_static_model_t*		model,
	VkCommandBuffer				cmd
	);

/*-------------------------------------
vlk_swapchain.c
-------------------------------------*/

/**
Initializes a swapchain.
*/
void _vlk_swapchain__init
	(
	_vlk_swapchain_t*			swap,			/* swapchain to init */
	_vlk_dev_t*					dev,			/* Logical device the surface is tied to */
	VkSurfaceKHR				surface,		/* Surface to use */
	VkExtent2D					extent			/* Desired swapchain extent */
	);

/**
Destroys a swapchain.
*/
void _vlk_swapchain__term(_vlk_swapchain_t* swap);

/**
Begins the next frame.
*/
void _vlk_swapchain__begin_frame(_vlk_swapchain_t* swap, _vlk_t* vlk, _vlk_frame_t* frame);

/**
Ends the specified frame.
*/
void _vlk_swapchain__end_frame(_vlk_swapchain_t* swap, _vlk_frame_t* frame);

/**
Gets the command buffer for the specified frame.
*/
VkCommandBuffer _vlk_swapchain__get_cmd_buf(_vlk_swapchain_t* swap, _vlk_frame_t *frame);

/**
Gets the swapchain extent.
*/
VkExtent2D _vlk_swapchain__get_extent(_vlk_swapchain_t* swap);

/**
Flags swap chain to resize itself.
*/
void _vlk_swapchain__recreate(_vlk_swapchain_t* swap, int width, int height);

/*-------------------------------------
vlk_texture.c
-------------------------------------*/

/**
Constructs a texture.
*/
void _vlk_texture__construct
	(
	_vlk_texture_t*				tex,
	_vlk_dev_t*					device,
	const _vlk_texture_create_info_t* create_info
	);

/**
Destructs a texture.
*/
void _vlk_texture__destruct(_vlk_texture_t* tex);

/*-------------------------------------
vlk_window.c
-------------------------------------*/

_vlk_window_t* _vlk_window__from_base(gpu_window_t* window);

#endif /* VLK_PRV_H */