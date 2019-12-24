/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/pspgu/pspgu_prv.h"
#include "utl/utl_log.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _pspgu_texture__construct
	(
	_pspgu_texture_t*			texture,
	_pspgu_t*					ctx,
	void*						img,
	uint32_t					width,
	uint32_t					height,
	uint32_t					size_in_bytes
	)
{
	clear_struct(texture);

	/* Allocate memory for the texture data and copy the texture */
	texture->data = malloc(size_in_bytes);
	if (!texture->data)
	{
		FATAL("Failed to allocate memory for texture.");
	}

	memcpy(texture->data, img, size_in_bytes);
	texture->height = height;
	texture->width = width;
	texture->size_in_bytes = size_in_bytes;
}

void _pspgu_texture__destruct(_pspgu_texture_t* texture)
{
	/* Free texture memory */
	free(texture->data);
}

/*=========================================================
FUNCTIONS
=========================================================*/
