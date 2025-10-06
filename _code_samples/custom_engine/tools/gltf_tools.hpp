#pragma once
#include <utility>
#include <stdexcept>
#include <tinygltf/tiny_gltf.h>

#include "tools/log.hpp"

namespace bee::gltf_tools
{
template <typename T>
const T* ReadBufferGLTF(const tinygltf::Model& scene, const tinygltf::BufferView& view)
{
    const auto& buffer = scene.buffers[view.buffer];

    if (buffer.data.empty())
    {
        throw std::runtime_error("Missing data - Data not embeded in gltf");
    }

    const auto* data_start = buffer.data.data() + view.byteOffset;

    return reinterpret_cast<const T*>(data_start);
}

template <typename T>
std::pair<const T*, size_t> ReadAttributeGLTF(const tinygltf::Model& scene,
                                              const tinygltf::Primitive& primitive,
                                              const std::string attribute_name)
{
    auto accessor_index = primitive.attributes.find(attribute_name);

    if (accessor_index != primitive.attributes.end())
    {
        const auto& accessor = scene.accessors[accessor_index->second];
        const auto* address = ReadBufferGLTF<T>(scene, scene.bufferViews[accessor.bufferView]);

        return std::make_pair(address, accessor.count);
    }
    else
    {
        return std::make_pair(nullptr, 0);
    }
}

std::vector<unsigned int> ConvertIndicesToUnsignedInt(const tinygltf::Accessor& indexAccessor, const tinygltf::Model& model)
{
    std::vector<unsigned int> indices(indexAccessor.count);

    const auto& bufferView = model.bufferViews[indexAccessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    const void* data = &buffer.data[bufferView.byteOffset + indexAccessor.byteOffset];

    switch (indexAccessor.componentType)
    {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        {
            const auto* byteData = reinterpret_cast<const unsigned char*>(data);
            std::copy(byteData, byteData + indexAccessor.count, indices.begin());
            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
            const auto* shortData = reinterpret_cast<const unsigned short*>(data);
            std::copy(shortData, shortData + indexAccessor.count, indices.begin());
            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        {
            const auto* intData = reinterpret_cast<const unsigned int*>(data);
            std::copy(intData, intData + indexAccessor.count, indices.begin());
            break;
        }
        default:
            throw std::runtime_error("Unsupported index component type.");
    }

    return indices;
}

}  // namespace bee::gltf_tools
