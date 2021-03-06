#ifndef ED_CMD_H
#define ED_CMD_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/editor/ed_cmd_.h"
#include "app/editor/ed_undo_.h"
#include "ecs/ecs_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct _ed_cmd__set_component_property_cmd_s
{
	ecs_t*				ecs;
	entity_id_t			entity;
	comp_intf_t*		component;
	uint32_t			property_idx;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ed_cmd.internal.h"

#endif /* ED_CMD_H */