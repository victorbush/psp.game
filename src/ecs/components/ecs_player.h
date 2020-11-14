#ifndef ECS_PLAYER_H
#define ECS_PLAYER_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_.h"
#include "ecs/components/ecs_player_.h"
#include "lua/lua_script_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs_component.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_PLAYER_NAME;

/*=========================================================
TYPES
=========================================================*/

//enum ecs_transform_properties_e
//{
//	ECS_TRANSFORM_PROPERTY_POS,
//
//	ECS_TRANSFORM_PROPERTY__COUNT
//};

/**
Player.
*/
struct ecs_player_s
{
	ecs_component_t		base;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ecs_player.public.h"

#endif /* ECS_PLAYER_H */