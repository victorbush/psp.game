/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_texture.h"
#include "thirdparty/stb/stb_image.h"
#include "utl/utl_log.h"

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

	//gpu->intf->texture__construct(texture, gpu);
}

void gpu_texture__destruct(gpu_texture_t* texture, gpu_t* gpu)
{
	//gpu->intf->texture__destruct(texture);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void load_texture(gpu_texture_t* texture, const char* filename)
{
	//int width, height, channels;
	//stbi_uc* img = stbi_load(filename, &width, &height, &channels, 0);
	//
}