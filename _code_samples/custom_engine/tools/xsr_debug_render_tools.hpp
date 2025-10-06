#pragma once
#include <xsr/include/xsr.hpp>
#include "glm/glm.hpp"

namespace bee::debug_draw_tools
{

void draw_cross(glm::vec3 position, float size, glm::vec4 color);

void draw_square(glm::vec3 position, float size, glm::vec4 color);

void draw_cone(glm::vec3 position, glm::vec3 upDirection, float radius, float angleDegrees);

void draw_arrow(glm::vec3 position, glm::vec3 direction, float length, glm::vec4 color);

namespace Colors
{

inline glm::vec4 Black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
inline glm::vec4 White = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
inline glm::vec4 Grey = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
inline glm::vec4 Red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
inline glm::vec4 Green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
inline glm::vec4 Blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
inline glm::vec4 Orange = glm::vec4(1.0f, 0.66f, 0.0f, 1.0f);
inline glm::vec4 Cyan = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
inline glm::vec4 Magenta = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
inline glm::vec4 Yellow = glm::vec4(1.0f, 1.0, 0.0f, 1.0f);
inline glm::vec4 Purple = glm::vec4(0.55f, 0.0, 0.65f, 1.0f);

}  // namespace Colors

}  // namespace bee::debug_draw_tools
