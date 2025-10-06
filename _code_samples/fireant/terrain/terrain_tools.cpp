#include "terrain/terrain_tools.hpp"

#include "platform/opengl/open_gl.hpp"

#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/input.hpp"
#include "core/resources.hpp"
#include "core/transform.hpp"
#include "platform/opengl/image_gl.hpp"
#include "rendering/camera_system.hpp"
#include "rendering/mesh.hpp"
#include "terrain/terrain_components.hpp"
#include "terrain/terrain_editor.hpp"
#include "tools/log.hpp"

#include "rendering/model.hpp"
#include "tools/asset_manager/engine_ui.hpp"

using namespace bee;

namespace bee::terrain_tools
{
// DO NOT USE DIRECTLY. Use GetCachedTerrainEntity()!
entt::entity cachedTerrainEntity = entt::null;

// DO NOT USE DIRECTLY. Use GetCachedTerrainHeightMapData()!
std::vector<unsigned char> cachedTerrainHeightMapData;
};  // namespace bee::terrain_tools

void bee::terrain_tools::CacheTerrain()
{
    for (const auto& [entity, terrain] : bee::Engine.ECS().Registry.view<bee::Terrain>().each())
    {
        cachedTerrainEntity = entity;
    }

    if (!IsTerrainCached())
    {
        Log::Warn("Warning: No terrain found to cache");
        return;
    }

    cachedTerrainHeightMapData = GetHeightMapTexture(cachedTerrainEntity);
    // Log::Info("Succesfully cached terrain");
}

bool bee::terrain_tools::IsTerrainCached()
{
    return Engine.ECS().Registry.valid(cachedTerrainEntity);
}

entt::entity& bee::terrain_tools::GetCachedTerrainEntity()
{
    assert(IsTerrainCached() && "Error: terrain entity is not cached or invalid");
    return cachedTerrainEntity;
}

std::vector<unsigned char>& bee::terrain_tools::GetCachedTerrainHeightMapData()
{
    assert(IsTerrainCached() && "Error: terrain entity is not cached or invalid");
    assert(!cachedTerrainHeightMapData.empty() && "Error: terrain height map data is not cached");
    return cachedTerrainHeightMapData;
}

void bee::terrain_tools::CreateDefaultTerrain()
{
    Log::Info("Creating default terrain...");

    const auto& entity{bee::Engine.ECS().CreateEntity()};

    auto& terrain{bee::Engine.ECS().CreateComponent<bee::Terrain>(entity)};
    terrain.HeightMapImagePath = "terrain/textures/2048_HM.png";
    terrain.TerrainTextureImagePath = "terrain/textures/4096_TT.png";

    terrain.TerrainSize = 1024;

    auto material{bee::terrain_tools::CreateTerrainMaterial(terrain)};
    auto mesh{bee::terrain_tools::CreateTerrainMesh(terrain)};

    bee::Engine.ECS().CreateComponent<bee::TerrainMeshRenderer>(entity, mesh, material);
    auto& transform = bee::Engine.ECS().CreateComponent<bee::Transform>(entity);

    transform.Name = "Terrain";

    CacheTerrain();
}

std::vector<unsigned char> terrain_tools::GetHeightMapTexture(const entt::entity terrainEntity)
{
    auto& ecs = Engine.ECS();

    std::vector<unsigned char> fullTextureData;

    const auto& renderer = ecs.Registry.get<TerrainMeshRenderer>(terrainEntity);
    const auto& image = renderer.Material->HeightMapTexture->Image;

    glBindTexture(GL_TEXTURE_2D, image->GetTextureId());
    fullTextureData.resize(image->GetWidth() * image->GetHeight() * 4);  // RGBA8 format
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, fullTextureData.data());

    return fullTextureData;
}

std::vector<unsigned char> terrain_tools::GetTerrainTexture(const entt::entity terrainEntity)
{
    auto& ecs = Engine.ECS();

    std::vector<unsigned char> fullTextureData;

    const auto& renderer = ecs.Registry.get<TerrainMeshRenderer>(terrainEntity);
    const auto& image = renderer.Material->BaseColorTexture->Image;

    glBindTexture(GL_TEXTURE_2D, image->GetTextureId());
    fullTextureData.resize(image->GetWidth() * image->GetHeight() * 4);  // RGBA8 format
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, fullTextureData.data());

    return fullTextureData;
}

std::pair<bool, float> terrain_tools::GetTerrainHeight(const glm::vec3& position)
{
    auto& ecs = Engine.ECS();
    const auto& terrainEntity = GetCachedTerrainEntity();
    const auto& fullTextureData = GetCachedTerrainHeightMapData();

    // const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);
    auto _width =
        static_cast<float>(ecs.Registry.get<TerrainMeshRenderer>(terrainEntity).Material->HeightMapTexture->Image->GetWidth());
    auto _length =
        static_cast<float>(ecs.Registry.get<TerrainMeshRenderer>(terrainEntity).Material->HeightMapTexture->Image->GetHeight());

    const glm::vec2 uv = GetUVCoordinate(position);

    if (!IsUVOnPlane(uv))
        return std::make_pair(false, 0.0f);

    const glm::vec2 intersect = {uv.x * _width, uv.y * _length};

    const glm::vec2 pos00 = floor(intersect);
    glm::vec2 pos10 = glm::vec2(pos00.x + 1, pos00.y);
    glm::vec2 pos01 = glm::vec2(pos00.x, pos00.y + 1);
    glm::vec2 pos11 = glm::vec2(pos00.x + 1, pos00.y + 1);

    // Clamp to terrain bounds
    if (pos10.x >= _width)
    {
        pos10.x = static_cast<float>(_width - 1);
        pos11.x = static_cast<float>(_width - 1);
    }
    if (pos01.y >= _length)
    {
        pos01.y = static_cast<float>(_length - 1);
        pos11.y = static_cast<float>(_length - 1);
    }

    const int index00 = static_cast<int>(pos00.y * _width + pos00.x) * 4;
    const int index10 = static_cast<int>(pos10.y * _width + pos10.x) * 4;
    const int index01 = static_cast<int>(pos01.y * _width + pos01.x) * 4;
    const int index11 = static_cast<int>(pos11.y * _width + pos11.x) * 4;
    const float height00 = DecodeHeightFromRGBA(fullTextureData[index00],
                                                fullTextureData[index00 + 1],
                                                fullTextureData[index00 + 2],
                                                fullTextureData[index00 + 3]);
    const float height10 = DecodeHeightFromRGBA(fullTextureData[index10],
                                                fullTextureData[index10 + 1],
                                                fullTextureData[index10 + 2],
                                                fullTextureData[index10 + 3]);
    const float height01 = DecodeHeightFromRGBA(fullTextureData[index01],
                                                fullTextureData[index01 + 1],
                                                fullTextureData[index01 + 2],
                                                fullTextureData[index01 + 3]);
    const float height11 = DecodeHeightFromRGBA(fullTextureData[index11],
                                                fullTextureData[index11 + 1],
                                                fullTextureData[index11 + 2],
                                                fullTextureData[index11 + 3]);

    const float u = intersect.x - intersect.x;
    const float v = intersect.y - intersect.y;

    float interpolatedHeight =
        (1 - u) * (1 - v) * height00 + u * (1 - v) * height10 + (1 - u) * v * height01 + u * v * height11;

    return std::make_pair(true, interpolatedHeight);
}

std::pair<bool, glm::quat> bee::terrain_tools::GetTerrainNormal(const glm::vec3& position)
{
    auto resultCenter = GetTerrainHeight(position);

    if (!resultCenter.first)
    {
        return std::make_pair(false, glm::quat(1, 0, 0, 0));
    }

    float height = resultCenter.second;

    // Compute height differences for normals
    auto resultL = GetTerrainHeight(position + glm::vec3(-1, 0, 0) * 0.5f);
    float heightL = resultL.first ? resultL.second : height;

    auto resultR = GetTerrainHeight(position + glm::vec3(1, 0, 0) * 0.5f);
    float heightR = resultR.first ? resultR.second : height;

    auto resultD = GetTerrainHeight(position + glm::vec3(0, 0, -1) * 0.5f);
    float heightD = resultD.first ? resultD.second : height;

    auto resultU = GetTerrainHeight(position + glm::vec3(0, 0, 1) * 0.5f);
    float heightU = resultU.first ? resultU.second : height;

    // Calculate normal vector
    glm::vec3 normal = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));

    // Calculate rotation quaternion to align object with the normal
    glm::quat rotation = glm::rotation(glm::vec3(0, 1, 0), normal);

    return std::make_pair(true, rotation);
};

bool terrain_tools::IsUVOnPlane(const glm::vec2& uv)
{
    return uv.x >= 0.0f && uv.x < 1.0f && uv.y >= 0.0f && uv.y < 1.0f;
}

glm::vec2 terrain_tools::GetUVCoordinate(const glm::vec3& targetPosition)
{
    auto& ecs = Engine.ECS();
    const auto& terrainEntity = GetCachedTerrainEntity();

    const auto& transform = ecs.Registry.get<Transform>(terrainEntity);
    const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);

    // Define the plane's local axes
    // glm::vec3 uAxis = glm::normalize(glm::cross(planeNormal, glm::vec3(0.0f, 1.0f, 0.0f)));
    const auto uAxis = glm::vec3(1.0f, 0.0f, 0.0f);

    const auto vAxis = glm::vec3(0.0f, 0.0f, 1.0f);  // glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), uAxis);

    // Transform intersection point to plane's local space
    const glm::vec3 localPoint = targetPosition - transform.GetTranslation();

    // Project the local point onto the plane's axes
    const float uCoord = dot(localPoint, uAxis);
    const float vCoord = dot(localPoint, vAxis);

    glm::vec2 uv;

    // Convert to UV coordinates (normalized to [0, 1])
    uv.x = (uCoord / terrain.TerrainSize) + 0.5f;  // Map from [-width/2, width/2] to [0, 1]
    uv.y = (vCoord / terrain.TerrainSize) + 0.5f;  // Map from [-height/2, height/2] to [0, 1]

    return uv;
}

// Encode a float height into RGBA channels
void terrain_tools::EncodeHeightToRGBA(const float height,
                                       unsigned char& r,
                                       unsigned char& g,
                                       unsigned char& b,
                                       unsigned char& a)
{
    // Reinterpret the float as a 32-bit integer for bit manipulation
    const uint32_t heightBits = *reinterpret_cast<const uint32_t*>(&height);

    // Extract 8 bits for each channel
    r = (heightBits >> 24) & 0xFF;  // Most significant 8 bits
    g = (heightBits >> 16) & 0xFF;  // Next 8 bits
    b = (heightBits >> 8) & 0xFF;   // Next 8 bits
    a = heightBits & 0xFF;          // Least significant 8 bits
}

// Decode RGBA channels back into a float height
float terrain_tools::DecodeHeightFromRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // Combine the 8-bit channels into a 32-bit integer
    uint32_t heightBits = (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) |
                          (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);

    // Reinterpret the 32-bit integer as a float
    return *reinterpret_cast<float*>(&heightBits);
}

bool bee::terrain_tools::IsPointOnTerrain(const glm::vec3& targetPosition)
{
    return terrain_tools::IsUVOnPlane(terrain_tools::GetUVCoordinate(targetPosition));
}

glm::vec3 bee::terrain_tools::GetTerrainLocalCoordinate(const glm::vec3& targetPosition)
{
    auto& ecs = Engine.ECS();
    const auto& terrainEntity = GetCachedTerrainEntity();
    const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);

    auto UV = terrain_tools::GetUVCoordinate(targetPosition);

    auto _localCoordinate = glm::vec3(0.0f);

    // Convert UV to pixel coordinates
    _localCoordinate.x = static_cast<float>(static_cast<int>(UV.x * terrain.TerrainSize));
    _localCoordinate.y = terrain_tools::GetTerrainHeight(targetPosition).second;
    _localCoordinate.z = static_cast<float>(static_cast<int>(UV.y * terrain.TerrainSize));
    return _localCoordinate;
}

float bee::terrain_tools::GetTerrainHeightMapScale()
{
    auto& ecs = Engine.ECS();
    const auto& terrainEntity = GetCachedTerrainEntity();
    const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);
    auto _width =
        static_cast<float>(ecs.Registry.get<TerrainMeshRenderer>(terrainEntity).Material->HeightMapTexture->Image->GetWidth());

    return _width / terrain.TerrainSize;
}

float bee::terrain_tools::GetTerrainTextureScale()
{
    auto& ecs = Engine.ECS();
    const auto& terrainEntity = GetCachedTerrainEntity();
    const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);
    auto _width =
        static_cast<float>(ecs.Registry.get<TerrainMeshRenderer>(terrainEntity).Material->BaseColorTexture->Image->GetWidth());

    return _width / terrain.TerrainSize;
}

std::shared_ptr<TerrainMesh> terrain_tools::CreateTerrainMesh(const Terrain& terrain, uint32_t resolution)
{
    // For tessellation, we'll create a grid of patches (quads)
    // resolution determines how many patches we want in each dimension

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
    //  Calculate the size of each patch
    const auto width = static_cast<float>(terrain.TerrainSize);
    const auto height = static_cast<float>(terrain.TerrainSize);
    const float patchWidth = width / static_cast<float>(resolution);
    const float patchLength = height / static_cast<float>(resolution);
    auto indexOffset{0u};
    for (uint32_t j = 0; j < resolution; j++)
    {
        for (uint32_t i = 0; i < resolution; i++)
        {
            const auto realI = static_cast<float>(i);
            const auto realJ = static_cast<float>(j);
            // Create a quad (4 vertices) for each patch
            // Vertex 0: bottom-left
            positions.emplace_back(-width / 2.0f + realI * patchWidth, 0.0f, -height / 2.0f + realJ * patchLength);
            uvs.emplace_back(realI / static_cast<float>(resolution), realJ / static_cast<float>(resolution));

            // Vertex 1: bottom-right
            positions.emplace_back(-width / 2.0f + (realI + 1) * patchWidth, 0.0f, -height / 2.0f + realJ * patchLength);
            uvs.emplace_back((realI + 1) / static_cast<float>(resolution), realJ / static_cast<float>(resolution));

            // Vertex 2: top-left
            positions.emplace_back(-width / 2.0f + realI * patchWidth, 0.0f, -height / 2.0f + (realJ + 1) * patchLength);
            uvs.emplace_back(realI / static_cast<float>(resolution), (realJ + 1) / static_cast<float>(resolution));

            // Vertex 3: top-right
            positions.emplace_back(-width / 2.0f + (realI + 1) * patchWidth, 0.0f, -height / 2.0f + (realJ + 1) * patchLength);
            uvs.emplace_back((realI + 1) / static_cast<float>(resolution), (realJ + 1) / static_cast<float>(resolution));

            indices.push_back(indexOffset);      // Bottom-left
            indices.push_back(indexOffset + 2);  // Top-left
            indices.push_back(indexOffset + 1);  // Bottom-right
            indices.push_back(indexOffset + 3);  // Top-right

            indexOffset += 4;  // Move to next patch
        }
    }

    // patches do not require indices
    auto planeMesh = Engine.Resources().Create<TerrainMesh>();
    planeMesh->SetAttribute(TerrainMesh::Attribute::Position, positions);
    planeMesh->SetAttribute(TerrainMesh::Attribute::Texture, uvs);
    planeMesh->SetIndices(indices);
    return planeMesh;
}

std::shared_ptr<TerrainMaterial> terrain_tools::CreateTerrainMaterial(const Terrain& terrain)
{
    auto terrainMaterial{std::make_shared<TerrainMaterial>()};

    auto samplerHeightMap{std::make_shared<Sampler>()};

    terrainMaterial->BaseColorFactor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    const std::shared_ptr<Texture> texture = {
        std::make_shared<Texture>(Engine.Resources().Load<Image>(FileIO::Directory::Assets, terrain.HeightMapImagePath),
                                  samplerHeightMap)};

    auto samplerBaseColor{std::make_shared<Sampler>()};
    samplerBaseColor->WrapS = Sampler::Wrap::Repeat;
    samplerBaseColor->WrapT = Sampler::Wrap::Repeat;

    const std::shared_ptr<Texture> baseColorTexture = {
        std::make_shared<Texture>(Engine.Resources().Load<Image>(FileIO::Directory::Assets, terrain.TerrainTextureImagePath),
                                  samplerBaseColor)};

    terrainMaterial->HeightMapTexture = texture;

    terrainMaterial->UseHeightMapTexture = true;

    terrainMaterial->BaseColorTexture = baseColorTexture;
    terrainMaterial->UseBaseTexture = true;

    return terrainMaterial;
}

bool terrain_tools::GetPointOnTerrain(glm::vec3& hit)
{
    bool foundIntersection = false;

    for (const auto& [entity, camera, cam_transform] : Engine.ECS().Registry.view<Camera, Transform>().each())
    {
        if (CameraSystem::IsMainCamera(entity) == false)
        {
            continue;
        }
        auto ray = GetRayFromScreenToWorld(cam_transform, camera.Projection);

        foundIntersection = FindIntersection(cam_transform.GetTranslation(), ray, hit);

        if (foundIntersection)
        {
            return true;
        }
    }

    return false;
}

glm::vec3 terrain_tools::GetRayFromScreenToWorld(bee::Transform& cameraTransform, const glm::mat4& cameraProjection)
{
    const glm::vec2 mousePosition = Engine.Input().GetMousePositionInViewport();

    // Derive the view matrix from camera transform (assuming it's a world transform)
    const glm::mat4 viewMatrix = inverse(cameraTransform.World());  // Convert world-space transform to view matrix

    // Compute the inverse of the View-Projection matrix
    const glm::mat4 invVP = inverse(cameraProjection * viewMatrix);

    // Unproject near and far points
    const glm::vec4 farPoint = invVP * glm::vec4(mousePosition, 1.0f, 1.0f);

    const glm::vec3 nearPos = cameraTransform.GetTranslation();
    const glm::vec3 farPos = glm::vec3(farPoint) / farPoint.w;

    // Compute ray direction
    const glm::vec3 dir = normalize(farPos - nearPos);

    return dir;
}

bool terrain_tools::FindIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& hit)
{
    // Terrain info.
    auto& ecs = Engine.ECS();

    const auto& terrainEntity = GetCachedTerrainEntity();

    const auto& terrain = ecs.Registry.get<Terrain>(terrainEntity);

    // Ray traversal info.
    const float stepSize = 1.0f;
    const int maxSteps = static_cast<int>(terrain.TerrainSize / stepSize);
    glm::vec3 currentPos = rayOrigin;

    // Traversal loop.
    for (int step = 0; step < maxSteps; step++)
    {
        // Get the current height of the ray that is in world space.
        const float t = (currentPos.x - rayOrigin.x) / rayDirection.x;
        const float rayHeight = rayOrigin.y + rayDirection.y * t;

        const float terrainHeight = GetTerrainHeight(currentPos).second;

        // Is the ray below the terrain? Then we have a intersection.
        if (rayHeight <= terrainHeight)
        {
            hit = glm::vec3(currentPos.x, terrainHeight, currentPos.z);
            return true;
        }

        // Step onwards.
        currentPos += rayDirection * stepSize;
    }

    return false;
}
