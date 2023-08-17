#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    /*
    float r = texture(texSampler, fragTexCoord).r;
    float g = texture(texSampler, fragTexCoord).g;
    float b = texture(texSampler, fragTexCoord).b;
    outColor = vec4(1-r, 1-g, 1-b, 1);
    */
    
    
    outColor = texture(texSampler, fragTexCoord);
}