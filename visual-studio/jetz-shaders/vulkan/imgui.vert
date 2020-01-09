#version 450
#extension GL_ARB_separate_shader_objects : enable

/*---------------------------------------------------------
Uniforms
---------------------------------------------------------*/
layout(push_constant) uniform uPushConstant {
    vec2 uScale;
    vec2 uTranslate;
} pc;

/*---------------------------------------------------------
Vertex input attributes
---------------------------------------------------------*/
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;

/*---------------------------------------------------------
Outputs
---------------------------------------------------------*/
layout(location = 0) out struct {
    vec4 Color;
    vec2 UV;
} Out;

out gl_PerVertex {
    vec4 gl_Position;
};

/*---------------------------------------------------------
Functions
---------------------------------------------------------*/

void main()
{
    Out.Color = inColor;
    Out.UV = inTexCoord;
    gl_Position = vec4(inPos * pc.uScale + pc.uTranslate, 0, 1);
}