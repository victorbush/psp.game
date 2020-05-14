/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_static_model.h"
#include "platform/platform.h"
#include "thirdparty/tinyobj/tinyobj.h"

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
	clear_struct(model);

	char* data;
	long size;
	tinyobj_t obj;

	/* Load model file */
	if (!g_platform->load_file(filename, FALSE, &size, (void*)&data))
	{
		kk_log__fatal("Failed to load model.");
	}

	/* Parse the file */
	int result = tinyobj_parse_obj(&obj.attrib, &obj.shapes, &obj.shapes_cnt, &obj.materials, &obj.materials_cnt, data, size, TINYOBJ_FLAG_TRIANGULATE);
	if (result != TINYOBJ_SUCCESS)
	{
		kk_log__fatal("Failed to parse model.");
	}

	/* Construct */
	gpu->intf->static_model__construct(model, gpu, &obj);

	/* Free file buffer */
	free(data);

	/* Free obj */
	tinyobj_attrib_free(&obj.attrib);
	tinyobj_shapes_free(obj.shapes, obj.shapes_cnt);
	tinyobj_materials_free(obj.materials, obj.materials_cnt);
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
	gpu->intf->static_model__render(model, gpu, window, frame, material, transform);
}
