/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_texture.h"
#include "thirdparty/stb/stb_image.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_texture__construct(gpu_texture_t* texture, gpu_t* gpu, const char* filename)
{
	clear_struct(texture);

	int width = 0;
	int height = 0;
	int channels = 0;

	stbi_uc* img = stbi_load(filename, &width, &height, &channels, 0);
	if (!img || !width || !height || !channels)
	{
		kk_log__fatal("Failed to load texture file.");
	}

	gpu->intf->texture__construct(texture, gpu, (void*)img, width, height);

	stbi_image_free(img);
}

void gpu_texture__construct_from_data(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height)
{
	clear_struct(texture);

	gpu->intf->texture__construct(texture, gpu, (void*)img, width, height);
}

void gpu_texture__destruct(gpu_texture_t* texture, gpu_t* gpu)
{
	gpu->intf->texture__destruct(texture, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/
