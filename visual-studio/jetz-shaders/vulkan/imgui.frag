#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms - per instance
---------------------------------------------------------*/
layout(set = 0, binding = 0) uniform sampler2D sTexture;

/*---------------------------------------------------------
Inputs
---------------------------------------------------------*/
layout(location = 0) in struct {
    vec4 Color;
    vec2 UV;
} In;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out vec4 outColor;

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/

void main()
{
    outColor = In.Color * texture(sTexture, In.UV.st);
}
