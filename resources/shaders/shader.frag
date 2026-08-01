#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform constants
{
    uint textureIndex;
} pushConstants;

layout(set = 1, binding = 0) uniform sampler sharedSampler;
layout(set = 1, binding = 1) uniform texture2D textures[];

layout(location = 0) in vec3 fragNormal; 
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main()
{
    uint idx = nonuniformEXT(pushConstants.textureIndex);
    outColor = texture(sampler2D(textures[idx], sharedSampler), fragTexCoord);
}



