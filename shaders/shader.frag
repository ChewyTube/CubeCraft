#version 450

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 Texcoord;

layout(binding = 1) uniform sampler2D Sampler;

void main() {
    outColor = texture(Sampler, Texcoord);
    //outColor = vec4(Texcoord, 0.0f, 1.0f);
    //outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}