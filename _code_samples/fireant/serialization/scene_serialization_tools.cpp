#include "tools/scene_serialization_tools.hpp"

#include "core/engine.hpp"
#include "core/fileio.hpp"
#include "core/transform.hpp"
#include "rendering/camera_system.hpp"
#include "tools/editor_system.hpp"

#include "rendering/render_components.hpp"
#include "rendering/particle_system_components.hpp"

#include "terrain/terrain_components.hpp"

#include "tools/asset_manager/scene_file.hpp"
#include "tools/inspector.hpp"
#include "tools/log.hpp"

#include <cereal/archives/json.hpp>

#include <entt/entt.hpp>

#include <fstream>

#include "camera/camera_controller.hpp"
#include "ui/ui_element.hpp"
#include "ui/text_renderer.hpp"
#include "ui/button_element.hpp"

using namespace bee;
using namespace entt::literals;

namespace bee::internal
{

void CreateSceneFromFile(cereal::JSONInputArchive& archive,
                         const Entity parentEntity = entt::null,
                         bool isInstancedScene = false);

bool StepThroughHierarchy(Entity entity,
                          cereal::JSONOutputArchive& archive,
                          SceneFileData& sceneData,
                          std::vector<UID>& visitedSet,
                          const std::string& parentKey = "");

bool RecursiveInvestigation(const std::string& path, std::vector<UID>& visitedSet);

bool Investigate(cereal::JSONInputArchive& archive, std::vector<UID>& visitedSet);

bool RecursionCheck(const std::string& path, std::vector<UID>& visitedSet);

size_t HashFilepath(const std::string& filepath);

template <typename Type>
void RegisterComponentForSerialization();

template <typename Type>
void SaveType(cereal::JSONOutputArchive& archive, Entity entity);

template <typename Type>
void LoadType(cereal::JSONInputArchive& archive, Entity entity);

}  // namespace bee::internal

void bee::scene_serialization::RegisterAllComponents()
{
    // Register all of the components here! Make sure they have proper 'cereal' serialize and deserialize functions.
    internal::RegisterComponentForSerialization<Transform>();
    internal::RegisterComponentForSerialization<Light>();
    internal::RegisterComponentForSerialization<particle_components::Emitter>();
    internal::RegisterComponentForSerialization<particle_components::Shape>();
    internal::RegisterComponentForSerialization<Terrain>();
    internal::RegisterComponentForSerialization<MeshRenderer>();
    internal::RegisterComponentForSerialization<CameraComponent>();
    internal::RegisterComponentForSerialization<CameraControl>();
    internal::RegisterComponentForSerialization<frustum_culling::Frustum>();
    internal::RegisterComponentForSerialization<GameCollider>();

    internal::RegisterComponentForSerialization<UIElement>();
    internal::RegisterComponentForSerialization<TextElement>();
    internal::RegisterComponentForSerialization<ButtonElement>();

    /*
    Components left:
    - Terrain mesh renderer
    - Billboard sprite
    - Camera?
    - Mesh Renderer
    */
}

[[maybe_unused]] bool scene_serialization::SaveScene(const FileIO::Directory directory, const std::string& name)
{
    // Write to a temporary source first. If the save fails, the current file would be lost.
    // Once the save is confirmed successful, we'll copy the temp file to the real one.
    std::string temp{"TEMP_"};
    temp.append(std::to_string(internal::HashFilepath(name)).substr(0, 4));

    auto tempFilename{Engine.FileIO().GetPath(directory, temp)};
    std::ofstream tempOutput(tempFilename);

    if (!tempOutput.is_open())
    {
        Log::Warn("Cannot save scene. Error opening file.");
        return false;
    }

    try
    {
        internal::SceneFileData sceneData{};
        sceneData.m_metadata.m_filepath = name;
        sceneData.m_metadata.m_uid = std::hash<std::string>()(name);

        // Add parent scene info to visited set.
        // If an item cannot be added to the set, recursion is happening.
        // Abort the saving process and notify user.
        std::vector<UID> visitedSet;
        visitedSet.push_back(sceneData.m_metadata.m_uid);

        cereal::JSONOutputArchive archive(tempOutput);

        //  Since we operate from the root, we simply find every node which has no parent (i.e. at root) and begin stepping.
        for (const auto [entity, transform] : Engine.ECS().Registry.view<const Transform>(entt::exclude<HideTransform>).each())
        {
            if (!transform.HasParent())
            {
                if (!StepThroughHierarchy(entity, archive, sceneData, visitedSet))
                {
                    tempOutput.close();
                    std::remove(tempFilename.c_str());

                    return false;
                }
            }
        }

        //  Since we operate from the root, we simply find every node which has no parent (i.e. at root) and begin stepping.
        for (const auto [entity, uiElement] : Engine.ECS().Registry.view<const UIElement>(entt::exclude<HideTransform>).each())
        {
            if (!uiElement.HasParent())
            {
                if (!StepThroughHierarchy(entity, archive, sceneData, visitedSet))
                {
                    tempOutput.close();
                    std::remove(tempFilename.c_str());

                    return false;
                }
            }
        }

        sceneData.save(archive);
    }
    catch (const std::exception& e)
    {
        Log::Error("Serialization error: {}", e.what());
        return false;
    }

    // Save temp to real file.
    tempOutput.close();

    auto realFilename{Engine.FileIO().GetPath(directory, name)};
    std::ofstream realOutput(realFilename);

    if (!realOutput.is_open())
    {
        Log::Warn("Cannot save scene. Error opening file.");
        return false;
    }

    // Copy contents.
    std::ifstream tempInput(tempFilename);
    realOutput << tempInput.rdbuf();

    Log::Info("Saving scene: {}", realFilename);

    realOutput.close();
    tempInput.close();

    // Erase temporary.
    std::remove(tempFilename.c_str());

    return true;
}

[[maybe_unused]] bool scene_serialization::SavePrefab(Entity entity, FileIO::Directory directory, const std::string& name)
{
    if (!Engine.ECS().Registry.valid(entity))
    {
        Log::Warn("Cannot save sub-scene. No entity selected");
        return false;
    }

    auto filename{Engine.FileIO().GetPath(directory, name)};
    std::ofstream ofs(filename);

    if (!ofs.is_open())
    {
        Log::Warn("Cannot save sub-scene. File not found");
        return false;
    }

    try
    {
        internal::SceneFileData sceneData{};
        sceneData.m_metadata.m_filepath = filename;
        sceneData.m_metadata.m_uid = std::hash<std::string>()(name);

        // Add parent scene info to visited set.
        // If an item cannot be added to the set, recursion is happening.
        // Abort the saving process and notify user.
        std::vector<UID> visitedSet;
        visitedSet.push_back(sceneData.m_metadata.m_uid);

        cereal::JSONOutputArchive archive(ofs);

        if (!StepThroughHierarchy(entity, archive, sceneData, visitedSet))
        {
            return false;
        }

        sceneData.save(archive);
    }
    catch (const std::exception& e)
    {
        Log::Error("Serialization error: {}", e.what());
        return false;
    }

    Log::Info("Saving sub-scene: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

[[maybe_unused]] bool scene_serialization::LoadScene(FileIO::Directory directory, const std::string& name)
{
    auto filename = Engine.FileIO().GetPath(directory, name);
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        Log::Warn("Cannot load scene. File not found");
        return false;
    }
    try
    {
        cereal::JSONInputArchive archive(ifs);
        internal::CreateSceneFromFile(archive);
    }
    catch (const std::exception& e)
    {
        Log::Error("Deserialization error: {}", e.what());
        return false;
    }
    Log::Info("Loading scene: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

[[maybe_unused]] bool scene_serialization::LoadSceneAsChild(const Entity rootEntity,
                                                            const FileIO::Directory directory,
                                                            const std::string& name)
{
    if (!Engine.ECS().Registry.valid(rootEntity))
    {
        Log::Warn("Cannot load scene as child. No parent entity selected");
        return false;
    }

    const auto filename{Engine.FileIO().GetPath(directory, name)};
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        Log::Warn("Cannot load scene as child. File not found");
        return false;
    }

    try
    {
        cereal::JSONInputArchive archive(ifs);
        internal::CreateSceneFromFile(archive, rootEntity, true);
    }
    catch (const std::exception& e)
    {
        bee::Log::Error("Deserialization error: {}", e.what());
        return false;
    }
    bee::Log::Info("Loading scene as child: {}", Engine.FileIO().GetPath(directory, name));
    return true;
}

void internal::CreateSceneFromFile(cereal::JSONInputArchive& archive, Entity rootParent, bool isInstancedScene)
{
    // Populate scene data from file.
    SceneFileData sceneData{};
    sceneData.load(archive);

    // Use data to reconstruct local entities.
    AllEntityStructures& eStructs{sceneData.m_entities};
    for (auto& eStruct : eStructs.m_data)
    {
        eStruct.m_entity = Engine.ECS().CreateEntity();

        for (auto&& [id, type] : entt::resolve())
        {
            eStruct.m_data.emplace_back(type.func("LoadType"_hs));
        }
    }

    // Now that the entity structure data has the loading functions it needs,
    // call the deserialize method directly.
    archive(cereal::make_nvp("DATA", eStructs));

    // Associate each reference id with the filepath.
    using RefKey = std::string;
    using RefPath = std::string;
    std::unordered_map<RefKey, RefPath> refPaths{};
    AllSceneReferences& eRefs{sceneData.m_references};
    for (auto& eRef : eRefs.m_data)
    {
        refPaths[eRef.m_key] = eRef.m_filepath;
    }

    // Find all entities that have an instance id and load the reference scene as a child.
    for (auto& eStruct : eStructs.m_data)
    {
        if (auto it{refPaths.find(eStruct.m_referenceKey)}; it != refPaths.end())
        {
            // Mark as instanced scene to hide its children.
            auto& iScene{Engine.ECS().CreateComponent<scene_serialization::InstancedScene>(eStruct.m_entity)};

            // Reassign path to instance so when saving it can be used to set the reference path.
            iScene.InstancePath = it->second;

            // Add instanced scene as child(ren).
            scene_serialization::LoadSceneAsChild(eStruct.m_entity, FileIO::Directory::Assets, it->second);
        }
    }

    // Build parentage and set names.
    for (auto& eStruct : eStructs.m_data)
    {
        if (Engine.ECS().Registry.any_of<Transform>(eStruct.m_entity))
        {
            auto& transform{Engine.ECS().Registry.get<Transform>(eStruct.m_entity)};
            transform.Name = eStruct.m_name;

            // Parentage.
            if (eStruct.m_parentKey.empty())
            {
                transform.SetParent(rootParent);
            }
            else
            {
                for (auto& otherEStruct : eStructs.m_data)
                {
                    if (otherEStruct.m_key == eStruct.m_parentKey)
                    {
                        transform.SetParent(otherEStruct.m_entity);
                    }
                }
            }
        }
        else if (Engine.ECS().Registry.any_of<UIElement>(eStruct.m_entity))
        {
            auto& uiElement{Engine.ECS().Registry.get<UIElement>(eStruct.m_entity)};
            uiElement.Name = eStruct.m_name;

            // Parentage.
            if (eStruct.m_parentKey.empty())
            {
                uiElement.SetParent(rootParent);
            }
            else
            {
                for (auto& otherEStruct : eStructs.m_data)
                {
                    if (otherEStruct.m_key == eStruct.m_parentKey)
                    {
                        uiElement.SetParent(otherEStruct.m_entity);
                    }
                }
            }
        }
    }

    // For instanced scenes, hide all entities.
    if (isInstancedScene)
    {
        for (const auto& eStruct : eStructs.m_data)
        {
            Engine.ECS().CreateComponent<HideTransform>(eStruct.m_entity);
        }
    }
}

bool internal::StepThroughHierarchy(const Entity entity,
                                    cereal::JSONOutputArchive& archive,
                                    SceneFileData& sceneData,
                                    std::vector<UID>& visitedSet,
                                    const std::string& parentKey)
{
    EntityStructure eStruct{};

    // If this is an instanced entity / scene, save its reference.
    auto* iScene{Engine.ECS().Registry.try_get<scene_serialization::InstancedScene>(entity)};
    if (iScene)
    {
        auto& path{iScene->InstancePath};

        // Check for recursion.
        if (RecursionCheck(path, visitedSet) || RecursiveInvestigation(path, visitedSet))
        {
            return false;
        }

        // Check that a scene with same path was not already saved (in a different iteration).
        std::string referenceKey{};
        bool wasAlreadySaved{false};
        for (auto& reference : sceneData.m_references.m_data)
        {
            if (reference.m_filepath == path)
            {
                wasAlreadySaved = true;
                referenceKey = reference.m_key;
                break;
            }
        }

        // Save the instance scene reference.
        if (!wasAlreadySaved)
        {
            SceneReference sRef{};

            const auto startPos{path.find_last_of('/')};
            const auto endPos{path.find_last_of('.')};
            auto name{path.substr(startPos + 1, (endPos - startPos - 1))};

            sRef.CreateKey(entity);
            sRef.m_filepath = path;
            sRef.m_uid = 0;  // TODO: use this.
            sceneData.m_references.m_data.push_back(sRef);

            referenceKey = sRef.m_key;
        }

        // Set the instance scene
        eStruct.m_referenceKey = referenceKey;
    }

    // Gather all component types that exist for this entity.
    for (auto&& [id, type] : entt::resolve())
    {
        auto* storage = Engine.ECS().Registry.storage(id);

        if (!storage || !storage->contains(entity))
        {
            continue;
        }

        entt::meta_func saveFunction = type.func("SaveType"_hs);
        if (!saveFunction)
        {
            continue;
        }

        eStruct.m_data.emplace_back(saveFunction);
    }

    // Serialize all components within the same key.
    if (!eStruct.m_data.empty())
    {
        if (Engine.ECS().Registry.any_of<Transform>(entity))
        {
            auto& childTransform{Engine.ECS().Registry.get<Transform>(entity)};

            // Add additional data.
            eStruct.m_parentKey = parentKey;
            eStruct.CreateKey(childTransform.Name, entity);
        }
        else if (Engine.ECS().Registry.any_of<UIElement>(entity))
        {
            auto& uiElement{Engine.ECS().Registry.get<UIElement>(entity)};

            // Add additional data.
            eStruct.m_parentKey = parentKey;
            eStruct.CreateKey(uiElement.Name, entity);
        }

        // Add to list of entities to serialize.
        sceneData.m_entities.m_data.emplace_back(eStruct);
    }

    // Recursively add any children of this entity.
    if (Engine.ECS().Registry.any_of<Transform>(entity))
    {
        auto& childTransform{Engine.ECS().Registry.get<Transform>(entity)};
        if (childTransform.HasChildren())
        {
            for (const auto childEntity : childTransform)
            {
                if (!Engine.ECS().Registry.any_of<HideTransform>(childEntity))
                {
                    if (!StepThroughHierarchy(childEntity, archive, sceneData, visitedSet, eStruct.m_key))
                    {
                        return false;
                    }
                }
            }
        }
    }
    else if (Engine.ECS().Registry.any_of<UIElement>(entity))
    {
        auto& uiElement{Engine.ECS().Registry.get<UIElement>(entity)};
        if (uiElement.HasChildren())
        {
            for (const auto childEntity : uiElement)
            {
                if (!Engine.ECS().Registry.any_of<HideTransform>(childEntity))
                {
                    if (!StepThroughHierarchy(childEntity, archive, sceneData, visitedSet, eStruct.m_key))
                    {
                        return false;
                    }
                }
            }
        }
    }

    // Reduce recursion depth.
    if (iScene)
    {
        visitedSet.pop_back();
    }

    return true;
}

bool internal::RecursiveInvestigation(const std::string& path, std::vector<UID>& visitedSet)
{
    const auto filename{Engine.FileIO().GetPath(FileIO::Directory::Assets, path)};
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        Log::Warn("Cannot load scene. File not found");
        return false;
    }

    try
    {
        cereal::JSONInputArchive archive(ifs);

        return Investigate(archive, visitedSet);
    }
    catch (const std::exception& e)
    {
        Log::Error("Deserialization error: {}", e.what());
        return false;
    }
}

bool internal::Investigate(cereal::JSONInputArchive& archive, std::vector<UID>& visitedSet)
{
    SceneFileData sceneData{};
    sceneData.load(archive);

    // Check each referenced scene.
    AllSceneReferences& eRefs{sceneData.m_references};
    for (auto& eRef : eRefs.m_data)
    {
        // If true, this file caused recursion.
        if (RecursionCheck(eRef.m_filepath, visitedSet))
        {
            return true;
        }

        // If this is true, a reference within this file caused recursion.
        if (RecursiveInvestigation(eRef.m_filepath, visitedSet))
        {
            return true;
        }

        // Reduce recursion depth.
        visitedSet.pop_back();
    }

    // This is good! No recursion.
    return false;
}

bool internal::RecursionCheck(const std::string& path, std::vector<UID>& visitedSet)
{
    Log::Info("Recursion check on {}", path);

    // Check if path was already visited in this recursion excursion. :)
    auto hash{HashFilepath(path)};

    auto it{std::find(visitedSet.begin(), visitedSet.end(), hash)};

    // Add path hash to visited list if not visited yet.
    if (it == visitedSet.end())
    {
        visitedSet.push_back(hash);
        return false;
    }

    // This is a problem.
    Log::Warn("Aborting save: scene recursion detected.");
    return true;
}

size_t internal::HashFilepath(const std::string& filepath)
{
    return std::hash<std::string>()(filepath);
}

template <typename Type>
[[maybe_unused]] void internal::RegisterComponentForSerialization()
{
    entt::meta_factory<Type>()
        .type(entt::type_hash<Type>::value())
        .template func<&internal::SaveType<Type>>("SaveType"_hs)
        .template func<&internal::LoadType<Type>>("LoadType"_hs)
        .template func<&Type::Load>("Load"_hs)
        .template func<&Type::Save>("Save"_hs);
}

template <typename Type>
void internal::SaveType(cereal::JSONOutputArchive& archive, const Entity entity)
{
    Type& component = Engine.ECS().Registry.get<Type>(entity);

    component.Save(archive, entity);
}

template <typename Type>
void internal::LoadType(cereal::JSONInputArchive& archive, const Entity entity)
{
    Type& component = Engine.ECS().CreateComponent<Type>(entity);

    try
    {
        component.Load(archive, entity);
    }
    catch (...)
    {
        Engine.ECS().Registry.remove<Type>(entity);
    }
}
