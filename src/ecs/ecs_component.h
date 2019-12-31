#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_component_.h"

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

/**
Base struct for all components. Must be the chunk of data in each component.
*/
struct ecs_component_s
{
	boolean 		is_used;
};

#endif /* ECS_COMPONENT_H */