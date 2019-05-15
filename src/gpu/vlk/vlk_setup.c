/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "platforms/common.h"
#include "platforms/glfw/glfw.h"
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
Gets a list of required instance extensions.
Caller responsible for freeing pointer.
*/
static utl_array_t(string) get_required_instance_extensions(_vlk_type* vlk)
{
	utl_array_create(string, extensions);
	int i;

	/* get extensions required by GLFW */
	uint32_t num_glfw_extensions = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);

	/* add glfw extensions */
	for (i = 0; i < num_glfw_extensions; ++i)
	{
		utl_array_push(&extensions, glfw_extensions[i]);
	}

	/* determine other extensions needed */
	if (vlk->enable_validation)
	{
		/* debug report extension is required to use validation layers */
		utl_array_push(&extensions, VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	/* throw error if the desired extensions are not available */
	if (!are_instance_extensions_available(&extensions))
	{
		assert(FALSE);
	}

	return extensions;
}

/**
Gets list of required instance layers (validation layers).
Caller responsible for freeing pointer.
*/
static utl_array_t(string) get_required_instance_layers(_vlk_type* vlk)
{
	utl_array_create(string, layers);

	/* add validation layers */
	if (vlk->enable_validation)
	{
		utl_array_push(&layers, "VK_LAYER_LUNARG_standard_validation");
	}

	if (!are_instance_layers_available(&layers))
	{
		assert(FALSE);
	}

	return layers;
}

/**
Checks if a specified list of device extensions are available.
*/
static boolean are_device_extensions_available
	(
	utl_array_t(string)*		extensions
	)
{
	
}

/**
Checks if a specified list of instance extensions are available.
*/
static boolean are_instance_extensions_available
	(
	utl_array_t(string)*		extensions
	)
{
	boolean desired_extensions_available = TRUE;
	int i, j;

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
	utl_array_t(string)*	layers
	)
{
	boolean desired_layers_available = TRUE;
	int i;

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
		int j;

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
Selects a physical GPU.
*/
static VkPhysicalDevice select_physical_device(_vlk_type* vlk)
{
	utl_array_t(VkPhysicalDevice) devices;
	utl_array_t(_vlk_gpu_t) gpus;
	utl_array_t(string) device_ext;	/* required device extensions */
	int i;

	/* check assumptions */
	if (vlk->instance == VK_NULL_HANDLE)
	{
		FATAL("Vulkan instance must be created before selecting a phyiscal device.");
	}

	if (vlk->surface == VK_NULL_HANDLE)
	{
		FATAL("Vulkan surface must be created before selecting a phyiscal device.");
	}

	/*
	* Device extensions
	*/

	/* clear list */
	//_requiredDeviceExtensions.clear();

	/* swap chain support is required */
	device_ext.
	//_requiredDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);


	/* get number of GPUs */
	uint32_t num_gpus = 0;
	vkEnumeratePhysicalDevices(vlk->instance, &num_gpus, NULL);
	if (num_gpus == 0)
	{
		FATAL("No GPU found with Vulkan support.");
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
	
	/* inspect the physical devices and select one */
	for (i = 0; i < num_gpus; ++i)
	{
		_vlk_gpu_t* gpu = &devices.data[i];
		
		/*
		* Check required extensions
		*/
		if (!areDeviceExtensionsAvailable(_requiredDeviceExtensions, gpu))
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
		_vlk_gpu_qfi_type* qfi = &gpu->queue_family_indices;

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
		FATAL("Failed to find a suitable GPU.");
	}

	/* Cleanup temp GPU info array */
	for (i = 0; i < num_gpus; i++)
	{
		_vlk_gpu__term(&gpus.data[i]);
	}
	utl_array_destroy(&gpus);
}

/**
_vlk_setup__create_device
*/
void _vlk_setup__create_device(_vlk_type* vlk)
{
	/* Select a physical device */
	select_physical_device(vlk);
}

/**
_vlk_setup__create_instance
*/
void _vlk_setup__create_instance(_vlk_type* vlk)
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
	utl_array_t(string) extensions = get_required_instance_extensions(vlk);
	create_info.enabledExtensionCount = extensions.count;
	create_info.ppEnabledExtensionNames = extensions.data;

	/* get validation layers */
	utl_array_t(string) layers = get_required_instance_layers(vlk);
	create_info.enabledLayerCount = layers.count;
	create_info.ppEnabledLayerNames = layers.data;

	/*
	* Create the instance
	*/
	VkResult result = vkCreateInstance(&create_info, NULL, &vlk->instance);
	if (result != VK_SUCCESS)
	{
		FATAL("Failed to create Vulkan instance.");
	}

	utl_array_destroy(&extensions);
	utl_array_destroy(&layers);
}

/**
_vlk_setup__create_surface
*/
void _vlk_setup__create_surface(_vlk_type* vlk)
{
	VkResult result = glfwCreateWindowSurface(vlk->instance, vlk->window, NULL, &vlk->surface);

	if (result != VK_SUCCESS)
	{
		FATAL("Failed to create window surface.");
	}
}

/**
_vlk_setup__destroy_device
*/
void _vlk_setup__destroy_device(_vlk_type* vlk)
{
	_vlk_gpu__term(&vlk->gpu);
}

/**
_vlk_setup__destroy_instance
*/
void _vlk_setup__destroy_instance(_vlk_type* vlk)
{
	vkDestroyInstance(vlk->instance, NULL);
}

/**
_vlk_setup__destroy_surface
*/
void _vlk_setup__destroy_surface(_vlk_type* vlk)
{
	vkDestroySurfaceKHR(vlk->instance, vlk->surface, NULL);
}

/**
_vlk_determine_requirements
*/
void _vlk_determine_requirements(_vlk_type* vlk, VkApplicationInfo* app_info)
{


	/*
	* Instance extensions
	*/

	

	/*
	* Device extensions
	*/
	
	/* clear list */
	//_requiredDeviceExtensions.clear();

	/* swap chain support is required */
	//_requiredDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/* device extensions aren't validated here since a phyiscal device is required */
}