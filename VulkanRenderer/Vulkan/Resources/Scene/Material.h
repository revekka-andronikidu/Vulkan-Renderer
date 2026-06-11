#pragma once
#include "TextureArray.h"

struct Material
{
	uint32_t textureIndex = 0; // index into the shared TextureArray

    static Material Create(TextureArray& textureArray, const std::string& texturePath)
    {
        return { textureArray.AddTexture(texturePath) };
    }
};