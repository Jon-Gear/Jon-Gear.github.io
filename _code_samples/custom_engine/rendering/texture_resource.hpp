#pragma once

#include "core/resource.hpp"
#include <xsr/include/xsr.hpp>

namespace bee
{
class ModelResource;

class TextureResource : public Resource
{
    friend class ResourceManager;

public:
    TextureResource(const ModelResource& model, int index);
    TextureResource(FileIO::Directory directory, const std::string& path);

    const xsr::texture_handle* GetHandleXSR() const { return &m_texture; }

private:
    static std::string GetPath(const ModelResource& model, int index);
    static std::string GetPath(FileIO::Directory directory, const std::string& path)
    {
        return Resource::GetPath(directory, path);
    }

    xsr::texture_handle m_texture;
    int m_width = -1;
    int m_height = -1;
    int m_channels = -1;
};
}  // namespace bee
