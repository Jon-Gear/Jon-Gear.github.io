#include "core/transform.hpp"

#include <algorithm>
#include <cassert>

#include <entt/entity/helper.hpp>

#include "core/ecs.hpp"
#include "core/engine.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cereal/cereal.hpp>

using namespace bee;
using namespace glm;

// ------------------------------ Iterator ------------------------------
Transform::Iterator::Iterator(const Transform* owner, std::size_t index) : m_owner(owner), m_index(index) {}

Transform::Iterator& Transform::Iterator::operator++()
{
    ++m_index;
    return *this;
}

bool Transform::Iterator::operator!=(const Iterator& rhs) const { return m_owner != rhs.m_owner || m_index != rhs.m_index; }

Entity Transform::Iterator::operator*() const
{
    assert(m_owner);
    assert(m_index < m_owner->m_children.size());
    return m_owner->m_children[m_index];
}

Transform::Iterator Transform::begin() const { return Iterator(this, 0); }
Transform::Iterator Transform::end() const { return Iterator(this, m_children.size()); }

// ------------------------------ Hierarchy ------------------------------
void Transform::SetParent(Entity parent)
{
    assert(parent == entt::null || Engine.ECS().Registry.valid(parent));

    // Find the entity that owns *this* component
    const auto& storage = Engine.ECS().Registry.storage<Transform>();
    Entity thisEntity = entt::to_entity(storage, *this);

    // Detach from current parent if any
    if (m_parent != entt::null && Engine.ECS().Registry.valid(m_parent))
    {
        auto& oldParent = Engine.ECS().Registry.get<Transform>(m_parent);
        oldParent.RemoveChild(thisEntity);
    }

    // Attach to new parent if any
    if (parent != entt::null && Engine.ECS().Registry.valid(parent))
    {
        auto& newParent = Engine.ECS().Registry.get<Transform>(parent);
        newParent.AddChild(thisEntity);
    }

    m_parent = parent;

    // Mark dirty for self & descendants
    SetMatrixDirty();
}

void Transform::AddChild(Entity child)
{
    assert(Engine.ECS().Registry.valid(child));

    // Avoid duplicates while keeping insertion order stable
    if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
    {
        m_children.push_back(child);
    }
}

void Transform::RemoveChild(Entity child)
{
    assert(Engine.ECS().Registry.valid(child));
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) m_children.erase(it);
}
// ------------------------------ Matrix ops ------------------------------
void Transform::SetFromMatrix(const mat4& m44)
{
    // Use glm::decompose for a distinct implementation from the original
    // while preserving semantics (T * R * S composition order).
    vec3 skew{};
    vec4 perspective{};
    vec3 translation{};
    vec3 scale{};
    quat rotation{};
    mat4 local = m44;

    // If the matrix contains a parent transform, we leave it to the caller to
    // supply a local matrix; behavior matches the original function contract.
    bool ok = glm::decompose(local, scale, rotation, translation, skew, perspective);
    if (ok)
    {
        m_translation = translation;
        m_scale = scale;
        // glm::decompose returns rotation conjugated compared to some conventions.
        // to keep parity with original usage (toMat4(m_rotation)), normalize.
        m_rotation = glm::normalize(rotation);
    }
    else
    {
        // Fallback: preserve prior state if decomposition fails.
    }

    SetMatrixDirty();
}

void Transform::SetMatrixDirty()
{
    m_worldMatrixDirty = true;

    // Propagate to descendants
    for (auto child : *this)
    {
        auto& ct = Engine.ECS().Registry.get<Transform>(child);
        ct.SetMatrixDirty();
    }
}

const mat4& Transform::World()
{
    if (!m_worldMatrixDirty) return m_worldMatrix;

    mat4 T = glm::translate(mat4(1.0f), m_translation);
    mat4 R = glm::toMat4(m_rotation);
    mat4 S = glm::scale(mat4(1.0f), m_scale);

    if (m_parent == entt::null)
    {
        m_worldMatrix = T * R * S;
    }
    else
    {
        assert(Engine.ECS().Registry.valid(m_parent));
        auto& pt = Engine.ECS().Registry.get<Transform>(m_parent);
        m_worldMatrix = pt.World() * (T * R * S);
    }

    m_worldMatrixDirty = false;
    return m_worldMatrix;
}

// ------------------------------ Serialization ------------------------------
void bee::Transform::Save(cereal::JSONOutputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("Transform_" + number, *this));
}

void bee::Transform::Load(cereal::JSONInputArchive& archive, const entt::entity, const std::string& number)
{
    archive(cereal::make_nvp("Transform_" + number, *this));
}

template <class Archive>
void Transform::serialize(Archive& ar)
{
    ar(cereal::make_nvp("Name", Name),
       cereal::make_nvp("Translation", m_translation),
       cereal::make_nvp("Scale", m_scale),
       cereal::make_nvp("Rotation", m_rotation));
}

// Explicit template instantiations for the types you intend to use
template void Transform::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void Transform::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);
