#include "tools/serialization_cereal.hpp"

#include "core/engine.hpp"

#include "core/fileio.hpp"

#include "core/transform.hpp"
#include "rendering/render_xsr_components.hpp"
#include "tile_editor/tile_components.hpp"
#include "particles/particle_components.hpp"

#include "tools/log.hpp"

#include <fstream>

#include <queue>

#include <entt/entt.hpp>
#include <tools/inspector.hpp>

using namespace bee;
using namespace entt::literals;

namespace bee::internal
{
struct SceneNode
{
    int childrenCount = 0;
};

template <class Archive>
void serialize(Archive& ar, SceneNode& p)
{
    ar(cereal::make_nvp("Children Count", p.childrenCount));
}

void CreateSceneFromFile(cereal::JSONInputArchive& archive, const Entity parentEntity = entt::null);

void StepThroughHierarchy(Entity e, cereal::JSONOutputArchive& archive, std::uint16_t& counter);

template <typename Type>
void RegisterComponentForSerialization();

template <typename Type>
void SaveType(cereal::JSONOutputArchive& archive, const Entity entity, const std::string& variableName);

template <typename Type>
void LoadType(cereal::JSONInputArchive& archive, const Entity entity, const std::string& variableName);

}  // namespace bee::internal

bee::Serializer::Serializer()
{
    RegisterAllComponents();
    Log::Info("Registered All Components");
}

bee::Serializer::~Serializer() {}

bool bee::Serializer::SaveScene(FileIO::Directory directory, const std::string& name)
{
    auto filename = Engine.FileIO().GetPath(directory, name);
    std::ofstream ofs(filename);

    if (!ofs.is_open())
    {
        bee::Log::Warn("Cannot save scene. File not found");
        return false;
    }

    try
    {
        cereal::JSONOutputArchive archive(ofs);

        uint16_t counter = 0;
        // Since we operate from the root, we simply set 'multiple' starting points.
        // From the registry, we find every node which has no parent (i.e. at root) and begin stepping.
        for (auto [e, transform] : Engine.ECS().Registry.view<Transform>(entt::exclude<HideTransform>).each())
        {
            if (!transform.HasParent())
            {
                internal::StepThroughHierarchy(e, archive, counter);
            }
        }

        archive(cereal::make_nvp("NodeCount", counter));
    }
    catch (const std::exception& e)
    {
        bee::Log::Error("Serialization error: {}", e.what());
        return false;
    }
    bee::Log::Info("Saving scene: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

bool bee::Serializer::LoadScene(FileIO::Directory directory, const std::string& name)
{
    ClearScene();
    auto filename = Engine.FileIO().GetPath(directory, name);
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        bee::Log::Warn("Cannot load scene. File not found");
        return false;
    }
    try
    {
        cereal::JSONInputArchive archive(ifs);
        internal::CreateSceneFromFile(archive);
    }
    catch (const std::exception& e)
    {
        bee::Log::Error("Deserialization error: {}", e.what());
        return false;
    }
    bee::Log::Info("Loading scene: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

bool bee::Serializer::SaveGameObject(Entity e, FileIO::Directory directory, const std::string& name)
{
    if (!Engine.ECS().Registry.valid(e))
    {
        Log::Warn("Cannot save sub-scene. No entity selected");
        return false;
    }

    auto filename = Engine.FileIO().GetPath(directory, name);
    std::ofstream ofs(filename);

    if (!ofs.is_open())
    {
        bee::Log::Warn("Cannot save sub-scene. File not found");
        return false;
    }

    try
    {
        cereal::JSONOutputArchive archive(ofs);
        uint16_t counter = 0;

        // We begin stepping through the entity to record it and reach all of its children.
        internal::StepThroughHierarchy(e, archive, counter);

        archive(cereal::make_nvp("NodeCount", counter));
    }
    catch (const std::exception& e)
    {
        bee::Log::Error("Serialization error: {}", e.what());
        return false;
    }
    bee::Log::Info("Saving sub-scene: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

bool bee::Serializer::LoadGameObject(FileIO::Directory directory, const std::string& name, Entity e)
{
    auto filename = Engine.FileIO().GetPath(directory, name);
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        bee::Log::Warn("Cannot load scene as child. File not found");
        return false;
    }
    try
    {
        cereal::JSONInputArchive archive(ifs);
        if (Engine.ECS().Registry.valid(e))
        {
            internal::CreateSceneFromFile(archive, e);
        }
        else
        {
            internal::CreateSceneFromFile(archive);
        }
    }
    catch (const std::exception& e)
    {
        bee::Log::Error("Deserialization error: {}", e.what());
        return false;
    }
    bee::Log::Info("Loading scene as child: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

void bee::Serializer::ClearScene()
{
    for (auto [e] : Engine.ECS().Registry.view<entt::entity>(entt::exclude<Camera, LightXSR>).each())
    {
        Engine.ECS().DeleteEntity(e);
    }
}

void bee::Serializer::RegisterAllComponents()
{
    // Register all of the components here! Make sure they have proper 'cereal' serialize and deserialize functions.

    internal::RegisterComponentForSerialization<Transform>();
    internal::RegisterComponentForSerialization<Grid>();
    internal::RegisterComponentForSerialization<Tile>();
    internal::RegisterComponentForSerialization<ParticleEmitter>();
    internal::RegisterComponentForSerialization<ConeShape>();
}

void internal::CreateSceneFromFile(cereal::JSONInputArchive& archive, Entity parentEntity)
{
    int nodeCount = 0;
    archive(cereal::make_nvp("NodeCount", nodeCount));

    // This helps us keep track on to whom we should parent the node to.
    std::deque<std::pair<entt::entity, int>> currentParent;

    if (parentEntity != entt::null)
    {
        currentParent.push_back(std::make_pair(parentEntity, -1));
    }

    auto& ecs{Engine.ECS()};

    // Since we have a set node count, we can simply iterate over all of them in a for-loop.
    for (int i = 1; i <= nodeCount; i++)
    {
        SceneNode node;
        // We get the node data based on the index it was saved with.
        archive(cereal::make_nvp(std::to_string(i), node));

        auto childEntity = ecs.CreateEntity();

        // Add all of the components
        for (auto&& [id, type] : entt::resolve())
        {
            entt::meta_func loadingFunction = type.func("LoadType"_hs);
            if (!loadingFunction)
            {
                continue;
            }

            loadingFunction.invoke({},
                                   entt::forward_as_meta<cereal::JSONInputArchive&>(archive),  // NOLINT
                                   entt::forward_as_meta<bee::Entity&>(childEntity),           // NOLINT
                                   entt::forward_as_meta<std::string>(std::to_string(i)));     // NOLINT
        }

        if (!currentParent.empty())
        {
            auto& childTransform = Engine.ECS().Registry.get<Transform>(childEntity);
            childTransform.SetParent(currentParent.back().first);
            currentParent.back().second--;

            if (currentParent.back().second == 0)
            {
                currentParent.pop_back();
            }
        }

        if (node.childrenCount != 0)
        {
            currentParent.push_back(std::make_pair(childEntity, node.childrenCount));
        }
    }
}

void internal::StepThroughHierarchy(Entity childEntity, cereal::JSONOutputArchive& archive, std::uint16_t& counter)
{
    counter += 1;

    auto& childTransform = Engine.ECS().Registry.get<Transform>(childEntity);
    SceneNode node;

    // We count the amount of children the transform has.
    // The reason why we don't count the children when stepping through is
    // because of the order the nodes are saved in to keep the hierarchy readable.
    for ([[maybe_unused]] auto _ : childTransform)
    {
        node.childrenCount++;
    }

    archive(cereal::make_nvp(std::to_string(counter), node));

    for (auto&& [id, type] : entt::resolve())
    {
        auto* storage = Engine.ECS().Registry.storage(id);

        if (!storage || !storage->contains(childEntity))
        {
            continue;
        }

        entt::meta_func saveFunction = type.func("SaveType"_hs);

        if (!saveFunction)
        {
            continue;
        }

        saveFunction.invoke({},
                            entt::forward_as_meta<cereal::JSONOutputArchive&>(archive),    // NOLINT
                            entt::forward_as_meta<bee::Entity&>(childEntity),              // NOLINT
                            entt::forward_as_meta<std::string>(std::to_string(counter)));  // NOLINT
    }

    if (childTransform.HasChildren())
    {
        for (auto childChildEntity : childTransform)
        {
            internal::StepThroughHierarchy(childChildEntity, archive, counter);
        }
    }
}

template <typename Type>
[[maybe_unused]] void internal::RegisterComponentForSerialization()
{
    entt::meta_factory<Type>()
        .type(entt::type_hash<Type>::value())
        .template func<&internal::SaveType<Type>>("SaveType"_hs)
        .template func<&internal::LoadType<Type>>("LoadType"_hs);
    //.template func<&Type::Load>("Load"_hs)
    //.template func<&Type::Save>("Save"_hs);
}

template <typename Type>
void internal::SaveType(cereal::JSONOutputArchive& archive, const Entity entity, const std::string& variableName)
{
    Type& component = Engine.ECS().Registry.get<Type>(entity);

    component.Save(archive, entity, variableName);
}

template <typename Type>
void internal::LoadType(cereal::JSONInputArchive& archive, const Entity entity, const std::string& variableName)
{
    Type& component = Engine.ECS().CreateComponent<Type>(entity);

    try
    {
        component.Load(archive, entity, variableName);
    }
    catch (...)
    {
        Engine.ECS().Registry.remove<Type>(entity);
    }
}
