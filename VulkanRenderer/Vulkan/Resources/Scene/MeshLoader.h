// MeshLoader.h
#pragma once
#include <vector>
#include <string>
#include "../Vertex.h"

namespace MeshLoader
{
    struct MeshData
    {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
        std::string           texturePath;
    };

    std::vector<MeshData> Load(const std::string& path);  // dispatches by extension
}



