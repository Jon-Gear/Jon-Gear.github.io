#include "core/resource_manager.hpp"

using namespace bee;

void ResourceManager::Unload()
{
    for (auto it = m_resources.begin(); it != m_resources.end();)
    {
        if (it->second.use_count() == 1)
        {
            it = m_resources.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
