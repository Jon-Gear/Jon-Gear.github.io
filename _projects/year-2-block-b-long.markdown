---
layout: project
title: "Custom C++ Terrain Editor"
role: "Engine Programmer"
date: 2025-01-24

version: long
short_url: /projects/year-2-block-b-short
long_url: /projects/year-2-block-b-long

categories: [projects]
image: /assets/images/Y2B/main.gif
description: "Custom C++ Terrain Editor was a solo research project. The project features editable terrain and variety of brushes. The Terrain Editor would later become based for the Fire Ant Engine."
contributions: "Terrain Mesh Deformation via Height Maps | Vertex Shader with Dynamic Normals & Displacement | Fully Custom Brush Framework | Normals-based Object Placement"
tools: "C++, OpenGL, GLSL (Vertex & Fragment Shaders), ImGui, GLM"
team_size: "Solo"
platforms: "Windows"
duration: "8 weeks (Nov 2024 - Jan 2025)"
---

![image1](../../assets/images/Y2B/main.gif)


<a class="cta-link" href="https://youtu.be/JV7p1OzVp_o" target="_blank" rel="noopener">▶ Watch Full Showcase</a>

In the past 8 weeks, I have been working on a terrain editor.

In this blog post, I will explain how the terrain editor works and the relevant concepts related to terrain editing.

# Editor Features

Feel free to watch the showcase video above!

**Editable Terrain**

The tool contains a large terrain object which can be modified with brushes. The terrain is generated using a height map texture.

**Brushes!**

The tool contains a variety of different brushes:

* “Raise/lower“ brushes with an averaging setting.
    
* “Plateu“ brush allowing to flatten the terrain to a certain height
    
* “Smoothing“ brush to smooth out the peaks.
    
* “Foliage” brush with a setting allowing to place the foliage along the surface of the terrain.
    

**Camera Movement**

The tool contains a camera system allowing for easy navigation around the world.

---

# Camera Movement System

The first thing that I wanted to nail was proper camera navigation. I would briefly go over the camera movement system.

The translation controls are straight-forward.

First, we calculate the movement input from “WASD“ and then make our movement vector to be in relation to our current rotation.

```cpp
// Translation control
vec3 dir(0);

// Default WASD movement
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::W)) dir.z -= 1;
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::S)) dir.z += 1;
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::A)) dir.x -= 1;
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::D)) dir.x += 1;

dir = transform.GetRotation() * dir;
```

Then, we calculate the vertical movement from Q and E.

```cpp
// Vertical movement
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::Q)) dir.y -= 1;
if (Engine.Input().GetKeyboardKey(Input::KeyboardKey::E)) dir.y += 1;
```

Finally, we check if we did any movement. If we did, we move ourselves in accordance to the movement vector. We also apply extra speed if left shift is held.

```cpp
if (dir != vec3(0))
{
    // Shift for faster movement
    float speed = Engine.Input().GetKeyboardKey(Input::KeyboardKey::LeftShift) ? 20.f : 10.f;
    
    // Moving the camera
    transform.SetTranslation(transform.GetTranslation() + normalize(dir) * speed * dt);
}
```

The rotation control is also straight forward.

We retrieve the mouse position and check if the right mouse button is held and has moved.

```cpp
// Rotation control
const auto& mousePos = Engine.Input().GetMousePosition();

if (Engine.Input().GetMouseButton(Input::MouseButton::Right) && mousePos != control.m_mousePos)
{
    // ... Further movement code.
}

// We save the last mouse position in the end for the next iteration.
control.m_mousePos = mousePos; 
```

We calculate the mouse movement by checking the difference between our current position and the last position. We also apply an arbitrary mosue sensitivity value.

```cpp
const float sensitivity = 0.003f;
const auto& dMouseScaled = (mousePos - control.m_mousePos) * sensitivity;
```

We calculate the horizontal (yaw) and vertical (pitch) rotation based on the mouse’s X and Y movement (`dMouseScaled`)

```cpp
quat initial(transform.GetRotation());

quat yawQuat = angleAxis(-dMouseScaled.x, vec3(0.0f, 1.0f, 0.0f));
quat pitchQuat = angleAxis(-dMouseScaled.y, vec3(1.0f, 0.0f, 0.0f));
```

We then apply the rotation movement by multiplying it with our initial rotation.

```cpp
transform.SetRotation(normalize(yawQuat * initial * pitchQuat));
```

In the end, we have a camera system which can effectively move and rotate in 3D space.

---

# Terrain Rendering

## Theory: CPU vs GPU

The first thing I did was to look how other people have been editing the terrain

I first stumbled upon [Vladimir Chavdarov's blog](https://vladimirchavdarov.hashnode.dev/standalone-rts-map-maker-tool#heading-camera-movement) in which he explained his method of editing the terrain:

> In the beginning of the project, I decided that all mesh modifications will be made by editing the vertex data. This means that I avoided shader programming which might not have been the wisest decision, but it was a decision nonetheless.

One of the main flaws is that this method passes the entire terrain mesh every frame to the GPU for when the terrain is edited:

> It is important to notice that this puts noticeable strain on the app's performance and needs to be used as little as possible. After all, we reset the whole mesh every time we use a brush!

For my editor, I decided to render and displace the terrain height in the vertex shader on the GPU.

I also had plans to modify the terrain directly on the GPU, but I wasn’t able to do that due to time constraints. So we will be editing on the CPU where will be passing at most a 256×256 pixel image every frame instead of the whole mesh.

## Theory: Storing a Height Data in a Texture

One of the common ways to store height data in a texture is using all RGBA channels (grayscale) or using one color channel (such as red) such as an image below:

![image1](../../assets/images/Y2B/height_map_norway-height-map-aster-30m.png)

The main flaw that I noticed with this method is that the height is limited to values of the RGB channels of \[0, 256\], which means that there must be a set minimum and maximum height to map to.

Due to being limited from 0 to 255, this solution also produced 'jagged' terrain, which makes it look unappealing.

Another problem with this method is that if the minimum-maximum heights are set as large numbers (such as -128 to 128), the mountains increase too quickly when brushing. This makes it cumbersome to edit the terrain.

The minimum and maximum height also adds more 'settings' for the user to keep in mind, which are not really necessary. The user would want to increase the height limit if they wanted to make a higher mountain, which is not convenient.

For my editor, I decided to encrypt the height (a float) into RGBA which can be encoded and decoded. The main benefit of this method is that we store the whole floating number, which means that we can store more finer height data and we don’t need to set a minimum and maximum height.

```cpp
// Encode a float height into RGBA channels
void EncodeHeightToRGBA(float height, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
    // Reinterpret the float as a 32-bit integer for bit manipulation
    uint32_t heightBits = *reinterpret_cast<uint32_t*>(&height);

    // Extract 8 bits for each channel
    r = (heightBits >> 24) & 0xFF;  // Most significant 8 bits
    g = (heightBits >> 16) & 0xFF;  // Next 8 bits
    b = (heightBits >> 8) & 0xFF;   // Next 8 bits
    a = heightBits & 0xFF;          // Least significant 8 bits
}

// Decode RGBA channels back into a float height
float DecodeHeightFromRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // Combine the 8-bit channels into a 32-bit integer
    uint32_t heightBits = (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) |
                          (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);

    // Reinterpret the 32-bit integer as a float
    return *reinterpret_cast<float*>(&heightBits);
}
```

This function also exists in the vertex shader in order to sample the height from the given texture.

```cpp
float decodeHeightFromRGBA(vec4 rgba) 
{
    // Combine the channels into a 32-bit float value
    uint heightBits = (uint(rgba.r * 255.0) << 24) | // Red channel (most significant)
                      (uint(rgba.g * 255.0) << 16) | // Green channel
                      (uint(rgba.b * 255.0) << 8)  | // Blue channel
                      uint(rgba.a * 255.0);         // Alpha channel (least significant)

    // Reinterpret the 32-bit integer as a float
    return uintBitsToFloat(heightBits);
}
```

---

## Implementation

In order for the terrain mesh to render using a height map texture, we need to update a few things in the rendering pipeline.

First, we need to update BEE’s current Material class to include a height map texture.

```cpp
struct Material
{
    Material(const Model& model, int index);
    Material() = default;
    
    //... Other variables
    bool UseHeightMapTexture = false;

    //... Other textures
    std::shared_ptr<Texture> HeightMapTexture;
};
```

Then, in the Renderer class, when we apply the material to the mesh, we need to add 2 lines for it to send the height map texture to the GPU.

```cpp
void Renderer::ApplyMaterial(const std::shared_ptr<Material>& material)
{
    //... other code
    if (material->UseHeightMapTexture) internal::SetTexture(material->HeightMapTexture, HEIGHT_MAP_SAMPLER_LOCATION);
    
    //... other code
    m_forwardPass->GetParameter("use_height_map_texture")->SetValue(material->UseHeightMapTexture);

}
```

After this, we will go into shader programming, we will be editing the vertex shader and other GLSL files.

---

First, we need to add a height map sampler location in the shader. All we need to add is just this line in `locations.glsl`

```plaintext
// Samplers
#define BASE_COLOR_SAMPLER_LOCATION    0
#define NORMAL_SAMPLER_LOCATION        1
#define EMISSIVE_SAMPLER_LOCATION      2
#define ORM_SAMPLER_LOCATION           3
#define OCCLUSION_SAMPLER_LOCATION     4
#define DEPTH_SAMPLER_LOCATION         5
#define IRRADIANCE_LOCATION            6
#define LUT_SAMPER_LOCATION			   7
#define DIFFUSE_SAMPER_LOCATION		   8
#define SPECULAR_SAMPER_LOCATION	   9
#define SHADOWMAP_LOCATION			   10
#define HEIGHT_MAP_SAMPLER_LOCATION    16 // We add this line
```

Then, we need to change the vertex shader to account the new height map texture.

First, we will create a sampler2D where it will take in the height map and a boolean variable for whether to use the height map.

```cpp
layout(location = HEIGHT_MAP_SAMPLER_LOCATION) uniform sampler2D s_height_map;

uniform bool use_height_map_texture;
```

Then we sample the height from the image by decoding the RGBA into a float. We then adjust the position’s height.

```cpp
if(use_height_map_texture)
{
    vec2 uv = a_texture0;

    // Sample height map
    float center = decodeHeightFromRGBA(texture(s_height_map, uv));
    vec3 adjustedPosition = a_position + center * a_normal;
    
    //... other code
}
```

Since we don’t have precomputed normals from the mesh, we calculate them dynamically using the height map. This is important because normals are essential for accurate lighting.

This involves sampling the height differences in the surrounding pixels of the texture.

The difference in height values between the left and right (`heightL - heightR`) gives the slope in the X-axis (horizontal) direction. Similarly, the difference between the down and up heights (`heightD - heightU`) gives the slope in the Y-axis (vertical) direction.

```cpp
vec2 u_texel_size = vec2(1.0 / 256.0, 1.0 / 256.0);

// Calculating the surrounding UP-DOWN and LEFT-RIGHT height positions
float heightL = decodeHeightFromRGBA(texture(s_height_map, uv - vec2(u_texel_size.x, 0.0)));
float heightR = decodeHeightFromRGBA(texture(s_height_map, uv + vec2(u_texel_size.x, 0.0)));
float heightD = decodeHeightFromRGBA(texture(s_height_map, uv - vec2(0.0, u_texel_size.y)));
float heightU = decodeHeightFromRGBA(texture(s_height_map, uv + vec2(0.0, u_texel_size.y)));
```

Using the slopes, we construct a 3D normal vector. The X component is derived from the horizontal slope (`heightL - heightR`), and the Z component is derived from the vertical slope (`heightD - heightU`). The Y component is set to a constant (e.g., `2.0`) to scale the influence of height variations and maintain the balance of the normal vector's components.

```cpp
// Calculate normal vector
vec3 adjustedNormal = normalize(vec3(heightL - heightR, 2.0f, heightD - heightU));
```

We finish our vertex shader by passing in the adjusted normal and height data to the fragment shader.

```cpp
mat4 world = bee_transforms[gl_InstanceID].world;
mat4 wv = bee_view * world;
v_position = (world * vec4(adjustedPosition, 1.0)).xyz; // We pass our adjusted position here
v_normal = normalize((world * vec4(adjustedNormal, 0.0)).xyz); // We pass our adjusted normal here
v_tangent = normalize((world * vec4(a_tangent.xyz, 0.0)).xyz);
v_texture0 = a_texture0;
v_texture1 = a_texture1;
mat4 wvp = bee_transforms[gl_InstanceID].wvp;
gl_Position = wvp * vec4(adjustedPosition, 1.0); // We pass our adjusted position here
```

This concludes all the changes needed for rendering.

---

# Brushes

<a class="cta-link" href="https://youtu.be/rWUH2SRUBds" target="_blank" rel="noopener">▶ Watch Brush Showcase</a>


We will go step-by-step from the input function to the brush function.

### Input Function

To interact with the terrain, we need to take in input from the mouse and the keyboard.

First, we check the keyboard input. We check the brush size keyboard shortcuts created for ease of use.

```cpp
// A keyboard shortcut for increasing/decreasing the brush size. 
if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::LeftBracket)) brushSize -= 1;
if (Engine.Input().GetKeyboardKeyOnce(Input::KeyboardKey::RightBracket)) brushSize += 1;
```

After this, we check the mouse input. We begin by checking if our mouse is in the viewport at all.

```cpp
 // An early out in case the mouse is not in the viewport.
 if (!GearInternal::IsMouseInViewport()) return;
```

After we ensured that our mouse is in the viewport, we calculate a direction ray that originates from the camera and points towards a specific point in the world based on a 2D screen position.

```cpp
glm::vec3 GetRayFromScreenToWorld(Camera camera, Transform transform)
{
    // We get the mouse position in the viewport ranging from (0, 0) to (1, 1)
    glm::vec2 mousePosition = Engine.Input().GetMousePositionInViewport();
    
    // Calculate the inverse transformation matrix. This allows you to map from screen space to world space.
    const glm::mat4& inv = transform.World() * inverse(camera.Projection);

    // Calculate the near and far planes.
    // Using the inverse matrix, we map the near and far clip points into world space.
    const glm::vec4& n = inv * glm::vec4(mousePosition.x, mousePosition.y, -1, 1);
    const glm::vec4& f = inv * glm::vec4(mousePosition.x, mousePosition.y, 1, 1);
    
    // Calculate the final ray direction. 
    // The ray direction is the vector from the near world point to the far world point (fWorld)
    const glm::vec3& dir = glm::normalize(glm::vec3(f / f.w) - glm::vec3(n / n.w));

    return dir;
}

// We find the direction ray of what the camera is looking at.
glm::vec3 direction = GearInternal::GetRayFromScreenToWorld(camera, transform);
```

Then, we take our direction ray and check if it is intersecting the origin plane. We are working with the assumption that the terrain object is at origin height and is facing upwards, since we do not expect the terrain to be something that is moved vertically or rotated in any way.

From here, we get where in the world our brush is located.

```cpp
bool IsIntersectingOriginPlane(const glm::vec3& position, const glm::vec3& direction, glm::vec3& intersectionPoint)
{
    // Assumption that all terrain planes are facing up.
    float denominator = glm::dot(glm::vec3(0, 1.0f, 0), direction);

    if (glm::abs(denominator) < 1e-6f) return false;  // Ray is parallel to plane

    // Compute the numerator
    float t = glm::dot(glm::vec3(0, 1.0f, 0), glm::vec3(0.0f) - position) / denominator;

    if (t < 0) return false;  // the plane is behind the camera

    // Compute the intersection point
    intersectionPoint = position + t * direction;
    return true;
}

glm::vec3 intersectionPoint = glm::vec3(0.0f);

// We check if the ray is intersecting the origin plane. We early out if there is none.
if (!GearInternal::IsIntersectingOriginPlane(transform.GetTranslation(), direction, intersectionPoint)) continue;

// We set the brush position to the intersection point. This is later used for drawing and also displaying the brush circle.
brushPosition = intersectionPoint;
```

After this, we check if the left mouse button is pressed. If it is, we pass our brush position to our draw function.

```cpp
// We draw when we pressed the left mouse button.
if (Engine.Input().GetMouseButton(Input::MouseButton::Left))
{
    Draw(brushPosition, dt);
}
```

### Draw Function

After we know where the brush is, we can begin drawing.

First, we get the brush’s UV position in relation to the plane.

```cpp
glm::vec2 GetUVCoordinate(glm::vec3 targetPosition, glm::vec3 planePosition, int width, int length)
{
    // Define the plane's local axes
    glm::vec3 uAxis = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 vAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    // Transform intersection point to plane's local space
    glm::vec3 localPoint = targetPosition - planePosition;

    // Project the local point onto the plane's axes
    float uCoord = glm::dot(localPoint, uAxis);
    float vCoord = glm::dot(localPoint, vAxis);

    glm::vec2 uv;

    // Convert to UV coordinates (normalized to [0, 1])
    uv.x = (uCoord / width) + 0.5f;   // Map from [-width/2, width/2] to [0, 1]
    uv.y = (vCoord / length) + 0.5f;  // Map from [-height/2, height/2] to [0, 1]

    return uv;
}

glm::vec2 uv = GearInternal::GetUVCoordinate(drawPosition, transform.GetTranslation(), width, length);
```

Then, we check if the point is on the plane.

```cpp
// Is the UV point on the plane?
if (!(uv.x > 0.0f && uv.x < 1.0f && uv.y > 0.0f && uv.y < 1.0f)) continue;
```

After we verify that our brush position is on the plane, we begin calculating and extracting the region the brush will modify.

We calculate the pixel coordinates from our UV position and then calculate the square region based on the brush’s size.

```cpp
// Convert UV to pixel coordinates
int centerX = static_cast<int>(uv.x * width);
int centerY = static_cast<int>(uv.y * length);

// Calculate the bounds of the square
int halfSize = brushSize / 2;
int startX = std::clamp(centerX - halfSize, 0, width - 1);
int endX = std::clamp(centerX + halfSize, 0, width - 1);
int startY = std::clamp(centerY - halfSize, 0, length - 1);
int endY = std::clamp(centerY + halfSize, 0, length - 1);

// Prepare a buffer for the square region
int regionWidth = endX - startX + 1;
int regionHeight = endY - startY + 1;
```

Then, we get the full texture data from the GPU.

```cpp
auto& image = renderer.Material->HeightMapTexture->Image;
glBindTexture(GL_TEXTURE_2D, image->GetTextureId());
std::vector<unsigned char> fullTextureData(width * length * 4);  // RGBA8 format
glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, fullTextureData.data());
```

After that, we extract the region data from the full texture data. This allows us only to focus only on what is directly under the brush.

```cpp
std::vector<unsigned char> regionData(regionWidth * regionHeight * 4);

for (int y = 0; y < regionHeight; ++y)
{
    for (int x = 0; x < regionWidth; ++x)
    {
        int fullIndex = ((startY + y) * width + (startX + x)) * 4;  // Full texture index
        int regionIndex = (y * regionWidth + x) * 4;                // Region index
        regionData[regionIndex] = fullTextureData[fullIndex];
        regionData[regionIndex + 1] = fullTextureData[fullIndex + 1];
        regionData[regionIndex + 2] = fullTextureData[fullIndex + 2];
        regionData[regionIndex + 3] = fullTextureData[fullIndex + 3];
    }
}
```

After this, we have all the data we need to begin brushing: the brush position and the texture data.

In here, we have a switch case for which brush to use, after which we pass in the necessary data for it.

```cpp
switch (currentBrushState)
{
    case BrushState::FOLIAGE:
        Foliage(centerX, centerY, regionData);
        break;
    case BrushState::SMOOTH:
        Smooth(centerX, centerY, dt, regionData);
        break;
    case BrushState::PLATEU:
        Plateu(centerX, centerY, regionData);
        break;
    default:
        GaussianDistribution(centerX, centerY, dt, regionData);
        break;
}
```

Once all of the brushwork is completed, we upload the new texture data back to the GPU.

```cpp
// Upload the modified square back to the texture
glTexSubImage2D(GL_TEXTURE_2D,
                0,
                startX,
                startY,
                regionWidth,
                regionHeight,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                regionData.data()
);

// Unbinding the texture once everything is complete.
glBindTexture(GL_TEXTURE_2D, 0);
```

From here, we will take a closer look at each brush.

### Raise and Lower Brush

![image1](../../assets/images/Y2B/brush_raise.gif)

![image1](../../assets/images/Y2B/brush_lower.gif)


Raises and lowers based on Gaussian Distribution.

First, since we are working with a square region, we want to make sure that the height point is within the brush circle.

```cpp
for (int y = 0; y < regionHeight; ++y)
{
    for (int x = 0; x < regionWidth; ++x)
    {
        // Calculate the distance from the center of the circle
        int pixelX = startX + x;
        int pixelY = startY + y;
        float dx = static_cast<float>(pixelX - centerX);
        float dy = static_cast<float>(pixelY - centerY);
        float distanceSq = dx * dx + dy * dy;

        // Check if the pixel is within the circle radius
        if (distanceSq <= halfSize * halfSize)
        {
            //... Code for modifying the height
        }
    }
}
```

If our point is within the circle, we can begin the height modification.

One of the settings that I added to the brush is the ‘averaging‘ setting. We check this setting only if the user has toggled the option in the menu.

This only affect grid points which are lower or equal to the average height of all points hovered by the brush. This is helpful if you're aiming to create a mountain-like shape and wish to add some detail to its base.

```cpp
// We calculate the average height in the region.
if (isAverageTerrainOn)
{
    averageHeight = CalculateAverageHeightInTerrain(regionWidth, regionHeight, regionData);
}

// ... Other code.

// We only affect the height points where the: 
// - current height is lower than/equal to average height AND we are in a raising state
// - current height is higher than/equal to average height AND we are in a lowering state
if (isAverageTerrainOn && ( (!(height <= averageHeight) && currentBrushState == BrushState::RAISE) || (!(height >= averageHeight) && currentBrushState == BrushState::LOWER)))
{                
    continue;
}
```

Then, we finally modify the height point in accordance to the Gaussian weight.

```cpp
// Calculate indices
int index = (y * regionWidth + x) * 4;
float height = GearInternal::DecodeHeightFromRGBA(regionData[index], regionData[index + 1], regionData[index + 2], [index + 3]);
              
// Apply Gaussian distribution
float gaussianWeight = exp(-distanceSq / twoSigmaSq);
height += brushIntensity * gaussianWeight * dt;

GearInternal::EncodeHeightToRGBA(height, regionData[index], regionData[index + 1], regionData[index + 2], regionData[index + 3]);  
```

Most of the other brushes follow more-less similar code, so we will only focus on the main points of difference for the sake of brevity.

### Plateu Brush

![image1](../../assets/images/Y2B/brush_plateau.gif)


The plateu brush calculates the center height in the middle of the brush and applies it to all other height points in the region.

```cpp
float height = GearInternal::DecodeHeightFromRGBA(regionData[index],regionData[index + 1], regionData[index + 2], regionData[index + 3]);

height = centerHeight;

GearInternal::EncodeHeightToRGBA(height, regionData[index], regionData[index + 1], regionData[index + 2], regionData[index + 3]);
```

### Smoothing Brush

![image1](../../assets/images/Y2B/brush_smooth.gif)


The smoothing brush works by evening out the terrain heights in the area it touches. We calculate the average terrain height and ‘nudge’ our the current height point closer to that average.

```cpp
// Calculating the average height in the terrain.
float averageHeight = CalculateAverageHeightInTerrain(regionWidth, regionHeight, regionData);

// ... other code

float height = GearInternal::DecodeHeightFromRGBA(regionData[index],regionData[index + 1], regionData[index + 2], regionData[index + 3]);

float difference = averageHeight - height;
height += difference * brushIntensity * dt;

GearInternal::EncodeHeightToRGBA(height, regionData[index], regionData[index + 1], regionData[index + 2], regionData[index + 3]);
```

### Foliage Brush

![image1](../../assets/images/Y2B/brush_foliage.gif)


The last brush is the foliage brush. It doesn’t modify the terrain, it places an object on the terrain. In my demo, I simply create a tree at the given position, but the object itself is arbitrary and can be any other 3D model.

```cpp

float height = GearInternal::DecodeHeightFromRGBA(regionData[index],regionData[index + 1], regionData[index + 2], regionData[index + 3]);

glm::vec3 position = glm::vec3(pixelX - width / 2, height, pixelY - length / 2);
glm::quat rotation = glm::quat(1, 0, 0, 0); // By default, we apply no rotation

// ... code calculations for when we want to place along the normals.

CreateTree(position, rotation);
```

The brush also contains a setting to place the objects along the normal of the terrain. This setting would be important for things that need to be perpendicular to the terrain surface such as fences or grass.

We will be using the same principle used in the vertex shader when calculating the normal.

```cpp
if (isPlacingAlongNormalsOn)
{
    // Compute height differences for normals
    float heightL = x - 1 > 0 ? GearInternal::DecodeHeightFromRGBA(regionData[((y * regionWidth + (x - 1)) * 4) + 0], regionData[((y * regionWidth + (x - 1)) * 4) + 1], regionData[((y * regionWidth + (x - 1)) * 4) + 2], regionData[((y * regionWidth + (x - 1)) * 4) + 3]) : height;
    float heightR = x + 1 < regionWidth - 1 ? GearInternal::DecodeHeightFromRGBA(regionData[((y * regionWidth + (x + 1)) * 4) + 0], regionData[((y * regionWidth + (x + 1)) * 4) + 1], regionData[((y * regionWidth + (x + 1)) * 4) + 2], regionData[((y * regionWidth + (x + 1)) * 4) + 3]) : height;
    float heightD = y - 1 > 0 ? GearInternal::DecodeHeightFromRGBA(regionData[(((y - 1) * regionWidth + x) * 4) + 0], regionData[(((y - 1) * regionWidth + x) * 4) + 1], regionData[(((y - 1) * regionWidth + x) * 4) + 2], regionData[(((y - 1) * regionWidth + x) * 4) + 3]) : height;
    float heightU = y + 1 < regionHeight - 1 ? GearInternal::DecodeHeightFromRGBA(regionData[(((y + 1) * regionWidth + x) * 4) + 0], regionData[(((y + 1) * regionWidth + x) * 4) + 1], regionData[(((y + 1) * regionWidth + x) * 4) + 2],  regionData[(((y + 1) * regionWidth + x) * 4) + 3]) : height;

    // Calculate normal vector
    glm::vec3 normal = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));

    // Calculate rotation quaternion to align object with the normal
    rotation = glm::rotation(glm::vec3(0, 1, 0), normal);
}
```

---

# Closing Thoughts

Overall, I’m happy with how the project turned out. I managed to gain valuable skills and most importantly face and complete a challenging project.