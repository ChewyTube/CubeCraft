#version 450

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 Texcoord;

layout(set = 1, binding = 0) uniform sampler2D Sampler;

void main() {
    outColor = texture(Sampler, Texcoord);
}