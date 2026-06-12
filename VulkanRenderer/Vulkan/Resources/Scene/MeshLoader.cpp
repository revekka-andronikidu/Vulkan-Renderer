#include "MeshLoader.h"
#include <stdexcept>
#include <filesystem>

// OBJ
#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// glTF
#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE        // stb_image already compiled in Material.cpp
#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include "tiny_gltf.h"

namespace MeshLoader
{
    static std::vector<MeshData> LoadOBJ(const std::string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err, war;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &war, &err, path.c_str()))
            throw std::runtime_error(err);

        // OBJ has no embedded texture path — caller must supply it separately
        MeshData data;
        std::unordered_map<Vertex, uint32_t> uniqueVertices;

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                else {
                    vertex.normal = { 0.0f, 1.0f, 0.0f };  // fallback up vector
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(data.vertices.size());
                    data.vertices.push_back(vertex);
                }
                data.indices.push_back(uniqueVertices[vertex]);
            }
        }

        // texturePath left empty — ResourceManager fills it for OBJ
        return { std::move(data) };
    }

    static std::vector<MeshData> LoadGLTF(const std::string& path)
    {
        std::vector<MeshData> results;
        tinygltf::TinyGLTF loader;
        tinygltf::Model    model;
        std::string err, war;

        bool success = path.ends_with(".glb")
            ? loader.LoadBinaryFromFile(&model, &err, &war, path)
            : loader.LoadASCIIFromFile(&model, &err, &war, path);

        if (!success)
            throw std::runtime_error("failed to load glTF: " + err);

        // Base directory for resolving texture paths
        std::string baseDir = std::filesystem::path(path).parent_path().string() + "/";

        for (const auto& mesh : model.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                MeshData data;

                // --- positions ---
                const auto& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto& posView = model.bufferViews[posAccessor.bufferView];
                const float* positions = reinterpret_cast<const float*>(
                    model.buffers[posView.buffer].data.data()
                    + posView.byteOffset + posAccessor.byteOffset);

                // --- tex coords ---
                const float* texCoords = nullptr;
                if (primitive.attributes.count("TEXCOORD_0"))
                {
                    const auto& uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                    const auto& uvView = model.bufferViews[uvAccessor.bufferView];
                    texCoords = reinterpret_cast<const float*>(
                        model.buffers[uvView.buffer].data.data()
                        + uvView.byteOffset + uvAccessor.byteOffset);
                }

                // --- normals --- 
                const float* normals = nullptr;
                if (primitive.attributes.count("NORMAL"))
                {
                    const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
                    const auto& normalView = model.bufferViews[normalAccessor.bufferView];
                    normals = reinterpret_cast<const float*>(
                        model.buffers[normalView.buffer].data.data()
                        + normalView.byteOffset + normalAccessor.byteOffset);
                }

                // --- build vertices ---
                for (size_t i = 0; i < posAccessor.count; i++)
                {
                    Vertex v{};
                    v.pos = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };
                    v.texCoord = texCoords
                        ? glm::vec2{ texCoords[i * 2], texCoords[i * 2 + 1] }
                    : glm::vec2{ 0.0f, 0.0f };
                    v.normal = normals
                        ? glm::vec3{ normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2] }
                    : glm::vec3{ 0.0f, 1.0f, 0.0f };  // fallback
                    data.vertices.push_back(v);
                }

                // --- indices ---
                if (primitive.indices >= 0)
                {
                    const auto& idxAccessor = model.accessors[primitive.indices];
                    const auto& idxView = model.bufferViews[idxAccessor.bufferView];
                    const void* idxData = model.buffers[idxView.buffer].data.data()
                        + idxView.byteOffset + idxAccessor.byteOffset;
                    for (size_t i = 0; i < idxAccessor.count; i++)
                    {
                        uint32_t index = 0;
                        switch (idxAccessor.componentType)
                        {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            index = reinterpret_cast<const uint8_t*>(idxData)[i]; break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            index = reinterpret_cast<const uint16_t*>(idxData)[i]; break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                            index = reinterpret_cast<const uint32_t*>(idxData)[i]; break;
                        }
                        data.indices.push_back(index);
                    }
                }

                // --- resolve texture path from material ---
                if (primitive.material >= 0)
                {
                    const auto& mat = model.materials[primitive.material];
                    int texIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
                    if (texIndex >= 0)
                    {
                        int imgIndex = model.textures[texIndex].source;
                        data.texturePath = baseDir + model.images[imgIndex].uri;
                    }
                }

                results.push_back(std::move(data));
            }
        }
        return results;
    }

    std::vector<MeshData> Load(const std::string& path)
    {
        const std::string ext = std::filesystem::path(path).extension().string();

        if (ext == ".obj")                   return LoadOBJ(path);
        if (ext == ".gltf" || ext == ".glb") return LoadGLTF(path);

        throw std::runtime_error("unsupported mesh format: " + ext);
    }
}