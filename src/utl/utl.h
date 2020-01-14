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
Calculates the difference between two timestamps. Handles when the time
wraps around the max value. It's assumed that timestamps are uint32_t.

@param new_time The new timestamp.
@param old_time The old timestamp.
*/
#define utl_calc_time_diff(new_time, old_time) \
	( new_time ) > ( old_time ) ? \
	( new_time ) - ( old_time ) : \
	UINT32_MAX - ( old_time ) + ( new_time )

/**
Packs 8-bit integer color components in range [0, 255] into a 32-bit RGBA color value.

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
Packs 8-bit floating-point color components in range [0.0f, 1.0f] into a 32-bit RGBA color value.

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

/**
Unpacks packed 32-bit RGBA color into 8-bit floating-point color components in range [0.0f, 1.0f].

@param in_rgba The packed RGBA color integer to unpack.
@param out_vec4 The vec4_t color destination to place the unpacked color components.
*/
#define utl_unpack_rgba_float(in_rgba, out_vec4) \
	out_vec4.x = ((in_rgba) & 0xFF) / 255.0f; \
	out_vec4.y = (((in_rgba) & 0xFF00) >> 8) / 255.0f; \
	out_vec4.z = (((in_rgba) & 0xFF0000) >> 16) / 255.0f; \
	out_vec4.w = (((in_rgba) & 0xFF000000) >> 24) / 255.0f

/**
Unpacks packed 32-bit RGBA color into 8-bit integer color components in range [0, 255].

@param in_rgba The packed RGBA color integer to unpack.
@param out_vec4 The vec4_t color destination to place the unpacked color components.
*/
#define utl_unpack_rgba_int(in_rgba, out_vec4_i) \
	out_vec4_i.x = ((in_rgba) & 0xFF); \
	out_vec4_i.y = (((in_rgba) & 0xFF00) >> 8); \
	out_vec4_i.z = (((in_rgba) & 0xFF0000) >> 16); \
	out_vec4_i.w = (((in_rgba) & 0xFF000000) >> 24)

#endif /* UTL_H */