/*=========================================================
This file is automatically generated. Do not edit manually.
=========================================================*/

void _ed_undo__construct(_ed_undo_t* undo, uint32_t max_undos)
;

void _ed_undo__destruct(_ed_undo_t* undo)
;

boolean _ed_undo__can_redo(_ed_undo_t* undo)
;

boolean _ed_undo__can_undo(_ed_undo_t* undo)
;

void _ed_undo__create_bool(_ed_undo_t* undo, void* context, boolean old_value, boolean new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_float(_ed_undo_t* undo, void* context, float old_value, float new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_int32(_ed_undo_t* undo, void* context, int32_t old_value, int32_t new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_uint32(_ed_undo_t* undo, void* context, uint32_t old_value, uint32_t new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_vec2(_ed_undo_t* undo, void* context, kk_vec2_t old_value, kk_vec2_t new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_vec3(_ed_undo_t* undo, void* context, kk_vec3_t old_value, kk_vec3_t new_value, _ed_undo_func undo_func)
;

void _ed_undo__create_vec4(_ed_undo_t* undo, void* context, kk_vec4_t old_value, kk_vec4_t new_value, _ed_undo_func undo_func)
;

void _ed_undo__redo(_ed_undo_t* undo)
;

void _ed_undo__undo(_ed_undo_t* undo)
;
