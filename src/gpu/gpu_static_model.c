/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_static_model.h"
#include "platform/platform.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_array.h"

#include "autogen/gpu_static_model.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Constructs a static model.

@param model The model to construct.
@param gpu The GPU context.
@param filename The model file to load.
*/
void gpu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const char* filename)
{
	kk_log__dbg_fmt("gpu_static_model__construct: %s", filename);

	clear_struct(model);
	utl_array_init(&model->materials);

	long size;
	tinyobj_t obj;

	/* Parse the file */
	int result = tinyobj_parse_obj(&obj.attrib, &obj.shapes, &obj.shapes_cnt, &obj.materials, &obj.materials_cnt, filename, file_reader, TINYOBJ_FLAG_TRIANGULATE);
	if (result != TINYOBJ_SUCCESS)
	{
		kk_log__fatal("Failed to parse model.");
	}

	kk_log__dbg("gpu_static_model__construct - file parsed");

	/* Load materials */
	utl_array_resize(&model->materials, obj.materials_cnt);
	for (int i = 0; i < obj.materials_cnt; ++i)
	{
		gpu_material_t* mat = &model->materials.data[i];
		gpu_material__construct_from_tinyobj(mat, gpu, &obj.materials[i]);
	}

	kk_log__dbg("gpu_static_model__construct - materials loaded");

	/* Construct */
	gpu->intf->static_model__construct(model, gpu, &obj);

	/* Free obj */
	tinyobj_attrib_free(&obj.attrib);
	tinyobj_shapes_free(obj.shapes, obj.shapes_cnt);
	tinyobj_materials_free(obj.materials, obj.materials_cnt);

	kk_log__dbg("gpu_static_model__construct - done");
}

//## public
/**
Destructs a static model.

@param model The model to destruct.
@param gpu The GPU context.
*/
void gpu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu)
{
	gpu->intf->static_model__destruct(model, gpu);

	/* Cleanup materials */
	for (uint32_t i = 0; i < model->materials.count; ++i)
	{
		gpu_material__destruct(&model->materials.data[i], gpu);
	}

	utl_array_destroy(&model->materials);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void gpu_static_model__render
	(
	gpu_static_model_t*		model, 
	gpu_t*					gpu, 
	gpu_window_t*			window, 
	gpu_frame_t*			frame, 
	gpu_material_t*			material, 
	ecs_transform_t*		transform
	)
{
	gpu->intf->static_model__render(model, gpu, window, frame, transform);
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

//## static
static void file_reader
	(
	const char*		filename,
	int				is_mtl,
	const char*		obj_filename,
	char**			buf,
	size_t*			len
	)
{
	char path[256];

	if (is_mtl)
	{
		sprintf_s(path, sizeof(path), "models/%s", filename);
	}
	else
	{
		sprintf_s(path, sizeof(path), "models/%s", filename);
	}

	if (!g_platform->load_file(path, FALSE, (long*)len, (void**)buf))
	{
		kk_log__error("Failed to load file.");
		*len = 0;
		*buf = NULL;
	}
}
