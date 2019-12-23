#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms
---------------------------------------------------------*/
layout(set = 1, binding = 0) uniform material_ubo_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} material_ubo;

layout(set = 1, binding = 1) uniform sampler2D diffuse_texture;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in vec2 in_tex_coord;
//layout(location = 1) in vec3 fragNormal;
//layout(location = 2) in vec3 lightDirNorm;
//layout(location = 3) in vec3 eyeDirNorm;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec4 outColor;

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/
void main() 
{
	vec4 diffuse = vec4(material_ubo.diffuse, 1);
	diffuse *= texture(diffuse_texture, in_tex_coord);

	outColor = diffuse;
}
