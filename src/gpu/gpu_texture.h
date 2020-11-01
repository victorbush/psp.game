#ifndef GPU_TEXTURE_H
#define GPU_TEXTURE_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_texture_.h"

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct gpu_texture_s
{
	void*				data;		/* Pointer to GPU-specific data. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a texture.

@param texture The texture to construct.
@param gpu The GPU context.
@param filename The texture file to load.
*/
void gpu_texture__construct(gpu_texture_t* texture, gpu_t* gpu, const char* filename);

/**
Constructs a texture from memory.

@param texture The texture to construct.
@param gpu The GPU context.
@param img The texture data.
@param width The width (in pixels) of the image.
@param height The height (in pixels) of the image.
*/
void gpu_texture__construct_from_data(gpu_texture_t* texture, gpu_t* gpu, void* img, int width, int height);

/**
Destructs a texture.

@param texture The texture to destruct.
@param gpu The GPU context.
*/
void gpu_texture__destruct(gpu_texture_t* model, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_TEXTURE_H */