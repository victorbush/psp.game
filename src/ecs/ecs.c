/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/components/ecs_player.h"
#include "ecs/components/ecs_physics.h"
#include "ecs/components/ecs_static_model.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "lua/lua_script.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ecs__construct(ecs_t* ecs)
{
	memset(ecs, 0, sizeof(*ecs));
	utl_ringbuf_init(&ecs->recycled_ids_ringbuf, MAX_NUM_ENT);

	ecs_player__register(ecs);
	ecs_physics__register(ecs);
	ecs_static_model__register(ecs);
	ecs_transform__register(ecs);
}

void ecs__destruct(ecs_t* ecs)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

entity_id_t ecs__alloc_entity(ecs_t* ecs)
{
	entity_id_t id = ECS_INVALID_ID;

	/* Check recycled list */
	if (!utl_ringbuf_is_empty(&ecs->recycled_ids_ringbuf))
	{
		/* Use the first available recycled id */
		uint32_t idx = utl_ringbuf_dequeue(&ecs->recycled_ids_ringbuf);
		return ecs->recycled_ids[idx];
	}

	/* Check if buffer full */
	if (ecs->next_free_id == MAX_NUM_ENT)
	{
		return ECS_INVALID_ID;
	}

	/* Use next available id */
	id = ecs->next_free_id;
	ecs->next_free_id++;
	return id;
}

void ecs__free_entity(ecs_t* ecs, entity_id_t id)
{
	if (ecs->next_free_id - 1 == id)
	{
		/* This is the top most id, just decrease next free id */
		ecs->next_free_id--;
		return;
	}

	/* Add to recycled id list */
	uint32_t idx = utl_ringbuf_enqueue(&ecs->recycled_ids_ringbuf);
	ecs->recycled_ids[idx] = id;
}

entity_id_t ecs__iterate(ecs_t* ecs, entity_id_t* id)
{
	if (*id == ECS_INVALID_ID)
	{
		*id = 0;
	}
	else
	{
		*id = *id + 1;
	}

	return *id <= ecs->next_free_id;
}

void ecs__load_component(ecs_t* ecs, entity_id_t entity, const char* component_name, lua_script_t* lua)
{
	/* Find component */
	comp_intf_t** comp_intf = map_get(&ecs->component_registry, component_name);
	if (!comp_intf)
	{
		// TODO : Log warning
		return;
	}

	(*comp_intf)->load(ecs, entity, lua);
}

void ecs__load_entity(ecs_t* ecs, entity_id_t entity, lua_script_t* lua)
{
	char key[128];

	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get component name */
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			lua_script__cancel_loop(lua);
			break;
		}

		/* Load component for the entity */
		ecs__load_component(ecs, entity, key, lua);
	}
}

void ecs__register_component_intf(ecs_t* ecs, comp_intf_t* comp_intf)
{
	/* Check if already registered */
	comp_intf_t** existing = map_get(&ecs->component_registry, comp_intf->name);
	if (existing)
	{
		kk_log__error("Component already registered.");
		return;
	}

	/* Register */
	if (map_set(&ecs->component_registry, comp_intf->name, comp_intf))
	{
		kk_log__error("Failed to register component in ECS.");
		return;
	}
}