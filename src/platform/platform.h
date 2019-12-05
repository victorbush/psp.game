#ifndef PLATFORM_H
#define PLATFORM_H

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct platform_s platform_t;


/*-------------------------------------
Platform interface
-------------------------------------*/
struct platform_s
{

		boolean			keydown__camera_forward;
		boolean			keydown__camera_backward;
		boolean			keydown__camera_strafe_left;
		boolean			keydown__camera_strafe_right;

};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* PLATFORM_H */