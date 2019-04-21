/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
    int temp;

} ENG_context_type;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes an engine context.
*/
void ENG_init( ENG_context_type* context );

/**
Terminates an engine context.
*/
void ENG_term( ENG_context_type* contetx );