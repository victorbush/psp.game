/*=========================================================
INCLUDES
=========================================================*/

#include <malloc.h>

#include "common.h"
#include "ecs/components.h"
#include "gpu/gpu.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "platform/glfw/glfw.h"
#include "thirdparty/cglm/include/cglm/affine.h"
#include "thirdparty/cglm/include/cglm/vec3.h"
#include "thirdparty/md5/md5model.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "thirdparty/stb/stb_image.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

static GLFWwindow* s_glfw_window;

/*=========================================================
DECLARATIONS
=========================================================*/

static void gpu_begin_frame(gpu_t* gpu, camera_t* cam);
static void gpu_create_model(gpu_t* gpu, gpu_model_t* model);
static void gpu_destroy_model(gpu_t* gpu, gpu_model_t* model);
static void gpu_end_frame(gpu_t* gpu);
static void gpu_init(gpu_t* gpu);
static gpu_material_t* gpu_load_material(gpu_t* gpu, const char* filename);
static gpu_static_model_t* gpu_load_static_model(gpu_t* gpu, const char* filename);
static gpu_texture_t* gpu_load_texture(gpu_t* gpu, const char* filename);
static void gpu_render_model(gpu_t* gpu, gpu_model_t* model, transform_comp_t* transform);
static void gpu_render_plane(gpu_t* gpu, gpu_plane_t* plane, transform_comp_t* transform);
static void gpu_render_static_model(gpu_t* gpu, gpu_static_model_t* model, transform_comp_t* transform);
static void gpu_term(gpu_t* gpu);
static void gpu_unload_materials(gpu_t* gpu);
static void gpu_unload_static_models(gpu_t* gpu);
static void gpu_unload_textures(gpu_t* gpu);
static void gpu_wait_idle(gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void vlk__init(gpu_t* gpu, platform_t* platform, GLFWwindow* window)
{
	memset(gpu, 0, sizeof(*gpu));
	gpu->platform = platform;

	gpu->begin_frame = &gpu_begin_frame;
	gpu->create_model = &gpu_create_model;
	gpu->destroy_model = &gpu_destroy_model;
	gpu->end_frame = &gpu_end_frame;
	gpu->init = &gpu_init;
	gpu->load_material = &gpu_load_material;
	gpu->load_static_model = &gpu_load_static_model;
	gpu->load_texture = &gpu_load_texture;
	gpu->render_model = &gpu_render_model;
	gpu->render_plane = &gpu_render_plane;
	gpu->render_static_model = &gpu_render_static_model;
	gpu->term = &gpu_term;
	gpu->unload_materials = &gpu_unload_materials;
	gpu->unload_static_models = &gpu_unload_static_models;
	gpu->unload_textures = &gpu_unload_textures;
	gpu->wait_idle = &gpu_wait_idle;

	s_glfw_window = window;
}

static void gpu_begin_frame(gpu_t* gpu, camera_t* cam)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_swapchain__begin_frame(&vlk->swapchain, vlk);

	/* Get current frame data */
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk->dev.per_view_set, frame, cam, vlk->swapchain.extent);
	_vlk_per_view_set__bind(&vlk->dev.per_view_set, frame, vlk->plane_pipeline.layout);
}

static void gpu_create_model(gpu_t* gpu, gpu_model_t* model)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	/* Load the model file */
	// TODO
	ReadMD5Model("models\\bob_lamp\\bob_lamp_update.md5mesh", &model->md5);

	/* Allocate data used for the Vulkan GPU implementation */
	model->data = malloc(sizeof(_vlk_anim_model_t));
	if (!model->data)
	{
		FATAL("Failed to allocate memory for model.");
	}

	/* Initialize GPU data */
	_vlk_anim_model__construct((_vlk_anim_model_t*)model->data, &vlk->dev, &model->md5);
}

static void gpu_destroy_model(gpu_t* gpu, gpu_model_t* model)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	/* Free GPU data */
	_vlk_anim_model__destruct((_vlk_anim_model_t*)model->data);
	free(model->data);

	/* Free MD5 data */
	FreeModel(&model->md5);
}

static void gpu_end_frame(gpu_t* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_swapchain__end_frame(&vlk->swapchain);
}

static void gpu_init(gpu_t* gpu)
{
	_vlk_t* vlk = malloc(sizeof(_vlk_t));
	if (!vlk)
	{
		FATAL("Failed to allocate Vulkan context.");
	}

	gpu->context = vlk;
	clear_struct(vlk);
	vlk->window = s_glfw_window;
	vlk->enable_validation = TRUE;

	map_init(&vlk->static_models);

	_vlk_setup__create_requirement_lists(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_dbg__create_dbg_callbacks(vlk);
	_vlk_setup__create_surface(vlk);
	_vlk_setup__create_device(vlk);
	_vlk_setup__create_swapchain(vlk);
	_vlk_setup__create_pipelines(vlk);
}

gpu_material_t* gpu_load_material(gpu_t* gpu, const char* filename)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	gpu_material_t** cached_material = NULL;

	/* Check if material is already in cache */
	cached_material = map_get(&vlk->materials, filename);
	if (cached_material)
	{
		return *cached_material;
	}

	/* Load model file */
	const char* data;
	long size;

	if (!gpu->platform->load_file(filename, &size, &data))
	{
		FATAL("Failed to load material.");
	}

	/* Allocate memory for the model */
	gpu_material_t* material = malloc(sizeof(gpu_material_t));
	if (!material)
	{
		FATAL("Failed to allocate memory for material.");
	}

	/* Allocate data used for the Vulkan GPU implementation */
	material->data = malloc(sizeof(_vlk_material_t));
	if (!material->data)
	{
		FATAL("Failed to allocate memory for material.");
	}

	/* Parse the file */
	if (!gpu__parse_material(data, size, material))
	{
		FATAL("Failed to parse material file.");
	}

	/* Load textures */
	// TODO

	/* Construct the material */
	_vlk_material__construct((_vlk_material_t*)material->data, &vlk->dev, );

	/* Free file buffer */
	free(data);

	/* Register the material in the cache */
	if (map_set(&vlk->materials, filename, material))
	{
		FATAL("Failed to register material in cache.");
	}

	return material;
}

gpu_static_model_t* gpu_load_static_model(gpu_t* gpu, const char* filename)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	gpu_static_model_t** cached_model = NULL;

	/* Check if model is already in cache */
	cached_model = map_get(&vlk->static_models, filename);
	if (cached_model)
	{
		return *cached_model;
	}

	/* Load model file */
	const char* data;
	long size;
	tinyobj_t obj;

	if (!gpu->platform->load_file(filename, &size, &data))
	{
		FATAL("Failed to load model.");
	}

	/* Parse the file */
	int result = tinyobj_parse_obj(&obj.attrib, &obj.shapes, &obj.shapes_cnt, &obj.materials, &obj.materials_cnt, data, size, TINYOBJ_FLAG_TRIANGULATE);
	if (result != TINYOBJ_SUCCESS)
	{
		FATAL("Failed to parse model.");
	}

	/* Allocate memory for the model */
	gpu_static_model_t* model = malloc(sizeof(gpu_static_model_t));
	if (!model)
	{
		FATAL("Failed to allocate memory for static model.");
	}
	
	/* Allocate data used for the Vulkan GPU implementation */
	model->data = malloc(sizeof(_vlk_static_model_t));
	if (!model->data)
	{
		FATAL("Failed to allocate memory for model.");
	}

	/* Construct the model */
	_vlk_static_model__construct((_vlk_static_model_t*)model->data, &vlk->dev, &obj);

	/* Free file buffer */
	free(data);

	/* Free obj */
	tinyobj_attrib_free(&obj.attrib);
	tinyobj_shapes_free(obj.shapes, obj.shapes_cnt);
	tinyobj_materials_free(obj.materials, obj.materials_cnt);

	/* Register the model in the cache */
	if (map_set(&vlk->static_models, filename, model))
	{
		FATAL("Failed to register static model in cache.");
	}

	return model;
}
//
//gpu_texture_t* gpu_load_texture(gpu_t* gpu, const char* filename)
//{
//	_vlk_t* vlk = (_vlk_t*)gpu->context;
//	gpu_texture_t** cached_texture = NULL;
//
//	/* Check if texture is already in cache */
//	cached_texture = map_get(&vlk->textures, filename);
//	if (cached_texture)
//	{
//		return *cached_texture;
//	}
//
//	/* Load model file */
//	const char* data;
//	long size;
//
//	if (!gpu->platform->load_file(filename, &size, &data))
//	{
//		FATAL("Failed to load material.");
//	}
//
//
//
//	/* Allocate memory for the model */
//	gpu_material_t* material = malloc(sizeof(gpu_material_t));
//	if (!material)
//	{
//		FATAL("Failed to allocate memory for material.");
//	}
//
//	/* Allocate data used for the Vulkan GPU implementation */
//	material->data = malloc(sizeof(_vlk_material_t));
//	if (!material->data)
//	{
//		FATAL("Failed to allocate memory for material.");
//	}
//
//	/* Parse the file */
//	if (!gpu__parse_material(data, size, material))
//	{
//		FATAL("Failed to parse material file.");
//	}
//
//	/* Load textures */
//	// TODO
//
//	/* Construct the material */
//	_vlk_material__construct((_vlk_material_t*)material->data, &vlk->dev, );
//
//	/* Free file buffer */
//	free(data);
//
//	/* Register the material in the cache */
//	if (map_set(&vlk->materials, filename, material))
//	{
//		FATAL("Failed to register material in cache.");
//	}
//
//	return material;
//}

static void gpu_render_model(gpu_t* gpu, gpu_model_t* model, transform_comp_t* transform)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	// TODO : When to bind pipeline? Every time render model is called?

	_vlk_md5_pipeline__bind(&vlk->md5_pipeline, frame->cmd_buf);
	_vlk_anim_model__render((_vlk_anim_model_t*)model->data, frame->cmd_buf, transform);
}

static void gpu_render_plane(gpu_t* gpu, gpu_plane_t* plane, transform_comp_t* transform)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind the plane pipeline */
	_vlk_plane_pipeline__bind(&vlk->plane_pipeline, frame->cmd_buf);

	/* Update push constants */
	_vlk_plane_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);
	glm_translate(&pc.vertex.model_matrix, &transform->pos);
	glm_vec3_copy(&plane->color, &pc.fragment.color);
	pc.vertex.anchor.x = plane->anchor.x;
	pc.vertex.anchor.y = plane->anchor.y;
	pc.vertex.height = plane->height;
	pc.vertex.width = plane->width;

	uint32_t pcVertSize = sizeof(_vlk_plane_push_constant_vertex_t);
	uint32_t pcFragSize = sizeof(_vlk_plane_push_constant_fragment_t);
	uint32_t pcFragOffset = offsetof(_vlk_plane_push_constant_t, fragment);

	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);
	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_FRAGMENT_BIT, pcFragOffset, pcFragSize, &pc.fragment);

	/* Draw the plane */
	vkCmdDrawIndexed(frame->cmd_buf, 6, 1, 0, 0, 0);
}

void gpu_render_static_model(gpu_t* gpu, gpu_static_model_t* model, transform_comp_t* transform)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;
	_vlk_frame_t* frame = &vlk->swapchain.frame;

	/* Bind pipeline */
	_vlk_obj_pipeline__bind(&vlk->obj_pipeline, frame->cmd_buf);

	/* Update push constants */
	_vlk_obj_push_constant_t pc;
	clear_struct(&pc);

	glm_mat4_identity(&pc.vertex.model_matrix);
	glm_translate(&pc.vertex.model_matrix, &transform->pos);

	uint32_t pcVertSize = sizeof(_vlk_obj_push_constant_vertex_t);

	vkCmdPushConstants(frame->cmd_buf, vlk->plane_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pc.vertex);

	/* Render the model */
	_vlk_static_model__render((_vlk_static_model_t*)model->data, frame->cmd_buf);
}

static void gpu_term(gpu_t* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	gpu_unload_static_models(gpu);
	map_deinit(&vlk->static_models);

	_vlk_setup__destroy_pipelines(vlk);
	_vlk_setup__destroy_swapchain(vlk);
	_vlk_setup__destroy_device(vlk);
	_vlk_setup__destroy_surface(vlk);
	_vlk_dbg__destroy_dbg_callbacks(vlk);
	_vlk_setup__create_instance(vlk);
	_vlk_setup__destroy_requirement_lists(vlk);
	free(gpu->context);
}

void gpu_unload_static_models(gpu_t* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	const char* key;
	map_iter_t iter = map_iter(&vlk->static_models);

	while ((key = map_next(&vlk->static_models, &iter)))
	{
		/* The map returns a pointer to the value, so need a double pointer here */
		gpu_static_model_t** model = (gpu_static_model_t**)map_get(&vlk->static_models, key);
		if (!model)
		{
			continue;
		}

		/* Free Vulkan data */
		_vlk_static_model__destruct((_vlk_static_model_t*)(*model)->data);
		free((*model)->data);

		/* Free model data */
		free((*model));
	}

	/* Clear cache */
	map_deinit(&vlk->static_models);
	map_init(&vlk->static_models);
}

void gpu_wait_idle(gpu_t* gpu)
{
	_vlk_t* vlk = (_vlk_t*)gpu->context;

	/* wait for device to finsih current operations. example usage is at
	application exit - wait until current ops finish, then do cleanup. */
	vkDeviceWaitIdle(vlk->dev.handle);
}
