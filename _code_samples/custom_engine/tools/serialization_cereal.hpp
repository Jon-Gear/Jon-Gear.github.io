#pragma once

#include "core/engine.hpp"
#include "core/fileio.hpp"
#include "core/ecs.hpp"

#include "tools/log.hpp"

#include <fstream>
#include "cereal/archives/json.hpp"

namespace bee
{

class Serializer
{
public:
    Serializer();
    ~Serializer();

    template <typename T>
    inline static bool Serialize(const T& value,
                                 const std::string nvp_name,
                                 bee::FileIO::Directory directory,
                                 const std::string& name)
    {
        auto filename = Engine.FileIO().GetPath(directory, name);
        std::ofstream ofs(filename);

        if (!ofs.is_open())
        {
            return false;
        }
        try
        {
            cereal::JSONOutputArchive archive(ofs);
            archive(cereal::make_nvp(nvp_name, value));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Serialization error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    template <typename T>
    inline static bool Deserialize(T& value,
                                   const std::string nvp_name,
                                   bee::FileIO::Directory directory,
                                   const std::string& name)
    {
        auto filename = Engine.FileIO().GetPath(directory, name);
        std::ifstream ifs(filename);
        if (!ifs.is_open())
        {
            return false;
        }
        try
        {
            cereal::JSONInputArchive archive(ifs);
            archive(cereal::make_nvp(nvp_name, value));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Deserialization error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    /// <summary>
    /// Saves the entire scene and all of its children to a given file path.
    /// This will save all entities stored in the entity register.
    /// This will save all components and the parent-child hierarchy.
    /// </summary>
    /// <param name="directory"></param>
    /// <param name="name"></param>
    /// <returns></returns>
    bool SaveScene(FileIO::Directory directory, const std::string& name);

    /// <summary>
    /// Loads a scene of entities from a given file path.
    /// The loaded entities will appear at root of scene.
    /// </summary>
    /// <param name="directory"></param>
    /// <param name="name"></param>
    /// <returns></returns>
    bool LoadScene(FileIO::Directory directory, const std::string& name);

    /// <summary>
    /// Saves a given entity and all of its children to a given file path
    /// This will save all components and the parent-child hierarchy.
    /// </summary>
    /// <param name="e"></param>
    /// <param name="directory"></param>
    /// <param name="name"></param>
    /// <returns></returns>
    bool SaveGameObject(Entity e, FileIO::Directory directory, const std::string& name);

    /// <summary>
    /// Loads a scene of entities from a given file path.
    /// The loaded entities will appear as children of a given entity.
    /// </summary>
    /// <param name="directory"></param>
    /// <param name="name"></param>
    /// <returns></returns>
    bool LoadGameObject(FileIO::Directory directory, const std::string& name, Entity e = entt::null);

private:
    /// <summary>
    /// Clears the entire scene from all entities
    /// </summary>
    void ClearScene();

    /// <summary>
    /// Registers all components used by entities.
    /// This is called ONCE upon engine initialization. Do NOT call this.
    /// Populate this function with new components for them to be used in serialization.
    /// Ensure that the new component has correct 'cereal' serialize and deserialize functions.
    /// </summary>
    void RegisterAllComponents();
};

}  // namespace bee
