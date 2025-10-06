#include "tools/tools.hpp"
#include <glm/gtc/random.hpp>

using namespace std;

// Courtesy of: http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
string bee::StringReplace(const string& subject, const string& search, const string& replace)
{
    string result(subject);
    size_t pos = 0;

    while ((pos = subject.find(search, pos)) != string::npos)
    {
        result.replace(pos, search.length(), replace);
        pos += search.length();
    }

    return result;
}

bool bee::StringEndsWith(const string& subject, const string& suffix)
{
    // Early out test:
    if (suffix.length() > subject.length()) return false;

    // Resort to difficult to read C++ logic:
    return subject.compare(subject.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool bee::StringStartsWith(const string& subject, const std::string& prefix)
{
    // Early out, prefix is longer than the subject:
    if (prefix.length() > subject.length()) return false;

    // Compare per character:
    for (size_t i = 0; i < prefix.length(); ++i)
        if (subject[i] != prefix[i]) return false;

    return true;
}

std::vector<std::string> bee::SplitString(const std::string& input, const std::string& delim)
{
    std::vector<std::string> result;
    size_t pos = 0, pos2 = 0;
    while ((pos2 = input.find(delim, pos)) != std::string::npos)
    {
        result.push_back(input.substr(pos, pos2 - pos));
        pos = pos2 + 1;
    }

    result.push_back(input.substr(pos));

    return result;
}

float bee::GetRandomNumber(float min, float max, int decimals)
{
    if (min >= max)
    {
        return min;
    }

    auto p = static_cast<int>(pow(10, decimals));
    auto imin = static_cast<int>(min * p);
    auto imax = static_cast<int>(max * p);

    int irand = imin + rand() % (imax - imin);
    float val = (float)irand / p;
    return val;
}

glm::vec4 bee::GetRandomColor()
{
    return glm::vec4(GetRandomNumber(0.0f, 1.0f), GetRandomNumber(0.0f, 1.0f), GetRandomNumber(0.0f, 1.0f), 1.0f);
}

// Accepts degrees
glm::vec3 bee::RandomDirectionInCone(const glm::vec3& coneDirection, float angle)
{
    // Normalize the cone direction
    glm::vec3 normalizedConeDirection = glm::normalize(coneDirection);

    // Convert angle from degrees to radians
    float halfAngle = glm::radians(angle / 2.0f);

    // Generate a random angle within the cone
    float theta = glm::linearRand(0.0f, halfAngle);  // Adjusted to be from 0 to halfAngle

    // Generate a random azimuthal angle
    float phi = glm::linearRand(0.0f, glm::two_pi<float>());

    // Compute the direction using spherical coordinates
    float x = glm::sin(theta) * glm::cos(phi);
    float y = glm::cos(theta);
    float z = glm::sin(theta) * glm::sin(phi);

    // Create the random direction in local space
    glm::vec3 localDirection(x, y, z);

    // Rotate the local direction to align with the cone direction
    glm::vec3 axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normalizedConeDirection);
    float angleBetween = acos(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), normalizedConeDirection));

    if (glm::length(axis) > 0.0001f)
    {  // Check if the axis is not negligible
        axis = glm::normalize(axis);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleBetween, axis);
        localDirection = glm::vec3(rotationMatrix * glm::vec4(localDirection, 0.0f));
    }

    return localDirection;
}

glm::vec3 bee::RandomPointOnCircle(float radius)
{
    // Generate a random angle
    float angle = glm::linearRand(0.0f, glm::two_pi<float>());

    // Calculate the x and z coordinates on the circle
    float x = radius * glm::cos(angle);
    float z = radius * glm::sin(angle);

    // Return the point on the circle in the XY plane, assuming y = 0
    return glm::vec3(x, 0.0f, z);
}

glm::vec4 bee::lerp(const glm::vec4& start, const glm::vec4& end, const float t) { return (1.0f - t) * start + t * end; }
