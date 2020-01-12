/*---------------------------------------------------------
Picker shader

Used to render objects using a single color. The color
is used as the object's id. The application can query
the picker buffer and determine what object was clicked
on screen by checking the color value of the pixel that
was clicked on.
---------------------------------------------------------*/

#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms
---------------------------------------------------------*/

/*---------------------------------------------------------
Push constants
---------------------------------------------------------*/
layout(std430, push_constant) uniform PushConstants
{
	layout(offset = 64) vec4		id_color;
} constants;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec4 outColor;

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
void main() 
{
	outColor = constants.id_color;
}
