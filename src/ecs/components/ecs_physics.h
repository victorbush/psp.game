#ifndef ECS_PHYSICS_H
#define ECS_PHYSICS_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/ecs_.h"
#include "ecs/components/ecs_physics_.h"
#include "lua/lua_script_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs_component.h"
#include "engine/kk_math.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* ECS_PHYSICS_NAME;

/*=========================================================
TYPES
=========================================================*/

enum ecs_physics_properties_e
{
	ECS_PHYSICS_PROPERTY_MASS,
	ECS_PHYSICS_PROPERTY_INERTIA,

	ECS_PHYSICS_PROPERTY__COUNT
};

/**
Physics state data.
*/
struct ecs_physics_s
{
	ecs_component_t		base;

/* primary		*/	// kk_vec3_t		position;	<<-- stored in transform component
/* primary		*/	kk_vec3_t			momentum;
/* secondary	*/	kk_vec3_t			velocity;

/* constant		*/	float				mass;
/* constant		*/	float				inverse_mass;

/* primary		*/	// kk_vec4_t		orientation; <<-- stored in transform component
/* primary		*/	kk_vec3_t			angular_momentum;
/* secondary	*/	kk_vec4_t			spin;				// quaternion
/* secondary	*/	kk_vec3_t			angular_velocity;

/* constant		*/	float				inertia;
/* constant		*/	float				inverse_inertia;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/ecs_physics.public.h"

#endif /* ECS_PHYSICS_H */