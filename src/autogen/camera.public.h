
/**
camera__construct
*/
void camera__construct(camera_t* cam)
;

/**
camera__destruct
*/
void camera__destruct(camera_t* cam)
;

/**
Gets the current position of the camera.
*/
//void camera__get_pos(camera_t* cam, vec3_t* output)
//;

/**
Gets a view matrix for the current camera state.
*/
//void camera__get_view_matrix(camera_t* cam, mat4_t* output)
//;

/**
Moves the camera forward or backward on the current view vector.
*/
void camera__move(camera_t* cam, float move_delta)
;

/**
Pans the camera horizontally or vertically along the right and up
axes. Does not rotate the camera.
*/
void camera__pan(camera_t* cam, float vert_delta, float horz_delta)
;

void camera__rot_x(camera_t* cam, float delta_x)
;

void camera__rot_y(camera_t* cam, float delta_y)
;
