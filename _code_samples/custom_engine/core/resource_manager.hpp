#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "core/resource.hpp"
#include <cassert>

namespace bee
{

// Forward declaration
class Resource;
class EngineClass;

class ResourceManager
{
public:
    template <typename T, typename... Args>
    std::shared_ptr<T> Load(Args&&... args);

    void Unload();

    template <typename T>
    std::shared_ptr<T> Find(std::string key);

private:
    friend class bee::EngineClass;
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
};

template <typename T, typename... Args>
inline std::shared_ptr<T> ResourceManager::Load(Args&&... args)
{
    const std::string path = T::GetPath(args...);

    auto resource = Find<T>(path);
    if (resource) return resource;

    m_resources[path] = std::make_shared<T>(std::forward<Args>(args)...);
    m_resources[path]->m_path = path;

    return std::dynamic_pointer_cast<T>(m_resources[path]);
}

template <typename T>
inline std::shared_ptr<T> ResourceManager::Find(std::string key)
{
    auto it = m_resources.find(key);
    if (it != m_resources.end()) return std::dynamic_pointer_cast<T>(it->second);
    return std::shared_ptr<T>();
}

}  // namespace bee
