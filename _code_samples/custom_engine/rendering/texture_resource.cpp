#include "rendering/texture_resource.hpp"
#include "rendering/model_resource.hpp"

#include <tinygltf/stb_image.h>  // Implementation of stb_image is in gltf_loader.cpp
#include "platform/opengl/open_gl.hpp"
#include "tools/log.hpp"
using namespace std;

bee::TextureResource::TextureResource(const ModelResource& model, int index)
{
    const auto& image = model.GetDocument().images[index];
    if (image.uri.empty())
    {
        if (image.bufferView >= 0)
        {
            GLubyte* data = nullptr;
            const auto& view = model.GetDocument().bufferViews[image.bufferView];
            const auto& buffer = model.GetDocument().buffers[view.buffer];
            const auto* ptr = &buffer.data.at(view.byteOffset);
            data = stbi_load_from_memory(ptr, (int)buffer.data.size(), &m_width, &m_height, &m_channels, 4);
            if (data)
            {
                m_texture = xsr::create_texture(m_width, m_height, data);
                stbi_image_free(data);
            }
            else
            {
                Log::Error("Error");
                // Log::Error("Image could not be loaded from a PNG file. Image:{} URI:{}", GetPath(model, index), image.uri);
            }
        }
        else if (!image.image.empty())
        {
            m_width = image.width;
            m_height = image.height;
            m_channels = image.component;
            m_texture = xsr::create_texture(m_width, m_height, (unsigned char*)image.image.data());
        }
    }
    else
    {
        auto uri = model.GetPath();
        const auto lastSlashIdx = uri.rfind("/");
        uri = uri.substr(0, lastSlashIdx + 1);
        uri += image.uri;
        // const string path = Engine.Resources().GetPath(uri);
        const auto buffer = Engine.FileIO().ReadBinaryFile(FileIO::Directory::Assets, uri);

        if (!buffer.empty())
        {
            GLubyte* data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(buffer.data()),
                                                  (int)buffer.size(),
                                                  &m_width,
                                                  &m_height,
                                                  &m_channels,
                                                  4);
            if (data)
            {
                m_texture = xsr::create_texture(m_width, m_height, data);
                stbi_image_free(data);
            }
            else
            {
                Log::Error("Error");
                // Log::Error("Image could not be loaded from a PNG file. Image:{} URI:{}", GetPath(model, index), image.uri);
            }
        }
        else
        {
            Log::Error("Error");
            // Log::Error("Image could not be loaded from a file. Image:{} URI:{}", GetPath(model, index), image.uri);
        }
    }
}

bee::TextureResource::TextureResource(FileIO::Directory directory, const std::string& path)
{
    const auto buffer = Engine.FileIO().ReadBinaryFile(directory, path);

    if (!buffer.empty())
    {
        GLubyte* data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(buffer.data()),
                                              (int)buffer.size(),
                                              &m_width,
                                              &m_height,
                                              &m_channels,
                                              4);

        if (data)
        {
            m_texture = xsr::create_texture(m_width, m_height, data);
            stbi_image_free(data);
            Log::Info("Image Loaded {}", path);
        }
        else
        {
            Log::Error("Image could not be loaded from a PNG file. Image:{}", path);
        }
    }
    else
    {
        Log::Error("Image could not be loaded from a file. Image:{}", path);
    }
}

std::string bee::TextureResource::GetPath(const ModelResource& model, int index)
{
    const auto& image = model.GetDocument().images[index];
    return model.GetPath() + " | Texture-" + to_string(index) + ": " + image.name;
}
