
/**
Constructs a static model.

@param model The model to construct.
@param gpu The GPU context.
@param filename The model file to load.
*/
void gpu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const char* filename)
;

/**
Destructs a static model.

@param model The model to destruct.
@param gpu The GPU context.
*/
void gpu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu)
;

void gpu_static_model__render
	(
	gpu_static_model_t*		model, 
	gpu_t*					gpu, 
	gpu_window_t*			window, 
	gpu_frame_t*			frame, 
	gpu_material_t*			material, 
	ecs_transform_t*		transform
	)
;
