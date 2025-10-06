#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cereal/cereal.hpp>

namespace glm
{

template <class Archive>
void serialize(Archive& archive, glm::vec2& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y));
}

template <class Archive>
void serialize(Archive& archive, glm::vec3& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
}

template <class Archive>
void serialize(Archive& archive, glm::vec4& v)
{
    archive(cereal::make_nvp("r", v.r), cereal::make_nvp("g", v.g), cereal::make_nvp("b", v.b), cereal::make_nvp("a", v.a));
}

template <class Archive>
void serialize(Archive& archive, glm::ivec2& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y));
}

template <class Archive>
void serialize(Archive& archive, glm::ivec3& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
}

template <class Archive>
void serialize(Archive& archive, glm::ivec4& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w));
}

template <class Archive>
void serialize(Archive& archive, glm::uvec2& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y));
}

template <class Archive>
void serialize(Archive& archive, glm::uvec3& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
}

template <class Archive>
void serialize(Archive& archive, glm::uvec4& v)
{
    archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w));
}

template <class Archive>
void serialize(Archive& archive, glm::mat3& m)
{
    archive(cereal::make_nvp("row0", m[0]), cereal::make_nvp("row1", m[1]), cereal::make_nvp("row2", m[2]));
}

template <class Archive>
void serialize(Archive& archive, glm::mat4& m)
{
    archive(cereal::make_nvp("row0", m[0]),
            cereal::make_nvp("row1", m[1]),
            cereal::make_nvp("row2", m[2]),
            cereal::make_nvp("row3", m[3]));
}

template <class Archive>
void serialize(Archive& ar, quat& q)
{
    ar(cereal::make_nvp("x", q.x), cereal::make_nvp("y", q.y), cereal::make_nvp("z", q.z), cereal::make_nvp("q", q.w));
}

}  // namespace glm
