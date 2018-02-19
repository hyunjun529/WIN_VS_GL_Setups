#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;

out vec4 ex_Color;

uniform mat4 WVP;

void main(void)
{
    gl_Position = WVP * in_Position;
    ex_Color = in_Color;
}