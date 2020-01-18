#ifndef ED_UNDO__H
#define ED_UNDO__H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct _ed_undo_s _ed_undo_t;
typedef struct _ed_undo_cmd_s _ed_undo_cmd_t;
typedef union _ed_undo_cmd_val_u _ed_undo_cmd_val_t;
typedef enum _ed_undo_cmd_val_type_e _ed_undo_cmd_val_type_t;

typedef void (*_ed_undo_func)(void* context, _ed_undo_cmd_val_t old_value, _ed_undo_cmd_val_t new_value, _ed_undo_cmd_val_type_t type);

#endif /* ED_UNDO__H */