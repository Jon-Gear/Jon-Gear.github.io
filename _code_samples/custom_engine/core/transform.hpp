#pragma once

#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

#include "core/ecs.hpp"

#include <cereal/cereal.hpp>
#include "tools/glm_cereal.hpp"
#include <cereal/archives/json.hpp>

namespace bee
{

using Entity = entt::entity;

struct Transform
{
    std::string Name{};
    glm::vec3 m_translation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
    glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f};

    // ---- Hierarchy API ----
    void SetParent(Entity parent);

    // ---- Transform API ----
    void SetFromMatrix(const glm::mat4& m44);
    const glm::mat4& World();

    // ---- Set/Get accessors (compat layer) ----
    // Getters (const)
    const glm::vec3& Translation() const { return m_translation; }
    const glm::quat& Rotation() const { return m_rotation; }
    const glm::vec3& Scale() const { return m_scale; }

    // Getters (mutable refs) — use setters if you need dirty propagation
    glm::vec3& GetTranslation() { return m_translation; }
    glm::quat& GetRotation() { return m_rotation; }
    glm::vec3& GetScale() { return m_scale; }

    // Setters (mark matrix dirty)
    void SetTranslation(const glm::vec3& t)
    {
        m_translation = t;
        SetMatrixDirty();
    }
    void SetTranslation(float x, float y, float z)
    {
        m_translation = {x, y, z};
        SetMatrixDirty();
    }

    void SetRotation(const glm::quat& q)
    {
        m_rotation = glm::normalize(q);
        SetMatrixDirty();
    }

    void SetScale(const glm::vec3& s)
    {
        m_scale = s;
        SetMatrixDirty();
    }
    void SetScale(float x, float y, float z)
    {
        m_scale = {x, y, z};
        SetMatrixDirty();
    }

    // ---- Child iteration support (range-for compatible) ----
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entity;
        using difference_type = std::ptrdiff_t;
        using pointer = const Entity*;
        using reference = const Entity&;

        Iterator(const Transform* owner, std::size_t index);
        Iterator& operator++();
        bool operator!=(const Iterator& rhs) const;
        Entity operator*() const;

    private:
        const Transform* m_owner{};
        std::size_t m_index{};
    };

    Iterator begin() const;
    Iterator end() const;

    // ---- Serialization ----
    template <class Archive>
    void serialize(Archive& ar);

    void Save(cereal::JSONOutputArchive&, const entt::entity, const std::string&);
    void Load(cereal::JSONInputArchive&, const entt::entity, const std::string&);

    // ---- Accessors ----
    Entity GetParent() const { return m_parent; }
    const std::vector<Entity>& GetChildren() const { return m_children; }
    bool HasChildren() const { return !m_children.empty(); }
    bool HasParent() const { return m_parent != entt::null; }

private:
    // NOTE: Implementation intentionally differs from the original. We use a
    // compact vector to store children instead of a sibling linked-list.
    Entity m_parent{entt::null};
    std::vector<Entity> m_children{};

    void AddChild(Entity child);
    void RemoveChild(Entity child);
    void SetMatrixDirty();

    // Cached world matrix
    bool m_worldMatrixDirty{true};
    glm::mat4 m_worldMatrix{1.0f};
};

}  // namespace bee
