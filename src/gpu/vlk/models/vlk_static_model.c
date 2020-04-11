/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/cglm/include/cglm/affine.h"
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
