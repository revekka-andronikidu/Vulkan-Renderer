#pragma once
#include "TextureArray.h"

class Material
{
public:
    Material(TextureArray& textureArray, const std::string& texturePath)
        : m_TextureIndex(textureArray.AddTexture(texturePath))
    {
    }

    uint32_t GetTextureIndex() const { return m_TextureIndex; }

private:
    uint32_t m_TextureIndex;  // index into the shared TextureArray
};

