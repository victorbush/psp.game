
/**
Gets the pixel color value at the specified coordinate.

https://github.com/SaschaWillems/Vulkan/blob/master/examples/screenshot/screenshot.cpp
*/
static int read_pixel
	(
	_vlk_dev_t*					dev,
	VkImage						src_image,
	VkFormat					src_format,
	uint32_t					width,
	uint32_t					height,
	float						x,
	float						y
	)
;
