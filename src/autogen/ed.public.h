
void ed__construct(app_t* app)
;

void ed__destruct(app_t* app)
;

void ed__init_app_intf(app_intf_t* intf)
;

void ed__run_frame(app_t* app)
;

boolean ed__should_exit(app_t* app)
;

_ed_t* _ed__from_base(app_t* app)
;

void _ed__close_world(_ed_t* ed)
;

void _ed__open_world(_ed_t* ed, const char* world_file)
;
