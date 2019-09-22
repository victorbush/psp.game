#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Push constants
---------------------------------------------------------*/
layout(std430, push_constant) uniform PushConstants
{
	layout(offset = 80) vec3 Color;
} constants;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
//layout(location = 0) in vec3 fragNormal;
//layout(location = 1) in vec2 fragTexCoord;
//layout(location = 2) in vec3 lightDirNorm;
//layout(location = 3) in vec3 eyeDirNorm;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec4 outColor;

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
void main() {
	outColor = vec4(constants.Color.r, constants.Color.g, constants.Color.b, 1.0);
}
