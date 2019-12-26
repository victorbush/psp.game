#ifndef GAME_H
#define GAME_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct game_intf_s game_intf_t;
typedef struct game_s game_t;

typedef void (*game_construct_func)(game_t* game);
typedef void (*game_destruct_func)(game_t* game);

struct game_intf_s
{
	void*							impl;			/* Memory used by the game implementation. */

	game_construct_func				__construct;
	game_destruct_func				__destruct;
};

struct game_s
{
	game_intf_t*				intf;				/* Interface that implements game functions. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a game context.

@param gpu The game context to construct.
@param intf The game interface to use.
*/
void game__construct(game_t* game, game_intf_t* intf);

/**
Destructs a game context.
*/
void game__destruct(game_t* game);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GAME_H */