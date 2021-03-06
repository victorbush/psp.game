/*=========================================================
This file is automatically generated. Do not edit manually.
=========================================================*/

/**
kk_camera__construct
*/
void kk_camera__construct(kk_camera_t* cam)
;

/**
kk_camera__destruct
*/
void kk_camera__destruct(kk_camera_t* cam)
;

/**
Gets the current position of the camera.
*/
//void kk_camera__get_pos(kk_camera_t* cam, vec3_t* output)
//;

/**
Gets a view matrix for the current camera state.
*/
//void kk_camera__get_view_matrix(kk_camera_t* cam, mat4_t* output)
//;

/**
Moves the camera forward or backward on the current view vector.
*/
void kk_camera__move(kk_camera_t* cam, float move_delta)
;

/**
Pans the camera horizontally or vertically along the right and up
axes. Does not rotate the camera.
*/
void kk_camera__pan(kk_camera_t* cam, float vert_delta, float horz_delta)
;

void kk_camera__rot_x(kk_camera_t* cam, float delta_x)
;

void kk_camera__rot_y(kk_camera_t* cam, float delta_y)
;
