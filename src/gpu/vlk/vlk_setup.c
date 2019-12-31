/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "log/log.h"
#include "platform/glfw/glfw.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Checks if a specified list of device extensions are available for the
specified physical device.
*/
static boolean are_device_extensions_available
	(
	utl_array_ptr_t(char)*		extensions,		/* list of extension names to check */
	_vlk_gpu_t*					gpu				/* GPU to check                     */
	)
{
	uint32_t i, j;
	boolean desired_ext_avail = TRUE;

	/* check if specified extensions are available */
	for (i = 0; i < extensions->count; ++i)
	{
		char* ext_name = extensions->data[i];
		boolean found = FALSE;

		/* look for this layer in the available layers */
		for (j = 0; j < gpu->ext_properties.count; ++j)
		{
			VkExtensionProperties ext = gpu->ext_properties.data[j];

			if (!strncmp(ext_name, ext.extensionName, sizeof(ext.extensionName)))
			{
				found = TRUE;
				break;
			}
		}

		if (!found)
		{
			desired_ext_avail = FALSE;
			break;
		}
	}

	return (desired_ext_avail);
}

/**
Checks if a specified list of instance extensions are available.
*/
static boolean are_instance_extensions_available
	(
	utl_array_ptr_t(char)*		extensions
	)
{
	boolean desired_extensions_available = TRUE;
	uint32_t i, j;

	/* get number of instance extensions */
	uint32_t available_extensions_cnt = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &available_extensions_cnt, NULL);

	/* get list of instance extensions */
	VkExtensionProperties* available_extensions = malloc(available_extensions_cnt * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &available_extensions_cnt, available_extensions);

	/* check if specified extensions are available */
	for (i = 0; i < extensions->count; ++i)
	{
		char* name = extensions->data[i];
		boolean found = FALSE;

		/* look for this layer in the available layers */
		for (j = 0; j < available_extensions_cnt; ++j)
		{
			VkExtensionProperties* ext = &available_extensions[j];
			if (!strncmp(name, ext->extensionName, sizeof(ext->extensionName)))
			{
				found = TRUE;
				break;
			}
		}

		if (!found)
		{
			desired_extensions_available = FALSE;
			break;
		}
	}

	return (desired_extensions_available);
}


/**
Checks if a specified list of instance layers are available.
*/
static boolean are_instance_layers_available
	(
	utl_array_ptr_t(char)*	layers
	)
{
	boolean desired_layers_available = TRUE;
	uint32_t i;

	/* get number of available instance layers */
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, NULL);

	/* get list of available instance layers*/
	VkLayerProperties* available_layers;
	vkEnumerateInstanceLayerProperties(&count, NULL);
	available_layers = malloc(sizeof(VkLayerProperties) * count);
	vkEnumerateInstanceLayerProperties(&count, available_layers);

	/* check if specified layers are available */
	for (i = 0; i < layers->count; ++i)
	{
		const char* layer_name;
		boolean layer_found = FALSE;
		uint32_t j;

		layer_name = layers->data[i];

		/* look for this layer in the available layers */
		for (j = 0; j < count; ++j)
		{
			VkLayerProperties layer = available_layers[j];

			if (!strncmp(layer_name, layer.layerName, sizeof(layer.layerName)))
			{
				layer_found = TRUE;
				break;
			}
		}

		if (!layer_found)
		{
			desired_layers_available = FALSE;
			break;
		}
	}

	free(available_layers);

	return (desired_layers_available);
}

/**
create_required_device_extensions_list
*/
static void create_required_device_extensions_list(_vlk_t* vlk)
{
	utl_array_init(&vlk->req_dev_ext);

	/* swap chain support is required */
	utl_array_push(&vlk->req_dev_ext, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/* device extensions aren't validated here since a phyiscal device is required */
}

/**
create_required_instance_extensions_list
*/
static void create_required_instance_extensions_list(_vlk_t* vlk)
{
	uint32_t i;

	/* init array */
	utl_array_init(&vlk->req_inst_ext);

	/* get extensions required by GLFW */
	uint32_t num_glfw_extensions = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);

	/* add glfw extensions */
	for (i = 0; i < num_glfw_extensions; ++i)
	{
		utl_array_push(&vlk->req_inst_ext, (char**)glfw_extensions[i]);
	}

	/* determine other extensions needed */
	if (vlk->enable_validation)
	{
		/* debug report extension is required to use validation layers */
		utl_array_push(&vlk->req_inst_ext, VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	/* throw error if the desired extensions are not available */
	if (!are_instance_extensions_available(&vlk->req_inst_ext))
	{
		assert(FALSE);
	}
}

/**
create_required_instance_layers_list
*/
static void create_required_instance_layers_list(_vlk_t* vlk)
{
	utl_array_init(&vlk->req_inst_layers);

	/* add validation layers */
	if (vlk->enable_validation)
	{
		utl_array_push(&vlk->req_inst_layers, "VK_LAYER_LUNARG_standard_validation");
	}

	if (!are_instance_layers_available(&vlk->req_inst_layers))
	{
		assert(FALSE);
	}
}

/**
Selects a physical GPU.
*/
static void select_physical_device(_vlk_t* vlk)
{
	uint32_t i;
	utl_array_create(VkPhysicalDevice, devices);
	utl_array_create(_vlk_gpu_t, gpus);
	utl_array_ptr_create(char, device_ext);		/* required device extensions */

	/* check assumptions */
	if (vlk->instance == VK_NULL_HANDLE)
	{
		log__fatal("Vulkan instance must be created before selecting a phyiscal device.");
	}

	if (vlk->surface == VK_NULL_HANDLE)
	{
		log__fatal("Vulkan surface must be created before selecting a phyiscal device.");
	}

	/*
	Device extensions
	*/

	/* swap chain support is required */
	utl_array_push(&device_ext, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/*
	Enumerate physical devices
	*/

	/* get number of GPUs */
	uint32_t num_gpus = 0;
	vkEnumeratePhysicalDevices(vlk->instance, &num_gpus, NULL);
	if (num_gpus == 0)
	{
		log__fatal("No GPU found with Vulkan support.");
	}

	/* get the list of devices */
	utl_array_resize(&devices, num_gpus);
	vkEnumeratePhysicalDevices(vlk->instance, &num_gpus, devices.data);

	/* allocate mem for GPU info */
	utl_array_resize(&gpus, num_gpus);

	/* get info for each physical device */
	for (i = 0; i < num_gpus; i++)
	{
		_vlk_gpu__init(&gpus.data[i], devices.data[i], vlk->surface);
	}

	/*
	Select a physical device
	*/
	
	/* inspect the physical devices and select one */
	for (i = 0; i < num_gpus; ++i)
	{
		_vlk_gpu_t* gpu = &gpus.data[i];
		
		/*
		* Check required extensions
		*/
		if (!are_device_extensions_available(&device_ext, gpu))
		{
			continue;
		}

		/*
		* Check surface formats
		*/
		if (gpu->avail_surface_formats.count == 0)
		{
			/* no surface formats */
			continue;
		}

		/*
		* Check presentation modes
		*/
		if (gpu->avail_present_modes.count == 0)
		{
			/* no presentation modes */
			continue;
		}

		/*
		* Check queue families
		*/
		_vlk_gpu_qfi_t* qfi = &gpu->queue_family_indices;

		/* graphics */
		if (qfi->graphics_families.count == 0)
		{
			/* no graphics queue families */
			continue;
		}

		/* presentation */
		if (qfi->present_families.count == 0)
		{
			/* no present queue families */
			continue;
		}

		/*
		* This GPU meets our requirements, use it
		*/

		/* init new gpu info for selected GPU and destroy the temp array */
		_vlk_gpu__init(&vlk->gpu, gpu->handle, vlk->surface);
		break;
	}

	/* verify a physical device was selected */
	if (vlk->gpu.handle == VK_NULL_HANDLE) 
	{
		log__fatal("Failed to find a suitable GPU.");
	}

	/* Cleanup temp GPU info array */
	for (i = 0; i < num_gpus; i++)
	{
		_vlk_gpu__term(&gpus.data[i]);
	}

	utl_array_destroy(&gpus);
	utl_array_destroy(&device_ext);
	utl_array_destroy(&devices);
}

/**
_vlk_setup__create_device
*/
void _vlk_setup__create_device(_vlk_t* vlk)
{
	/* Select a physical device */
	select_physical_device(vlk);

	/* Create logical device */
	_vlk_device__construct(vlk, &vlk->dev, &vlk->gpu, &vlk->req_dev_ext, &vlk->req_inst_layers);
}

/**
_vlk_setup__create_instance
*/
void _vlk_setup__create_instance(_vlk_t* vlk)
{
	// steam validation layers started causing issues after a Steam update.
	//_putenv("DISABLE_VK_LAYER_VALVE_steam_overlay_1=1");

	/*
	* Build instance creation data
	*/

	/* build app info */
	VkApplicationInfo app_info;
	clear_struct(&app_info);
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Jetz PSP";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Jetz PSP Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	/* build create info */
	VkInstanceCreateInfo create_info;
	clear_struct(&create_info);
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	/* get required Vulkan extensions */
	create_info.enabledExtensionCount = vlk->req_inst_ext.count;
	create_info.ppEnabledExtensionNames = vlk->req_inst_ext.data;

	/* get validation layers */
	create_info.enabledLayerCount = vlk->req_inst_layers.count;
	create_info.ppEnabledLayerNames = vlk->req_inst_layers.data;

	/*
	* Create the instance
	*/
	VkResult result = vkCreateInstance(&create_info, NULL, &vlk->instance);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to create Vulkan instance.");
	}
}

/**
_vlk_setup__create_pipelines
*/
void _vlk_setup__create_pipelines(_vlk_t* vlk)
{
	_vlk_md5_pipeline__construct(&vlk->md5_pipeline, &vlk->dev, vlk->swapchain.render_pass, vlk->swapchain.extent);
	_vlk_obj_pipeline__construct(&vlk->obj_pipeline, &vlk->dev, vlk->swapchain.render_pass, vlk->swapchain.extent);
	_vlk_plane_pipeline__construct(&vlk->plane_pipeline, &vlk->dev, vlk->swapchain.render_pass, vlk->swapchain.extent);
}

/**
_vlk_setup__create_requirement_lists
*/
void _vlk_setup__create_requirement_lists(_vlk_t* vlk)
{
	create_required_device_extensions_list(vlk);
	create_required_instance_layers_list(vlk);
	create_required_instance_extensions_list(vlk);
}

/**
_vlk_setup__create_surface
*/
void _vlk_setup__create_surface(_vlk_t* vlk)
{
	VkResult result = glfwCreateWindowSurface(vlk->instance, vlk->window, NULL, &vlk->surface);

	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to create window surface.");
	}
}

/**
_vlk_setup__create_swapchain
*/
void _vlk_setup__create_swapchain(_vlk_t* vlk)
{
	_vlk_swapchain__init(&vlk->swapchain, &vlk->dev, vlk->surface, vlk->window);
}

/**
_vlk_setup__destroy_device
*/
void _vlk_setup__destroy_device(_vlk_t* vlk)
{
	_vlk_device__destruct(&vlk->dev);
	_vlk_gpu__term(&vlk->gpu);
}

/**
_vlk_setup__destroy_instance
*/
void _vlk_setup__destroy_instance(_vlk_t* vlk)
{
	vkDestroyInstance(vlk->instance, NULL);
}

/**
_vlk_setup__destroy_pipelines
*/
void _vlk_setup__destroy_pipelines(_vlk_t* vlk)
{
	_vlk_md5_pipeline__destruct(&vlk->md5_pipeline);
	_vlk_obj_pipeline__destruct(&vlk->obj_pipeline);
	_vlk_plane_pipeline__destruct(&vlk->plane_pipeline);
}

/**
_vlk_setup__destroy_requriement_lists
*/
void _vlk_setup__destroy_requirement_lists(_vlk_t* vlk)
{
	utl_array_destroy(&vlk->req_dev_ext);
	utl_array_destroy(&vlk->req_inst_ext);
	utl_array_destroy(&vlk->req_inst_layers);
}

/**
_vlk_setup__destroy_surface
*/
void _vlk_setup__destroy_surface(_vlk_t* vlk)
{
	vkDestroySurfaceKHR(vlk->instance, vlk->surface, NULL);
}

/**
_vlk_setup__destroy_swapchain
*/
void _vlk_setup__destroy_swapchain(_vlk_t* vlk)
{
	_vlk_swapchain__term(&vlk->swapchain);
}
