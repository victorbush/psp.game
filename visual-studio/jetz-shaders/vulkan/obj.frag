#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms
---------------------------------------------------------*/

// https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o

//struct material_t {
//
//}

layout(set = 1, binding = 0) uniform material_ubo_t {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	bool has_diffuse_texture;	// bool is 32-bits here
} material_ubo[10];

layout(set = 1, binding = 1) uniform sampler2D diffuse_texture[10];

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in vec2 in_tex_coord;
layout(location = 1) flat in int in_frag_material_idx;
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
	//vec4 diffuse = vec4(material_ubo[in_frag_material_idx].diffuse, 1);
	//diffuse *= texture(diffuse_texture, in_tex_coord);

//	vec4 diffuse = texture(diffuse_texture[in_frag_material_idx], in_tex_coord);

//	vec4 diffuse = vec4(material_ubo[in_frag_material_idx].diffuse, 1);
	vec4 diffuse = vec4(material_ubo[in_frag_material_idx].diffuse);
	outColor = diffuse;
}
