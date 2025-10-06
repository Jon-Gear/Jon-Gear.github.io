#pragma once
#include "core/ecs.hpp"
#include "core/fileio.hpp"

namespace bee::scene_serialization
{
// Component that marks an entity as an instance of a difference scene.
struct InstancedScene
{
    std::string InstancePath{};
};

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
/// Saves a given entity and all of its children to a given file path
/// This will save all components and the parent-child hierarchy.
/// </summary>
/// <param name="e"></param>
/// <param name="directory"></param>
/// <param name="name"></param>
/// <returns></returns>
bool SavePrefab(Entity e, FileIO::Directory directory, const std::string& name);

/// <summary>
/// Loads a scene of entities from a given file path.
/// The loaded entities will appear at root of scene.
/// </summary>
/// <param name="directory"></param>
/// <param name="name"></param>
/// <returns></returns>
bool LoadScene(FileIO::Directory directory, const std::string& name);

/// <summary>
/// Loads a scene of entities from a given file path.
/// The loaded entities will appear as children of a given entity.
/// </summary>
/// <param name="rootEntity">The entity to be the root of the scene.</param>
/// <param name="directory"></param>
/// <param name="name"></param>
bool LoadSceneAsChild(Entity rootEntity, FileIO::Directory directory, const std::string& name);

/// <summary>
/// Registers all components used by entities.
/// This is called ONCE upon engine initialization. Do NOT call this.
/// Populate this function with new components for them to be used in serialization.
/// Ensure that the new component has correct 'cereal' serialize and deserialize functions.
/// </summary>
void RegisterAllComponents();

}  // namespace bee::scene_serialization
