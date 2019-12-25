#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms - per view
---------------------------------------------------------*/
layout(set = 0, binding = 0) uniform PerViewUBO {
    mat4 view;
    mat4 proj;
	vec3 cameraPos;
} viewUbo;

/*---------------------------------------------------------
Push constants
---------------------------------------------------------*/
layout(std430, push_constant) uniform PushConstants
{
	mat4			ModelMatrix;
	vec2			Anchor;
	float			Height;
	float			Width;
} constants;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex_coord;
//layout(location = 2) in vec3 inNormal;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec2 frag_tex_coord;
//layout(location = 1) out vec3 fragNormal;
//layout(location = 2) out vec3 lightDirNorm;
//layout(location = 3) out vec3 eyeDirNorm;

out gl_PerVertex {
    vec4 gl_Position;
};

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
void main() 
{
	frag_tex_coord = in_tex_coord;

	/*
	Default orientation for plane is to be flat on the ground.
	Meaning the y coord is always 0 and the z coord is used for height.
	*/

	/* Adjust for anchor */
	vec4 pos = vec4(in_position, 1.0); 
	pos.x -= constants.Anchor.x;
	pos.z += constants.Anchor.y;

	/* Adjust for dimensions */
	pos.x *= constants.Width;
	pos.z *= constants.Height;

	/* Calc final position */
	vec4 worldPos = constants.ModelMatrix * pos;
    gl_Position = viewUbo.proj * viewUbo.view * worldPos;
}