#ifndef GPU_TEXTURE_H
#define GPU_TEXTURE_H

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_texture_s gpu_texture_t;

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
Destructs a texture.

@param texture The texture to destruct.
@param gpu The GPU context.
*/
void gpu_texture__destruct(gpu_texture_t* model, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_TEXTURE_H */