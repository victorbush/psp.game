/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "platforms/common.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Vulkan debug callback
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
	(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char *layerPrefix,
	const char *msg,
	void *userData
	)
{
	LOG_ERROR(msg);
	return VK_FALSE;
}

/**
_vlk_dbg__create_dbg_callbacks
*/
void _vlk_dbg__create_dbg_callbacks(_vlk_t* vlk)
{
	/* only do this if validation layers are enabled*/
	if (!vlk->enable_validation)
	{
		return;
	}

	/* setup the create info */
	VkDebugReportCallbackCreateInfoEXT createInfo;
	clear_struct(&createInfo);
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	/*
	Get address to the create callback function. In order for this to work,
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME must be added to the list of required
	extensions when the Vulkan instance is created.
	*/
	PFN_vkCreateDebugReportCallbackEXT func;
	func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vlk->instance, "vkCreateDebugReportCallbackEXT");
	VkResult result = VK_SUCCESS;

	/* create the callback */
	if (func != NULL)
	{
		result = func(vlk->instance, &createInfo, NULL, &vlk->dbg_callback_hndl);
	}

	if (func == NULL || result != VK_SUCCESS)
	{
		FATAL("Failed to create Vulkan debug callback.");
	}
}

/**
_vlk_dbg__destroy_dbg_callbacks
*/
void _vlk_dbg__destroy_dbg_callbacks(_vlk_t* vlk)
{
	/* only do this if validation layers are enabled*/
	if (!vlk->enable_validation)
	{
		return;
	}

	/* get address to the create callback function*/
	PFN_vkDestroyDebugReportCallbackEXT func;
	func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vlk->instance, "vkDestroyDebugReportCallbackEXT");

	/* destroy the callback */
	if (func != NULL)
	{
		func(vlk->instance, vlk->dbg_callback_hndl, NULL);
	}
	else
	{
		FATAL("Failed to destroy Vulkan debug callback.");
	}
}