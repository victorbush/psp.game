/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/models/vlk_static_mesh.h"
#include "gpu/vlk/models/vlk_static_model.h"
#include "thirdparty/cglm/include/cglm/affine.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_array.h"

#include "autogen/vlk_static_model.static.h"

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Constructs a static model.
*/
void vlk_static_model__construct
	(
	gpu_static_model_t*			base,
	gpu_t*						gpu,
	const tinyobj_t*			obj
	)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Allocate data used for the Vulkan implementation */
	base->data = malloc(sizeof(_vlk_static_model_t));
	if (!base->data)
	{
		kk_log__fatal("Failed to allocate memory for static model.");
	}

	_vlk_static_model_t* vlk_model = _vlk_static_model__from_base(base);
	clear_struct(vlk_model);
	vlk_model->base = base;
	vlk_model->vlk = vlk;

	create_material_set(vlk_model, &vlk->dev);
	create_meshes(vlk_model, &vlk->dev, obj);
}

//## public
/**
Destructs a static model.
*/
void vlk_static_model__destruct(gpu_static_model_t* base, gpu_t* gpu)
{
	_vlk_static_model_t* vlk_model = _vlk_static_model__from_base(base);

	/* Cleanup */
	destroy_meshes(vlk_model);
	destroy_material_set(vlk_model);

	/* Free Vulkan implementation memory */
	free(base->data);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Renders the model to the picker buffer.
*/
void vlk_static_model__render_to_picker_buffer
	(
	gpu_static_model_t*		model, 
	gpu_t*					gpu, 
	gpu_window_t*			window, 
	gpu_frame_t*			frame,
	kk_vec4_t				color,
	ecs_transform_t*		transform
	)
{
	_vlk_t* vlk = _vlk__from_base(gpu);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);

	/* Bind picker pipeline */
	_vlk_picker_pipeline__bind(&vlk_window->picker_pipeline, vlk_frame->picker_cmd_buf);

	/* Bind per-view descriptor set */
	_vlk_per_view_set__bind(&vlk_window->per_view_set, vlk_frame->picker_cmd_buf, vlk_frame, vlk_window->picker_pipeline.layout);

	/* Update push constants */
	_vlk_picker_push_constant_t picker_pc;
	clear_struct(&picker_pc);

	glm_mat4_identity(&picker_pc.vertex.model_matrix);
	glm_translate(&picker_pc.vertex.model_matrix, &transform->pos);

	/* Set the color to render the object - this is the object id */
	picker_pc.frag.id_color = color;

	uint32_t picker_pc_vert_size = sizeof(_vlk_picker_push_constant_vertex_t);
	uint32_t picker_pc_frag_size = sizeof(_vlk_picker_push_constant_frag_t);
	uint32_t picker_pc_frag_offset = offsetof(_vlk_picker_push_constant_t, frag);

	vkCmdPushConstants(vlk_frame->picker_cmd_buf, vlk_window->picker_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, picker_pc_vert_size, &picker_pc.vertex);
	vkCmdPushConstants(vlk_frame->picker_cmd_buf, vlk_window->picker_pipeline.layout, VK_SHADER_STAGE_FRAGMENT_BIT, picker_pc_frag_offset, picker_pc_frag_size, &picker_pc.frag);

	/* Render the model */
	_vlk_static_model__render((_vlk_static_model_t*)model->data, vlk_frame->picker_cmd_buf);
}

//## public
_vlk_static_model_t* _vlk_static_model__from_base(gpu_static_model_t* base)
{
	return (_vlk_static_model_t*)base->data;
}

//## public
/**
Renders a static model. The appropriate pipeline must already be bound.
*/
void _vlk_static_model__render
	(
	_vlk_static_model_t*		model,
	VkCommandBuffer				cmd
	)
{
	/* Render each mesh in the model */
	for (uint32_t i = 0; i < model->meshes.count; ++i)
	{
		_vlk_static_mesh__render(&model->meshes.data[i], cmd);
	}
}

//## static
static void create_material_set(_vlk_static_model_t* model, _vlk_dev_t* device)
{
	/*
	Create the material descriptor set. It will reference all materials for the model. Each vertex will
	map to a material index.
	*/
	_vlk_material_set__construct(&model->material_set, model->vlk, &device->material_layout, model->base->materials.data, model->base->materials.count);
}

//## static
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

//## static
static void destroy_material_set(_vlk_static_model_t* model)
{
	_vlk_material_set__destruct(&model->material_set);
}

//## static
static void destroy_meshes(_vlk_static_model_t* model)
{
	uint32_t num_meshes = 1;

	for (uint32_t i = 0; i < num_meshes; ++i)
	{
		_vlk_static_mesh__destruct(&model->meshes.data[i]);
	}

	utl_array_destroy(&model->meshes);
}
