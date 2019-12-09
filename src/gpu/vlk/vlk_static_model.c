/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_model.h"
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

/** Initializes the meshes in the model. */
static void create_meshes(_vlk_static_model_t* model, _vlk_dev_t* device);

/** Destroys the meshes in the model. */
static void destroy_meshes(_vlk_static_model_t* model);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_static_model__construct
	(
	_vlk_static_model_t*			model,
	_vlk_dev_t*						device,
	const md5_model_t*				md5
	)
{
	clear_struct(model);
	model->md5 = md5;

	create_meshes(model, device);
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
	VkCommandBuffer				cmd,
	transform_comp_t*			transform
	)
{
	for (uint32_t i = 0; i < model->meshes.count; ++i)
	{
		_vlk_static_mesh__render(&model->meshes.data[i], cmd, transform);
	}
}

void create_meshes(_vlk_static_model_t* model, _vlk_dev_t* device)
{
	utl_array_init(&model->meshes);
	utl_array_resize(&model->meshes, model->md5->num_meshes);

	for (uint32_t i = 0; i < model->md5->num_meshes; ++i)
	{
		_vlk_static_mesh__construct(&model->meshes.data[i], device, &model->md5->meshes[i]);
		_vlk_static_mesh__prepare(&model->meshes.data[i], model->md5->baseSkel);
	}
}

void destroy_meshes(_vlk_static_model_t* model)
{
	for (uint32_t i = 0; i < model->md5->num_meshes; ++i)
	{
		_vlk_static_mesh__destruct(&model->meshes.data[i]);
	}

	utl_array_destroy(&model->meshes);
}
