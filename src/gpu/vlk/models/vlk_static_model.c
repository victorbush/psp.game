/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "log/log.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Initializes the meshes in the model. */
static void create_meshes(_vlk_static_model_t* model, _vlk_dev_t* device, const tinyobj_t* obj);

/** Destroys the meshes in the model. */
static void destroy_meshes(_vlk_static_model_t* model);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_static_model__construct
	(
	_vlk_static_model_t*		model,
	_vlk_dev_t*					device,
	const tinyobj_t*			obj
	)
{
	clear_struct(model);

	create_meshes(model, device, obj);
}

void _vlk_static_model__destruct(_vlk_static_model_t* model)
{
	destroy_meshes(model);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void _vlk_static_model__render
	(
	_vlk_static_model_t*		model,
	VkCommandBuffer				cmd
	)
{
	for (uint32_t i = 0; i < model->meshes.count; ++i)
	{
		_vlk_static_mesh__render(&model->meshes.data[i], cmd);
	}
}

static void create_meshes(_vlk_static_model_t* model, _vlk_dev_t* device, const tinyobj_t* obj)
{
	uint32_t num_meshes = obj->shapes_cnt;

	utl_array_init(&model->meshes);
	utl_array_resize(&model->meshes, num_meshes);

	for (uint32_t i = 0; i < num_meshes; ++i)
	{
		_vlk_static_mesh__construct(&model->meshes.data[i], device, obj, &obj->shapes[i]);
	}
}

static void destroy_meshes(_vlk_static_model_t* model)
{
	uint32_t num_meshes = 1;

	for (uint32_t i = 0; i < num_meshes; ++i)
	{
		_vlk_static_mesh__destruct(&model->meshes.data[i]);
	}

	utl_array_destroy(&model->meshes);
}
