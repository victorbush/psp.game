#version 450
#extension GL_ARB_separate_shader_objects : enable

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
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
