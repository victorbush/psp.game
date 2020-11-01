/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/pspgu/pspgu.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_array.h"

#include "autogen/pspgu_static_model.static.h"

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
CONSTRUCTORS
=========================================================*/

//## internal
void _pspgu_static_model__construct
	(
	_pspgu_static_model_t*		model,
	_pspgu_t*					ctx,
	const tinyobj_t*			obj
	)
{
	clear_struct(model);

	create_meshes(model, ctx, obj);
}

//## internal
void _pspgu_static_model__destruct(_pspgu_static_model_t* model)
{
	destroy_meshes(model);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _pspgu_static_model__render
	(
	_pspgu_static_model_t*		model,
	_pspgu_t*					ctx
	)
{
	for (uint32_t i = 0; i < model->meshes.count; ++i)
	{
		_pspgu_static_mesh__render(&model->meshes.data[i], ctx);
	}
}

//## static
/** Initializes the meshes in the model. */
static void create_meshes(_pspgu_static_model_t* model, _pspgu_t* ctx, const tinyobj_t* obj)
{
	uint32_t num_meshes = obj->shapes_cnt;
	kk_log__dbg_fmt("Creating static meshes: %i", num_meshes);

	utl_array_init(&model->meshes);
	utl_array_resize(&model->meshes, num_meshes);

	for (uint32_t i = 0; i < num_meshes; ++i)
	{
		_pspgu_static_mesh__construct(&model->meshes.data[i], ctx, obj, &obj->shapes[i]);
	}
}

//## static
/** Destroys the meshes in the model. */
static void destroy_meshes(_pspgu_static_model_t* model)
{
	uint32_t num_meshes = 1;

	for (uint32_t i = 0; i < num_meshes; ++i)
	{
		_pspgu_static_mesh__destruct(&model->meshes.data[i]);
	}

	utl_array_destroy(&model->meshes);
}