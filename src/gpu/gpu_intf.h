#ifndef GPU_INTF_H
#define GPU_INTF_H

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_intf_struct gpu_intf_type;

typedef void (*gpu_init_func)(gpu_intf_type* gpu);
typedef void (*gpu_term_func)(gpu_intf_type* gpu);
typedef void (*gpu_begin_frame_func)(gpu_intf_type* gpu);
typedef void (*gpu_end_frame_func)(gpu_intf_type* gpu);

struct gpu_intf_struct
{
	gpu_init_func 			init;
	gpu_term_func 			term;
	gpu_begin_frame_func	begin_frame;
	gpu_end_frame_func		end_frame;

	void*			context;

};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_INTF_H */