
static void begin_picker_render_pass(_vlk_swapchain_t* swap, _vlk_frame_t* frame)
;

static void begin_primary_render_pass(_vlk_swapchain_t* swap, _vlk_frame_t* frame)
;

/**
Chooses best swap extent based on surface capabilties.
*/
static VkExtent2D choose_swap_extent(_vlk_swapchain_t* swap, VkExtent2D desired_extent, VkSurfaceCapabilitiesKHR *capabilities)
;

/**
Creates the swap chain and all associated resources.
*/
static void create_all(_vlk_swapchain_t* swap, VkExtent2D extent)
;

/**
create_command_buffers
*/
static void create_command_buffers(_vlk_swapchain_t* swap)
;

/**
create_depth_buffer
*/
static void create_depth_buffer(_vlk_swapchain_t* swap)
;

/**
create_framebuffers
*/
static void create_framebuffers(_vlk_swapchain_t* swap)
;

/**
create_image_views
*/
static void create_image_views(_vlk_swapchain_t* swap)
;

/**
Creates images and image views for the picker buffers. Objects are rendered
to the a picker buffer using a unique color. The buffer can then be sampled at a certain
pixel to determine what object is drawn at that pixel. It allows "picking" objects on the screen.
*/
static void create_picker_buffers(_vlk_swapchain_t* swap)
;

/**
create_semaphores
*/
static void create_semaphores(_vlk_swapchain_t* swap)
;

/**
create_swapchain
*/
static void create_swapchain(_vlk_swapchain_t* swap, VkExtent2D extent)
;

/**
Destroys the swap chain and all associated resources.
*/
static void destroy_all(_vlk_swapchain_t* swap)
;

/**
destroy_command_buffers
*/
static void destroy_command_buffers(_vlk_swapchain_t* swap)
;

/**
destroy_depth_buffer
*/
static void destroy_depth_buffer(_vlk_swapchain_t* swap)
;

/**
destroy_framebuffers
*/
static void destroy_framebuffers(_vlk_swapchain_t* swap)
;

/**
destroy_image_views
*/
static void destroy_image_views(_vlk_swapchain_t* swap)
;

/**
Destroys the picker buffer images and image views.
*/
static void destroy_picker_buffers(_vlk_swapchain_t* swap)
;

/**
destroy_semaphores
*/
static void destroy_semaphores(_vlk_swapchain_t* swap) 
;

/**
destroy_swapchain
*/
static void destroy_swapchain(_vlk_swapchain_t* swap)
;

/**
Recreates the swap chain and associated resources.
*/
static void resize(_vlk_swapchain_t* swap, VkExtent2D extent)
;
