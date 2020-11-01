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
	mat4			model_matrix;
} constants;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in int in_material_idx;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec2 frag_tex_coord;
layout(location = 1) flat out int frag_material_idx;	// Use "flat" since we don't want to interpolate
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
	frag_material_idx = in_material_idx;

	vec4 pos = vec4(in_position, 1.0); 
	vec4 worldPos = constants.model_matrix * pos;
    gl_Position = viewUbo.proj * viewUbo.view * worldPos;
}