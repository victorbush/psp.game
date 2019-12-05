/*=========================================================
Generic utilties 
=========================================================*/

#ifndef UTL_H
#define UTL_H

/*=========================================================
INCLUDES
=========================================================*/

#include <stdlib.h>
#include <stdint.h>

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Packs 8-bit integer color components into a 32-bit RGBA color value.

@param r The red component in range [0, 255].
@param g The green component in range [0, 255].
@param b The blue component in range [0, 255].
@param a The alpha component in range [0, 255].
*/
#define utl_pack_rgba_int(r, g, b, a) \
	((r) + \
	((g) << 8) + \
	((b) << 16) + \
	((a) << 24))

/**
Packs 8-bit floating-point color components into a 32-bit RGBA color value.

@param r The red component in range [0.0f, 1.0f].
@param g The green component in range [0.0f, 1.0f].
@param b The blue component in range [0.0f, 1.0f].
@param a The alpha component in range [0.0f, 1.0f].
*/
#define utl_pack_rgba_float(r, g, b, a) \
	((int)((r) * 255.0f) + \
	((int)((g) * 255.0f) << 8) + \
	((int)((b) * 255.0f) << 16) + \
	((int)((a) * 255.0f) << 24))

#endif /* UTL_H */