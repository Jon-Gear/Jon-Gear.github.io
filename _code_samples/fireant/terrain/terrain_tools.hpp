#pragma once

#include "platform/opengl/mesh_gl.hpp"

#include <vector>
#include <memory>

#include <entt/entt.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace bee
{
class Mesh;
struct TerrainMaterial;
struct Terrain;
struct Transform;
}  // namespace bee

namespace bee::terrain_tools
{

extern entt::entity cachedTerrainEntity;
extern std::vector<unsigned char> cachedTerrainHeightMapData;

void CacheTerrain();

bool IsTerrainCached();

entt::entity& GetCachedTerrainEntity();
std::vector<unsigned char>& GetCachedTerrainHeightMapData();

void CreateDefaultTerrain();

std::shared_ptr<bee::TerrainMesh> CreateDefaultTerrainMesh();

std::shared_ptr<bee::TerrainMaterial> CreateDefaultTerrainMaterial();

bool IsPointOnTerrain(const glm::vec3& targetPosition);

glm::vec3 GetTerrainLocalCoordinate(const glm::vec3& targetPosition);

float GetTerrainHeightMapScale();
float GetTerrainTextureScale();

/// <summary>
/// Used for tesselated version
/// </summary>
/// <param name="terrain"></param>
/// <param name="rez"></param>
/// <returns></returns>
std::shared_ptr<TerrainMesh> CreateTerrainMesh(const Terrain& terrain, uint32_t rez = 20u);

std::shared_ptr<bee::TerrainMaterial> CreateTerrainMaterial(const Terrain&);

std::vector<unsigned char> GetHeightMapTexture(const entt::entity);
std::vector<unsigned char> GetTerrainTexture(const entt::entity);

/// <summary>
/// Gets the height of the terrain at the given position.
/// </summary>
/// <param name="position">The position to get the height at (the Y value is ignored).</param>
/// <param name="terrainEntity">The terrain entity to get the height from.</param>
/// <param name="fullTextureData">The texture data for the terrain.</param>
/// <returns>A pair. The first value indicates if the lookup was successful. The second value is the height.</returns>
std::pair<bool, float> GetTerrainHeight(const glm::vec3& position);

std::pair<bool, glm::quat> GetTerrainNormal(const glm::vec3& position);

void EncodeHeightToRGBA(const float height, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);

float DecodeHeightFromRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

bool IsUVOnPlane(const glm::vec2& uv);

glm::vec2 GetUVCoordinate(const glm::vec3& targetPosition);

/// <summary>
/// Returns a world position of the mouse and terrain intersection.
/// </summary>
/// <param name="hit">An in-out parameter that will contain the intersection point.</param>
/// <param name="terrainEntity">The terrain entity to intersect. If not provided, the current terrain will be used.</param>
/// <returns>A value to indicate if the method was successful. Usually fails if the mouse is not over the terrain.</returns>
bool GetPointOnTerrain(glm::vec3& hit);

/// <summary>
/// Creates a ray from the camera, through the mouse, into the world.
/// Intended to be called by GetPointOnTerrain().
/// </summary>
/// <param name="worldSpace">Matrix of the world space.</param>
/// <param name="cameraProjection">Camera's projection matrix.</param>
/// <returns>A direction (ray) that travels from camera through the mouse.</returns>
glm::vec3 GetRayFromScreenToWorld(bee::Transform& cameraTransform, const glm::mat4& cameraProjection);

/// <summary>
/// Gets the world position of where a ray intersects the terrain.
/// Intended to be called by GetRayFromScreenToWorld().
/// </summary>
/// <param name="rayOrigin">Where the ray starts (usually the camera position).</param>
/// <param name="rayDirection">The direction (ray) to travel.</param>
/// <param name="hit">An in-out parameter that will contain the intersection point.</param>
/// <param name="terrainEntity">The terrain entity to intersect. If not provided, the current terrain will be used.</param>
/// <returns>A value to indicate if an intersection was found.</returns>
bool FindIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& hit);

}  // namespace bee::terrain_tools
