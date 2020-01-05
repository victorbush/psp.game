/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_window.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "log/log.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void create_descriptors(_vlk_window_t* window, _vlk_dev_t* dev);

static void create_pipelines(_vlk_window_t* window, _vlk_t* vlk);

static void create_surface(_vlk_window_t* window, _vlk_t* vlk);

static void create_swapchain(_vlk_window_t* window, _vlk_t* vlk, uint32_t width, uint32_t height);

static void destroy_descriptors(_vlk_window_t* window);

static void destroy_pipelines(_vlk_window_t* window);

static void destroy_surface(_vlk_window_t* window, _vlk_t* vlk);

static void destroy_swapchain(_vlk_window_t* window, _vlk_t* vlk);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void vlk_window__construct(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);

	/* Allocate implementation context */
	window->data = malloc(sizeof(_vlk_window_t));
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	vlk_window->base = window;

	create_surface(vlk_window, vlk);
	create_swapchain(vlk_window, vlk, width, height);
	create_pipelines(vlk_window, vlk);
	create_descriptors(vlk_window, &vlk->dev);
}

void vlk_window__destruct(gpu_window_t* window, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);

	destroy_descriptors(vlk_window);
	destroy_pipelines(vlk_window);
	destroy_swapchain(vlk_window, vlk);
	destroy_surface(vlk_window, vlk);

	/* Free implementation context */
	free(window->data);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void vlk_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, camera_t* camera)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	vlk_frame->frame_idx = frame->frame_idx;

	/* Setup render pass, command buffer, etc. */
	_vlk_swapchain__begin_frame(&vlk_window->swapchain, vlk, vlk_frame);

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk_window->per_view_set, vlk_frame, camera, vlk_window->swapchain.extent);
}

void vlk_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);

	/* End render pass, submit command buffer, preset swapchain */
	_vlk_swapchain__end_frame(&vlk_window->swapchain, vlk_frame);
}

void vlk_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_swapchain__recreate(&vlk_window->swapchain, width, height);

	// TODO : Dynamic state instead of recreating pipelines
	destroy_pipelines(vlk_window);
	create_pipelines(vlk_window, vlk);
}

_vlk_window_t* _vlk_window__from_base(gpu_window_t* window)
{
	return (_vlk_window_t*)window->data;
}

static void create_descriptors(_vlk_window_t* window, _vlk_dev_t* dev)
{
	_vlk_per_view_set__construct(&window->per_view_set, &dev->per_view_layout);
}

static void create_pipelines(_vlk_window_t* window, _vlk_t* vlk)
{
	_vlk_md5_pipeline__construct(&window->md5_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_obj_pipeline__construct(&window->obj_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_plane_pipeline__construct(&window->plane_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
}

static void create_surface(_vlk_window_t* window, _vlk_t* vlk)
{
	VkResult result = vlk->create_surface_func(window->base->platform_window, vlk->instance, &window->surface);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to create Vulkan surface.");
	}

	/* Must check to make sure surface supports presentation */
	boolean supported = FALSE;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(vlk->gpu.handle, vlk->dev.present_family_idx, window->surface, &supported);
	if (result != VK_SUCCESS || !supported)
	{
		log__fatal("Surface not supported for this GPU.");
	}
}

static void create_swapchain(_vlk_window_t* window, _vlk_t* vlk, uint32_t width, uint32_t height)
{
	VkExtent2D extent;
	extent.width = width;
	extent.height = height;

	_vlk_swapchain__init(&window->swapchain, &vlk->dev, window->surface, extent);
}

static void destroy_descriptors(_vlk_window_t* window)
{
	_vlk_per_view_set__destruct(&window->per_view_set);
}

static void destroy_pipelines(_vlk_window_t* window)
{
	_vlk_md5_pipeline__destruct(&window->md5_pipeline);
	_vlk_obj_pipeline__destruct(&window->obj_pipeline);
	_vlk_plane_pipeline__destruct(&window->plane_pipeline);
}

static void destroy_surface(_vlk_window_t* window, _vlk_t* vlk)
{
	vkDestroySurfaceKHR(vlk->instance, window->surface, NULL);
}

static void destroy_swapchain(_vlk_window_t* window, _vlk_t* vlk)
{
	_vlk_swapchain__term(&window->swapchain);
}
