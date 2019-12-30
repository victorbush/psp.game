#ifndef ECS_H
#define ECS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "lua/lua_script.h"
#include "thirdparty/rxi_map/src/map.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ECS_INVALID_ID 0xFFFFFFFF
#define MAX_COMPONENT_NAME 64
#define MAX_NUM_ENT 500

/*=========================================================
TYPES
=========================================================*/

typedef uint32_t entity_id_t;
typedef struct ecs_s ecs_t;

/*-------------------------------------
Component interface
-------------------------------------*/

typedef void (*comp_intf_load_func)(ecs_t* ecs, entity_id_t entity, lua_script_t* lua);

typedef struct comp_intf_s comp_intf_t;
struct comp_intf_s
{
	char						name[MAX_COMPONENT_NAME];		/* Component name. */

	comp_intf_load_func			load;			/* Loads a component instance from the specified lua script. */
};

/*-------------------------------------
Entity component system
-------------------------------------*/

struct ecs_s
{
	static_model_comp_t		static_model_comp[MAX_NUM_ENT];
	//anim_model_comp			anim_model_comp[MAX_NUM_ENT];
	transform_comp_t		transform_comp[MAX_NUM_ENT];

	entity_id_t				recycled_ids[MAX_NUM_ENT];
	utl_ringbuf_t			recycled_ids_ringbuf;

	entity_id_t				next_free_id;


	map_t(comp_intf_t*)		component_registry;		/* Registry of known component types and their interfaces. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes an ecs context.
@param ecs The context to init.
*/
void ecs__construct(ecs_t* ecs);

/**
Destructs an ecs context.
@param ecs The context to destruct.
*/
void ecs__destruct(ecs_t* ecs);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Allocates an entity id.
@param ecs The ECS context.
@return The allocated entity id. If none availabe, ECS_INVALID_ID.
*/
entity_id_t ecs__alloc_entity(ecs_t* ecs);

/**
Frees an entity id.
@param ecs The ECS context.
@param id The id to free.
*/
void ecs__free_entity(ecs_t* ecs, entity_id_t id);

void ecs__load_component(ecs_t* ecs, entity_id_t entity, const char* component_name, lua_script_t* lua);

void ecs__load_entity(ecs_t* ecs, entity_id_t entity, lua_script_t* lua);

void ecs__register_component_intf(ecs_t* ecs, comp_intf_t* comp_intf);

#endif /* ECS_H */