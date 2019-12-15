/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates the descriptor set for the material. */
static void create_set(_vlk_material_t* material, _vlk_descriptor_layout_t* layout);

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
	_vlk_descriptor_layout_t*	layout,
	vec3_t						ambient_color,
	vec3_t						diffuse_color,
	vec3_t						specular_color,
	const _vlk_texture_t*		diffuse_texture
	)
{
	clear_struct(material);
	material->ubo.ambient = ambient_color;
	material->ubo.diffuse = diffuse_color;
	material->ubo.specular = specular_color;
	material->diffuse_texture = diffuse_texture;

	create_set(material, layout);
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

static void create_set(_vlk_material_t* material, _vlk_descriptor_layout_t* layout)
{
	_vlk_material_set__construct(&material->descriptor_set, layout, &material->ubo);
}

static void destroy_set(_vlk_material_t* material)
{
	_vlk_material_set__destruct(&material->descriptor_set);
}