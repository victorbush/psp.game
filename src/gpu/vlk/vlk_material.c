/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates the descriptor set for the material. */
static void create_set
	(
	_vlk_material_t*			material, 
	_vlk_material_ubo_t*		ubo, 
	_vlk_descriptor_layout_t*	layout, 
	_vlk_texture_t*				diffuse_texture
	);

/** Destroys the descriptor set for the material. */
static void destroy_set(_vlk_material_t* material);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_material__construct
*/
void _vlk_material__construct
	(
	_vlk_material_t*			material,
	_vlk_material_ubo_t*		ubo,
	_vlk_descriptor_layout_t*	layout,
	_vlk_texture_t*				diffuse_texture
	)
{
	clear_struct(material);

	create_set(material, ubo, layout, diffuse_texture);
}

/**
_vlk_material__destruct
*/
void _vlk_material__destruct(_vlk_material_t* material)
{
	destroy_set(material);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void create_set(_vlk_material_t* material, _vlk_material_ubo_t* ubo, _vlk_descriptor_layout_t* layout, _vlk_texture_t* diffuse_texture)
{
	_vlk_material_set__construct(&material->descriptor_set, layout, ubo, diffuse_texture);
}

static void destroy_set(_vlk_material_t* material)
{
	_vlk_material_set__destruct(&material->descriptor_set);
}