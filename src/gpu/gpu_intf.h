#ifndef GPU_INTF_H
#define GPU_INTF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "platforms/platform.h"
#include "gpu/gpu_model.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_struct gpu_type;

typedef void (*gpu_init_func)(gpu_type* gpu);
typedef void (*gpu_term_func)(gpu_type* gpu);
typedef void (*gpu_begin_frame_func)(gpu_type* gpu);
typedef void (*gpu_end_frame_func)(gpu_type* gpu);

typedef void (*gpu_render_model_func)(gpu_type* gpu, const vec3_t* pos);
typedef void (*gpu_test_func)(gpu_type* gpu);


struct gpu_struct
{
	gpu_init_func 			init;
	gpu_term_func 			term;
	gpu_begin_frame_func	begin_frame;
	gpu_end_frame_func		end_frame;

	void*			context;


	gpu_test_func test;
	gpu_render_model_func	render_model;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_INTF_H */