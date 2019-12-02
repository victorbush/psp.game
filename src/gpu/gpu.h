#ifndef GPU_INTF_H
#define GPU_INTF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/components.h"
#include "platforms/platform.h"
#include "gpu/gpu_model.h"
#include "gpu/gpu_plane.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_s gpu_t;

typedef void (*gpu_init_func)(gpu_t* gpu);
typedef void (*gpu_term_func)(gpu_t* gpu);
typedef void (*gpu_begin_frame_func)(gpu_t* gpu);
typedef void (*gpu_end_frame_func)(gpu_t* gpu);

typedef void (*gpu_create_model_func)(gpu_t* gpu, gpu_model_t* model);
typedef void (*gpu_destroy_model_func)(gpu_t* gpu, gpu_model_t* model);
typedef void (*gpu_render_model_func)(gpu_t* gpu, gpu_model_t* model, transform_comp_t* transform);
typedef void (*gpu_render_plane_func)(gpu_t* gpu, gpu_plane_t* plane, transform_comp_t* transform);
typedef void (*gpu_test_func)(gpu_t* gpu);

struct gpu_s
{
	gpu_init_func 			init;
	gpu_term_func 			term;
	gpu_begin_frame_func	begin_frame;
	gpu_end_frame_func		end_frame;

	void*			context;


	gpu_test_func test;

	gpu_create_model_func		create_model;
	gpu_destroy_model_func		destroy_model;
	gpu_render_model_func		render_model;

	gpu_render_plane_func		render_plane;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_INTF_H */