#version 450

layout(constant_id = 0) const uint MAX_TEXTURES = 1; 

layout(push_constant) uniform constants
{
    uint textureIndex;
} pushConstants;

layout(set = 1, binding = 0) uniform sampler sharedSampler;
layout(set = 1, binding = 1) uniform texture2D textures[MAX_TEXTURES];

layout(location = 1) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;


void main()
{
    uint idx = (pushConstants.textureIndex < MAX_TEXTURES) ? pushConstants.textureIndex : 0;
    outColor = texture(sampler2D(textures[idx], sharedSampler), fragTexCoord);
}



