#pragma once

#include <string>
#include "core/engine.hpp"
#include "core/fileio.hpp"

namespace bee
{
/// <summary>
/// Base class to all resources, gives book-keeping option to the Resources class
/// </summary>
class Resource
{
    friend class ResourceManager;

public:
    const std::string& GetPath() const { return m_path; }

    /// Resources can't be copied
    Resource(const Resource&) = delete;

protected:
    std::string m_path;
    FileIO::Directory m_directory = FileIO::Directory::None;

    static std::string GetPath(FileIO::Directory directory, const std::string& path)
    {
        return Engine.FileIO().GetPath(directory, path);
    }

    /// <summary>
    /// Protected ctor, as resources are handled by the Resource.
    /// @param type The resource type of this resource.
    /// </summary>
    Resource() = default;

    /// <summary>
    /// Protected destructor, as resources are handled by the Resource.
    /// </summary>
    virtual ~Resource() = default;
};

}  // namespace bee
