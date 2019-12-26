/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "game/game.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void jetz__construct(game_t* game);
static void jetz__destruct(game_t* game);

/*=========================================================
FUNCTIONS
=========================================================*/

void jetz__init_intf(game_intf_t* intf)
{
	clear_struct(intf);
	intf->__construct = jetz__construct;
	intf->__destruct = jetz__destruct;
}

static void jetz__construct(game_t* game)
{

}

static void jetz__destruct(game_t* game)
{

}
