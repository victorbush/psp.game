
/**
Creates vertex and index buffer arrays.
*/
static void create_buffers(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Creates the descriptor set layout.
*/
static void create_descriptor_layout(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Creates a descriptor pool for this layout. Descriptor sets are allocated
from the pool.
*/
static void create_descriptor_pool(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Allocates a descriptor set for each possible concurrent frame.
*/
static void create_descriptor_sets(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Creates the font textures.
*/
static void create_font_texture(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Creates the pipeline.
*/
static void create_pipeline(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Creates the pipeline layout.
*/
static void create_pipeline_layout(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys buffers.
*/
static void destroy_buffers(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys descriptor set layout.
*/
static void destroy_descriptor_layout(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys descriptor pool.
*/
static void destroy_descriptor_pool(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys descriptor sets.
*/
static void destroy_descriptor_sets()
//##
;

/**
Destroys font texture and sampler.
*/
static void destroy_font_texture(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys the pipeline.
*/
static void destroy_pipeline(_vlk_imgui_pipeline_t* pipeline)
//##
;

/**
Destroys the pipeline layout.
*/
void destroy_pipeline_layout(_vlk_imgui_pipeline_t* pipeline)
//##
;
