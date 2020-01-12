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

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
out gl_PerVertex {
    vec4 gl_Position;
};

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
void main() 
{
	vec4 pos = vec4(in_position, 1.0); 
	vec4 worldPos = constants.model_matrix * pos;
    gl_Position = viewUbo.proj * viewUbo.view * worldPos;
}