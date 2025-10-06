#include "tools/xsr_debug_render_tools.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

void bee::debug_draw_tools::draw_cross(glm::vec3 position, float size, glm::vec4 color)
{
    glm::vec3 p1 = position + glm::vec3(size / 2.0f, 0, -size / 2.0f);
    glm::vec3 p2 = position + glm::vec3(size / 2.0f, 0, size / 2.0f);
    glm::vec3 p3 = position + glm::vec3(-size / 2.0f, 0, size / 2.0f);
    glm::vec3 p4 = position + glm::vec3(-size / 2.0f, 0, -size / 2.0f);

    xsr::render_debug_line(glm::value_ptr(p1), glm::value_ptr(p3), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(p2), glm::value_ptr(p4), glm::value_ptr(color));
}

void bee::debug_draw_tools::draw_square(glm::vec3 position, float size, glm::vec4 color)
{
    glm::vec3 p1 = position + glm::vec3(size / 2.0f, 0, -size / 2.0f);
    glm::vec3 p2 = position + glm::vec3(size / 2.0f, 0, size / 2.0f);
    glm::vec3 p3 = position + glm::vec3(-size / 2.0f, 0, size / 2.0f);
    glm::vec3 p4 = position + glm::vec3(-size / 2.0f, 0, -size / 2.0f);

    xsr::render_debug_line(glm::value_ptr(p1), glm::value_ptr(p2), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(p2), glm::value_ptr(p3), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(p3), glm::value_ptr(p4), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(p4), glm::value_ptr(p1), glm::value_ptr(color));
}

void bee::debug_draw_tools::draw_cone(glm::vec3 position, glm::vec3 upDirection, float radius, float angleDegrees)
{
    const int segments = 45;

    // Normalize the up direction
    glm::vec3 normalizedConeDirection = glm::normalize(upDirection);
    float halfAngle = glm::radians(angleDegrees / 2);

    // Calculate the base and top circle centers
    glm::vec3 position2 = position + normalizedConeDirection * radius;
    float radius2 = radius + radius * tan(halfAngle);

    // Compute the rotation matrix to align the cone with the up direction
    glm::vec3 defaultUp(0.0f, 1.0f, 0.0f);
    glm::quat rotation = glm::rotation(defaultUp, normalizedConeDirection);

    // Initial point holders for rendering
    glm::vec3 point1, lastPoint1;
    glm::vec3 point2, lastPoint2;

    // Draw the cone by rotating the points around the normalized cone direction
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * glm::pi<float>() * i / segments;

        // Calculate points on the base circle
        glm::vec3 basePoint(radius * glm::cos(angle), 0.0f, radius * glm::sin(angle));
        point1 = position + rotation * basePoint;

        // Calculate points on the top circle
        glm::vec3 topPoint(radius2 * glm::cos(angle), radius, radius2 * glm::sin(angle));
        point2 = position2 + rotation * topPoint;

        // Draw lines between the base and top circle points
        xsr::render_debug_line(glm::value_ptr(point1), glm::value_ptr(point2), glm::value_ptr(Colors::White));

        if (i > 0)
        {
            // Draw the circle edges for base and top
            xsr::render_debug_line(glm::value_ptr(lastPoint1), glm::value_ptr(point1), glm::value_ptr(Colors::White));
            xsr::render_debug_line(glm::value_ptr(lastPoint2), glm::value_ptr(point2), glm::value_ptr(Colors::White));
        }

        // Update last points
        lastPoint1 = point1;
        lastPoint2 = point2;
    }
}

void bee::debug_draw_tools::draw_arrow(glm::vec3 position, glm::vec3 direction, float length, glm::vec4 color)
{
    glm::vec3 endPoint = position + glm::normalize(direction) * length;

    xsr::render_debug_line(glm::value_ptr(position), glm::value_ptr(endPoint), glm::value_ptr(color));

    glm::vec3 orthogonal1 = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * (length * 0.1f);
    glm::vec3 orthogonal2 = glm::normalize(glm::cross(direction, glm::vec3(1.0f, 0.0f, 0.0f))) * (length * 0.1f);

    glm::vec3 arrowHead1 = endPoint - glm::normalize(direction) * (length * 0.2f) + orthogonal1;

    glm::vec3 arrowHead2 = endPoint - glm::normalize(direction) * (length * 0.2f) - orthogonal1;

    glm::vec3 arrowHead3 = endPoint - glm::normalize(direction) * (length * 0.2f) + orthogonal2;

    glm::vec3 arrowHead4 = endPoint - glm::normalize(direction) * (length * 0.2f) - orthogonal2;

    xsr::render_debug_line(glm::value_ptr(endPoint), glm::value_ptr(arrowHead1), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(endPoint), glm::value_ptr(arrowHead2), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(endPoint), glm::value_ptr(arrowHead3), glm::value_ptr(color));
    xsr::render_debug_line(glm::value_ptr(endPoint), glm::value_ptr(arrowHead4), glm::value_ptr(color));
}
