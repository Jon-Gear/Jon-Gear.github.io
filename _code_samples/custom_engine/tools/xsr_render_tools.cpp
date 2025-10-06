#include "tools/xsr_render_tools.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

xsr::texture_handle bee::xsr_internal::create_white_texture()
{
    int width = 4;
    int height = 4;
    std::vector<xsr::uchar> pixels(width * height * 4);
    for (int i = 0; i < width * height; ++i)
    {
        int index = i * 4;
        pixels[index + 0] = 255;
        pixels[index + 1] = 255;
        pixels[index + 2] = 255;
        pixels[index + 3] = 255;
    }
    return xsr::create_texture(width, height, pixels.data());
}

xsr::mesh_handle bee::xsr_internal::create_box()
{
    std::vector<vec3> positions = {{-0.5f, -0.5f, 0.5f},  // Front top left
                                   {0.5f, -0.5f, 0.5f},   // Front top right
                                   {0.5f, 0.5f, 0.5f},    // Front bottom right
                                   {-0.5f, 0.5f, 0.5f},   // Front bottom left

                                   {-0.5f, -0.5f, -0.5f},  // Back top left
                                   {0.5f, -0.5f, -0.5f},   // Back top right
                                   {0.5f, 0.5f, -0.5f},    // Back bottom right
                                   {-0.5f, 0.5f, -0.5f},   // Back bottom left
                                   {-0.5f, 0.5f, 0.5f},    // Top top left
                                   {0.5f, 0.5f, 0.5f},     // Top top right
                                   {0.5f, 0.5f, -0.5f},    // Top bottom right
                                   {-0.5f, 0.5f, -0.5f},   // Top bottom left
                                   {-0.5f, -0.5f, 0.5f},   // Bottom top left
                                   {0.5f, -0.5f, 0.5f},    // Bottom top right
                                   {0.5f, -0.5f, -0.5f},   // Bottom bottom right
                                   {-0.5f, -0.5f, -0.5f},  // Bottom bottom left
                                   {-0.5f, -0.5f, 0.5f},   // Left top left
                                   {-0.5f, 0.5f, 0.5f},    // Left top right
                                   {-0.5f, 0.5f, -0.5f},   // Left bottom right
                                   {-0.5f, -0.5f, -0.5f},  // Left bottom left
                                   {0.5f, -0.5f, 0.5f},    // Right top left
                                   {0.5f, 0.5f, 0.5f},     // Right top right
                                   {0.5f, 0.5f, -0.5f},    // Right bottom right
                                   {0.5f, -0.5f, -0.5f}};  // Right bottom left

    std::vector<vec3> normals = {
        {0.0f, 0.0f, 1.0f},   // Front face
        {0.0f, 0.0f, 1.0f},   // Front face
        {0.0f, 0.0f, 1.0f},   // Front face
        {0.0f, 0.0f, 1.0f},   // Front face
        {0.0f, 0.0f, -1.0f},  // Back face
        {0.0f, 0.0f, -1.0f},  // Back face
        {0.0f, 0.0f, -1.0f},  // Back face
        {0.0f, 0.0f, -1.0f},  // Back face
        {0.0f, 1.0f, 0.0f},   // Top face
        {0.0f, 1.0f, 0.0f},   // Top face
        {0.0f, 1.0f, 0.0f},   // Top face
        {0.0f, 1.0f, 0.0f},   // Top face
        {0.0f, -1.0f, 0.0f},  // Bottom face
        {0.0f, -1.0f, 0.0f},  // Bottom face
        {0.0f, -1.0f, 0.0f},  // Bottom face
        {0.0f, -1.0f, 0.0f},  // Bottom face
        {-1.0f, 0.0f, 0.0f},  // Left face
        {-1.0f, 0.0f, 0.0f},  // Left face
        {-1.0f, 0.0f, 0.0f},  // Left face
        {-1.0f, 0.0f, 0.0f},  // Left face
        {1.0f, 0.0f, 0.0f},   // Right face
        {1.0f, 0.0f, 0.0f},   // Right face
        {1.0f, 0.0f, 0.0f},   // Right face
        {1.0f, 0.0f, 0.0f}    // Right face
    };

    std::vector<vec2> texture_coordinates = {{0.0f, 0.0f},  // Front top left
                                             {1.0f, 0.0f},  // Front top right
                                             {1.0f, 1.0f},  // Front bottom right
                                             {0.0f, 1.0f},  // Front bottom left
                                             {0.0f, 0.0f},  // Repeat the same texture coordinates for other faces
                                             {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},  // Repeat
                                             {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},  // Repeat
                                             {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},  // Repeat
                                             {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},  // Repeat
                                             {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    std::vector<vec3> colors = {
        {1.0f, 0.0f, 0.0f},  // Front face
        {1.0f, 0.0f, 0.0f},  // Front face
        {1.0f, 0.0f, 0.0f},  // Front face
        {1.0f, 0.0f, 0.0f},  // Front face
        {0.0f, 1.0f, 0.0f},  // Back face
        {0.0f, 1.0f, 0.0f},  // Back face
        {0.0f, 1.0f, 0.0f},  // Back face
        {0.0f, 1.0f, 0.0f},  // Back face
        {0.0f, 0.0f, 1.0f},  // Top face
        {0.0f, 0.0f, 1.0f},  // Top face
        {0.0f, 0.0f, 1.0f},  // Top face
        {0.0f, 0.0f, 1.0f},  // Top face
        {1.0f, 1.0f, 0.0f},  // Bottom face
        {1.0f, 1.0f, 0.0f},  // Bottom face
        {1.0f, 1.0f, 0.0f},  // Bottom face
        {1.0f, 1.0f, 0.0f},  // Bottom face
        {1.0f, 0.0f, 1.0f},  // Left face
        {1.0f, 0.0f, 1.0f},  // Left face
        {1.0f, 0.0f, 1.0f},  // Left face
        {1.0f, 0.0f, 1.0f},  // Left face
        {0.0f, 1.0f, 1.0f},  // Right face
        {0.0f, 1.0f, 1.0f},  // Right face
        {0.0f, 1.0f, 1.0f},  // Right face
        {0.0f, 1.0f, 1.0f}   // Right face
    };

    // Set all colors to white with a for loop
    for (int i = 0; i < 24; i++)
    {
        colors[i] = {1.0f, 1.0f, 1.0f};
    }

    // Create indices
    std::vector<unsigned int> indices = {
        0,  1,  2,  0,  2,  3,   // Front face
        4,  5,  6,  4,  6,  7,   // Back face
        8,  9,  10, 8,  10, 11,  // Top face
        12, 13, 14, 12, 14, 15,  // Bottom face
        16, 17, 18, 16, 18, 19,  // Left face
        20, 21, 22, 20, 22, 23   // Right face
    };

    // Create a mesh from the vertex data
    return xsr::create_mesh(indices.data(),
                            (unsigned)indices.size(),
                            &positions.data()->x,
                            &normals.data()->x,
                            &texture_coordinates.data()->x,
                            &colors.data()->x,
                            (unsigned)positions.size());
}
