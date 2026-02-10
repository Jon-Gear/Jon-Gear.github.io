---
layout: project
title: "Fire Ant Engine: Custom C++ Strategy Game Engine"
role: "Engine Programmer"

date: 2025-04-11
version: long
short_url: /projects/year-2-block-c-short
long_url: /projects/year-2-block-c-long

categories: [featured_projects]
image: /assets/images/Y2C/main_large.gif
description: "Fire Ant Engine is a continuation of my custom Terrain Editor. The team was joined by seven talented programmers. The project features a prefab system, a terrain editor, flow-fields for AI agent navigation, an in-engine UI editor, optimizations which allowed a support for a large terrain map and many units, and support on Windows and Nintendo Switch."
contributions: "Terrain Editor | Terrain Rendering | Height Editing | Texture Editing | Prop Placement | Scene Serialization | Prefabs"
tools: "C++, OpenGL, GLSL (Compute & Tesselation Shaders), EnTT, Cereal, ImGui"
team_size: "7 Programmers"
platforms: "Windows | Nintendo Switch"
duration: "8 Weeks (Feb 2025 - Apr 2025)"
---

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/fireant" target="_blank" rel="noopener">⚙️ See Code Examples</a>


### Contribution 1: Terrain Editor

_Contributions have variation and are techincally challenging_

Description: 

The terrain editor is a panel which provides the user a set of tools to edit the terrain's features such as height and texture.

Defintion of Done:
- The editor manages an array of brushes (created using a common 'Base Terrain Brush Class'), keeping track of and updating the current brush.
- The editor has a GUI which allows to select a brush and choose settings.
- The editor can edit a visible rendered terrain such as its height map and albedo textures. _(See Contribution 1-0)_
- The editor has a 'Height Brush' _(See Contribution 1-1)_
- The editor has a 'Texture Brush' _(See Contribution 1-2)_
- The editor has a 'Prop Brush' _(See Contribution 1-3)_
- The editor has a 'Plateau Brush' _(See Contribution 1-4)_

**Implementation:**

_Explain the concepts_

First thing I did was to import and internally restructure the Terrain Editor from Block B. The terrain editor is responsible for managing the tools (brushes) which includes keeping track of the currently selected tool and updating it. 

```cpp
class TerrainEditor
{
public:
    // These functions update the currently selected brush
    void Update(float) override;
    void Render() override;
    void OnPanel() override;

    // ... Other functions

    // We now keep an array of brushes of TerrainBrush.
    std::vector<std::unique_ptr<TerrainBrush>> m_brushes;
    int currentBrushIndex = 0;
};
```

For ease of use, I created a base TerrainBrush interface class. It includes some of the basic interface functions such as keyboard input, the ability to draw, and UI panel functions. 

This is important because it allows to scale for newer brushes to be added in the game and to be handled easily by the Terrain Editor. The terrain editor does not know exact implementation of the brushes, it simply launches the common function defined by the base class. We can always inherit common functionality which is to be shared for future brushes.

```cpp
// Interface for terrain brushes.
class TerrainBrush
{
    // Interface of some functions
    virtual void Input() = 0;
    virtual void Draw() = 0;
    virtual void OnPanel() = 0; 
}

class HeightBrush : public TerrainBrush
{
    HeightBrush() : TerrainBrush("Height Brush") {}

    // Implementation of functions.
    void Input() override;
    void Draw() override;
    void OnPanel() override;
}
```

------

Along with the Terrain Editor, it comes with some helper functions. I won't cover all of them, but I will name most notable ones.

First one is caching the terrain, namely the entity and the terrain height map data.

```
namespace bee::terrain_tools
{
entt::entity cachedTerrainEntity = entt::null;

std::vector<unsigned char> cachedTerrainHeightMapData;
};  // namespace bee::terrain_tools
```

This is important because a lot of helper functions rely on knowing the terrain height map data and are called frequently. The simplest example is sampling the height at a position. Instead of pulling the height map data every time from the GPU, we simply cache it on the CPU. 

Second one is terrain intersection. This was done with the help of Lucas Algera with his Block B terrain project which featured terrain raymarching. What it does is marches through the terrain with a given origin and direction, sampling the height, until it finds an intersection.

```
bool terrain_tools::FindIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& hit)
{
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
```

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of the terrain editor in the engine:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1.mp4" type="video/mp4">
</video>

------
### Contribution 1-0: Terrain Rendering

_Contributions have variation and are techincally challenging_

Description: 

The terrain is a 3D object which is rendered in the game with an albedo texture and is displaced according to a given height map, creating contour.

Defintion of Done:
- The terrain is rendered in the game.
- The terrain uses a separate rendering pass. Other objects (e.g. standard 3D models) are not to be affected.
- The terrain uses a separate material which includes a height map. Other objects are not to be affected.
- The terrain is displaced by a given height map texture.


**Implementation:**

_Explain the concepts_

In the beginning, BEE only had one rendering pass. This posed a major problem. We had a lot of object types which had different rendering logic from standard 3D models, such as terrain. To solve this, we created custom rendering passes which are called by the Renderer. They perform their own implementation-specific code (shaders, materials, mesh renderers).

```cpp
void Renderer::Render()
{
    // Other calculations, including filling in the m_passData to pass to the other rendering passes.
    m_terrainPass->Draw();
    m_modelPass->Draw();
    // Other calculations
}
```

I also created a custom TerrainMeshRenderer and TerrainMaterial as seen below.

```cpp

// TerrainMaterial inherits from Material and adds a new HeightMapTexture
struct TerrainMaterial : ModelMaterial
{
    TerrainMaterial() = default;
    bool UseHeightMap = true;
    std::shared_ptr<Texture> HeightMapTexture;
}

// TerrainMaterial is similar in structure to MeshRenderer, with the exception of using TerrainMaterial instead.
struct TerrainMeshRenderer
{
    std::shared_ptr<bee::Mesh> Mesh;
    std::shared_ptr<bee::TerrainMaterial> Material;
};
```

***

Our terrain has the following OpenGL Rendering Pipeline:
- Vertex Shader
- Tessellation Shader (Tessellation Control Shader + Tessellation Evaluation Shader)
- Fragment Shader

In the vertex shader, we simply pass the data towards the tessellation shader.

In the tessellation shader, we displace the vertice based on the height map. Apart from this, I do not claim the work for the tessellation shader, credit goes to Bogdan.

```glsl
float decodeHeightFromRGBA(vec4 rgba) 
{
    // Combine the channels into a 32-bit float value
    uint heightBits = (uint(rgba.r * 255.0) << 24) | // Red channel (most significant)
                      (uint(rgba.g * 255.0) << 16) | // Green channel
                      (uint(rgba.b * 255.0) << 8)  | // Blue channel
                      uint(rgba.a * 255.0);          // Alpha channel (least significant)

    // Reinterpret the 32-bit integer as a float
    return uintBitsToFloat(heightBits);
}

...

float center = decodeHeightFromRGBA(texture(s_height_map, te_texture0));
```

In the fragment shader, the major difference from the standard 'uber.frag' is that we calculate our terrain normals differenty:

```glsl
vec3 calc_normal()
{   
    float left  = decodeHeightFromRGBA(textureOffset(s_height_map, v_texture0, ivec2(-1, 0 )));
    float right = decodeHeightFromRGBA(textureOffset(s_height_map, v_texture0, ivec2( 1, 0 )));
    float up    = decodeHeightFromRGBA(textureOffset(s_height_map, v_texture0, ivec2( 0, 1 )));
    float down  = decodeHeightFromRGBA(textureOffset(s_height_map, v_texture0, ivec2( 0, -1)));

    vec3 normal = normalize(vec3(left - right, 2.0, up - down));
    
    return normal;
}
```

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of terrain rendering:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1-0.mp4" type="video/mp4">
</video>


------
### Contribution 1-1: Height Brush

_Contributions have variation and are techincally challenging_

Description: 

The height brush is a terrain editor tool which modifies the terrain's height map. It has general brush to set the brush diameter and intensity. It has two specific settings which allows the user to raise or lower the terrain. 

The height brush has 2 brush types: default circle brush and custom alpha. The default circle brush displaces the terrain in a circle area. The custom alpha brush displaces the terrain in a given brush alpha image.

Defintion of Done:
- The user can set brush parameters in the GUI such as diameter, intensity, and to raise/lower the terrain.
- The user can see visual feedback of where the brush will edit via a brush circle.
- The user can choose between a default circle and a custom alpha brush.
- The brush can modify the height map image based on the settings given.

**Implementation:**

_Explain the concepts_

Initially, from Block B, the Height Brush was implemented on the CPU. This decision was not based on an intelligent reason or preference, but rather due to the time and resource constraints of the previous block. 

It was then updated to use GLSL compute shaders, as I intended from Block B. This change allowed to update the image on GPU, much more effectively than on CPU.

The switch to compute shaders is important because it allows for future scalability. On the CPU, the newer brushes would be clunkier to edit, whereas compute shaders are easier to edit, to understand, and to implement. An evidence to this is that I was able to easily add circle and custom alpha brushes quite easily to all brushes, whereas on the CPU that would have been a monumental task.

First, on CPU, we pass all of our brush data and launch the compute shader.

```cpp
// We pass our brush data to the terrain rendering pass. The radius is properly scaled based on the relationship between the terrain image size to the in-game terrain size. 

brushData.positionUV = terrain_tools::GetUVCoordinate(m_position);
brushData.radius = m_diameter / 2.0f * terrain_tools::GetTerrainHeightMapScale(); 
brushData.intensity = m_intensity * dt * (m_isRaising ? 1 : -1);

...

m_circleComputeShader->Activate();

// We pass in a terrain image ID which we will edit on the compute shader.
glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

// The work groups are defined by the diameter of the brush. It is also scaled based on the relationship between the terrain image size to the in-game terrain size.

const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainHeightMapScale());
glDispatchCompute(workGroupsX, workGroupsY, 1);

glMemoryBarrier(GL_ALL_BARRIER_BITS);

m_circleComputeShader->Deactivate();
```

We have two compute shaders which modify height: the one with the default circle and the one with the custom alpha.

The circle brush modifies the image in a circle. The reason why we do this and not simply have an alpha texture of a circle is that some precision is lost in an alpha texture, whereas with this method the circle is mathematically calculated. The image is then modified based on Gaussian weight.

```glsl

// texCoord is the current pixel being modified on the terrain image.
float dist = distance(vec2(texCoord), brushCenterPos);

if (dist < radius)
{
    float currentHeight = decodeHeightFromRGBA(imageLoad(terrain_image, texCoord));

    float sigma = radius / 2.0f;  // Standard deviation for the Gaussian function
    float twoSigmaSq = 2.0f * sigma * sigma;
    float gaussianWeight = exp(-(dist * dist) / (twoSigmaSq));

    float newHeight = currentHeight + intensity * gaussianWeight;

    imageStore(terrain_image, texCoord, encodeHeightToRGBA(newHeight));
}

```

The alpha brush modifies the image based on a given brush texture image, the effect of which is based on how bright a given pixel is. 


```glsl

float currentHeight = decodeHeightFromRGBA(imageLoad(terrain_image, texCoord));

// If a pixel is black, there is no effect. 
float brushValue = imageLoad(brush_texture_image, brushTexCoord).r;
float newHeight = currentHeight + intensity * brushValue;

imageStore(terrain_image, texCoord, encodeHeightToRGBA(newHeight));

```

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of the height brush:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1-1.mp4" type="video/mp4">
</video>

------
### Contribution 1-2: Texture Brush

_Contributions have variation and are techincally challenging_

Description: 

The texture brush is a terrain editor tool which modifies the terrain's base color texture. It has general brush to set the brush diameter and intensity. You can set the painting texture by dragging it from the asset browser.

The texture brush has 2 brush types: default circle brush and custom alpha. The default circle brush paints the terrain in a circle area. The custom alpha brush paints the terrain in a given brush alpha image.

Defintion of Done:
- The user can set brush parameters in the GUI such as diameter, intensity, and the ability to select a texture.
- The user can see visual feedback of where the brush will edit via a brush circle.
- The user can choose between a default circle and a custom alpha brush.
- The brush can modify the terrain's base color texture based on the settings given.

**Implementation:**

_Explain the concepts_

The texture brush is similar to the height brush in terms of implementation.

```cpp
m_circleComputeShader->Activate();

glBindImageTexture(0, terrain_image_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

// We additionally pass our brush terrain texture set by the user.
glBindImageTexture(3, brush_terrain_texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

// We also scale according to the terrain albedo texture instead.
const int workGroupsX = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
const int workGroupsY = static_cast<int>(m_diameter * terrain_tools::GetTerrainTextureScale());
glDispatchCompute(workGroupsX, workGroupsY, 1);
glMemoryBarrier(GL_ALL_BARRIER_BITS);

m_circleComputeShader->Deactivate();
```

In the compute shader, we sample the brush terrain texture to color a given pixel. The alpha texture compute shader is much of the same, except without the radius check.

```glsl
vec4 currentColor = imageLoad(terrain_image, texCoord);

// We tile-wrap the brush terrain texture based on its size.
vec4 brushTextureColor = vec4(1.0, 0.0, 0.0, 1.0);
{
    ivec2 imageSize = imageSize(brush_terrain_texture_image);

    // Wrap texCoord using modulo to ensure it always falls within [0, imageSize)
    ivec2 wrappedCoord = ivec2(texCoord.x % imageSize.x, texCoord.y % imageSize.y);

    // Handle negative coordinates properly by adding imageSize
    if (wrappedCoord.x < 0) wrappedCoord.x += imageSize.x;
    if (wrappedCoord.y < 0) wrappedCoord.y += imageSize.y;

    brushTextureColor = imageLoad(brush_terrain_texture_image, wrappedCoord);
}

float dist = distance(vec2(texCoord), brushCenterPos);
if (dist < radius)
{
    // We mix the current and new color based on set intensity
    vec4 newColor = mix(currentColor, brushTextureColor, intensity);
    imageStore(terrain_image, texCoord, newColor);
}
```

_Show relevant code snippets, algorithms, data structures, etc_

_Show that the contribution is robust, complete, and scalable_

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of the texture brush:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1-2.mp4" type="video/mp4">
</video>

------
### Contribution 1-3: Prop Brush

_Contributions have variation and are techincally challenging_

Description: 

The prop brush is a terrain editor tool which adds props along the contour of the terrain. It has general brush to set the brush diameter and intensity (in this case, it is the amount of props to place per stroke). It has a setting of whether to place the objects along the normal of the terrain. The user can set which prop to place in the inspector. 

Defintion of Done:
- The user can set brush parameters in the GUI such as diameter, props per stroke, whether to place along the normals, and the ability to select a prop to place.
- The user can see visual feedback of where the brush will edit via a brush circle.
- The brush can place the props along the contour of the terrain.
 
**Implementation:**

_Explain the concepts_

By itself, the prop brush's effectiveness is dependent on two features: the asset browser and the saving/loading system. 

For example, you can drag a prop from the asset browser which would save the prop's file path. 

```cpp
ImGui::Button("Drop Target", ImVec2(200, 50));
if (ImGui::BeginDragDropTarget())
{
    if (const ImGuiPayload * assetPayload{ImGui::AcceptDragDropPayload("ASSET")})
    {
        const auto* data{static_cast<const char*>(assetPayload->Data)};

        auto relativePath{Engine.FileIO().GetRelativeAssetsPath(data)};

        m_propPath = relativePath;
    }
    ImGui::EndDragDropTarget();
}
```

Then, we load the prop upon draw. We set it to a random position with our given brush area circle, set a random vertical rotation, and if the user has set to place along normals, we will rotate it accordingly.

```cpp
auto propEntity = ecs.CreateEntity();
auto& propTransform = ecs.CreateComponent<Transform>(propEntity);
auto& terrainEntity = terrain_tools::GetCachedTerrainEntity();

// Translation
glm::vec3 translationVariation = bee::rng::RandomPointInCircle(m_diameter / 2.0f);
glm::vec3 propTranslation = m_position + translationVariation;
propTranslation.y = terrain_tools::GetTerrainHeight(propTranslation).second;

// Rotation
glm::quat rotationVariation =
    glm::angleAxis(bee::rng::GetRandomFloat(0.0f, glm::two_pi<float>()), glm::vec3(0.0f, 1.0f, 0.0f));

glm::quat propRotation =
    m_placeAlongNormals ? terrain_tools::GetTerrainNormal(propTranslation).second : glm::quat(1, 0, 0, 0);

propRotation *= rotationVariation;

// Setting the translation and rotation

propTransform.SetParent(terrainEntity);
propTransform.SetTranslation(propTranslation);
propTransform.SetRotation(propRotation);

// Load the model using the relative path
const auto model{Engine.Resources().Load<bee::Model>(bee::FileIO::Directory::Assets, m_propPath)};
model->Instantiate(propEntity);
```

Later, the props can be saved in a scene. 

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of the prop brush:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1-3.mp4" type="video/mp4">
</video>

------
### Contribution 1-4: Plateau Brush

_Contributions have variation and are techincally challenging_

Description: 

The plateau brush is a terrain editor tool which allows the user to set a plateau of a specific height. It has general brush to set the brush diameter and intensity. It has a setting to set the target plateau height. The user can also choose a target height by sample-clicking the terrain. 

The plateau brush has 2 brush types: default circle brush and custom alpha. The default circle brush displaces the terrain in a circle area. The custom alpha brush displaces the terrain in a given brush alpha image.

Defintion of Done:
- The user can set brush parameters in the GUI such as diameter, intensity, and the target height.
- The user can see visual feedback of where the brush will edit via a brush circle.
- The user can choose between a default circle and a custom alpha brush.
- The user can choose a height by sampling the terrain.
- The brush will edit the terrain and set the height towards the plateau height.
 
**Implementation:**

_Explain the concepts_

The plateau brush is similar in implementation to the height brush.

The plateau brush is able to set the target height manually, but also by sampling the terrain as seen below.

```cpp

if (m_selectTargetHeightFromTerrain)
{
    const auto result = terrain_tools::GetTerrainHeight(m_position);

    if (result.first)
    {
        m_targetHeight = result.second;
    }
    m_selectTargetHeightFromTerrain = false;
    return;
}

```

We also pass an additional parameter in the brush data, target height.

```cpp
BrushData brushData;

brushData.positionUV = terrain_tools::GetUVCoordinate(m_position);
brushData.radius = m_diameter / 2.0f * terrain_tools::GetTerrainHeightMapScale();
brushData.intensity = m_intensity * dt;
brushData.targetHeight = m_targetHeight; 
```

In the circle compute shader, we apply a gaussian weight and smoothstep which would create smooth edges around the brush. 

```glsl
float dist = distance(vec2(texCoord), brushCenterPos);

if (dist < radius)
{
    float currentHeight = decodeHeightFromRGBA(imageLoad(terrain_image, texCoord));

    float sigma = radius / 2.0f;  // Standard deviation for the Gaussian function
    float twoSigmaSq = 2.0f * sigma * sigma;
    float gaussianWeight = exp(-(dist * dist) / (twoSigmaSq));

    float gradientFactor = smoothstep(radius * 0.5, radius, dist);
    float adjustedWeight = gaussianWeight * (1.0 - gradientFactor);

    float newHeight = currentHeight + (targetHeight - currentHeight) * intensity * adjustedWeight;

    imageStore(terrain_image, texCoord, encodeHeightToRGBA(newHeight));
}
```

In the alpha compute shader, we apply by brush alpha value.

```glsl
float currentHeight = decodeHeightFromRGBA(imageLoad(terrain_image, texCoord));

float brushValue = imageLoad(brush_texture_image, brushTexCoord).r;
float newHeight = currentHeight + (targetHeight - currentHeight) * intensity * brushValue;

imageStore(terrain_image, texCoord, encodeHeightToRGBA(newHeight));
```

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of the plateau brush

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C1-4.mp4" type="video/mp4">
</video>

------
### Contribution 2: Scene Saving and Loading 

_Contributions have variation and are techincally challenging_

Description: 

The user can save a given scene, which preserves the scene's transform hierarchy order, components (and their settings). The saved scene is saved as a 'JSON' file. This file can later be loaded back into the game.

Defintion of Done:
- The tool can save the scene's transform hierarchy order (which entities are parents/children of other entities).
- The tool can save the entity's components and their data (which components belong to which entities).
- The tool can load the scene's transform hierarchy order.
- The tool can load the entity's components and their data.

**Implementation:**

_Explain the concepts_

For this project, I have decided to use 'Cereal' library for serialization. 

My first task was to figure out how to correctly save and load the scene's hierarchy.

In order to save the transform hierarchy order, I thought of a step-through method using recursion. The scene hierarchy consists of nodes where each node can have children, like below.

```
Root
├── Node A
│   ├── Node A1
│   └── Node A2
│       └── Node A2a
├── Node B
│   └── Node B1
└── Node C
    ├── Node C1
    └── Node C2
```

Using recursion, we can visit every node. We can then record the node by marking it with an identifier and keeping track of their parent-child relationship. After this, we record the amount of nodes.

At the time of implementation, the identifier was a counter and we kept track of the parent-child relationship via a children count and the order of nodes. The code has been updated to use keys for nodes (entity name + entity ID) and their parents (parent entity name + parent entity ID) instead, but the logic is mostly same.


| Logic                           | My Initial Implementation | Current Implementation |
| -------------------------------------- | ------------------------ | ----------------------------------------------------- |
| Node Identifiers                       | A counter                | A key (entityName + entityID)                          |
| Parent-Child Relationship Tracking     | A children count + specific node order | A parent key (parentEntityName + parentEntityID)       |


So based on the example above:
- 'Root' has the ID of 1 and has 3 children
- 'Node A' has the ID of 2 and has 2 children
- 'Node A1' has the ID of 3 and has 0 children
- ...
- 'Node C2' has the ID of 11 and has 0 children

Because we have labelled the nodes and know their parent-child relationships, we can rebuild the hierarchy. 

***

My second task was to figure out how to correctly save and load the components of each entity in the hierarchy.

First, I needed to serialize the components themselves. As an example, here is how a Transform is serialized. 

All of GLM's data structures such as vec3 were also serialized. 

```cpp
template <class Archive>
void Transform::serialize(Archive& ar)
{
    // Here, we serialize the data itself.
    ar(cereal::make_nvp("Name", Name),
       cereal::make_nvp("Translation", m_translation),
       cereal::make_nvp("Scale", m_scale),
       cereal::make_nvp("Rotation", m_rotation));
}

// Explicit template instantiations for the types you intend to use
template void Transform::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive&);
template void Transform::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive&);

void bee::Transform::Save(cereal::JSONOutputArchive& archive, const entt::entity)
{
    // In here, we can perform any pre-save logic.
    archive(cereal::make_nvp("Transform", *this));
}

void bee::Transform::Load(cereal::JSONInputArchive& archive, const entt::entity)
{
    archive(cereal::make_nvp("Transform", *this));
    // In here, we can perform any post-load logic.
}
```

Then, I implemented a runtime reflection system using EnTT's Meta. 

First, we register all components in this function which is called upon engine initialization. This way, we have a list of components to choose from on runtime.

```cpp
void bee::scene_serialization::RegisterAllComponents()
{
    // Register all of the components here! Make sure they have proper 'cereal' serialize and deserialize functions.
    internal::RegisterComponentForSerialization<Transform>();
    // Other components...
}

template <typename Type>
[[maybe_unused]] void internal::RegisterComponentForSerialization()
{
    // Here, we register the type itself and two functions which would allow us to load and save the type.
    entt::meta_factory<Type>()
        .type(entt::type_hash<Type>::value())
        .template func<&internal::SaveType<Type>>("SaveType"_hs)
        .template func<&internal::LoadType<Type>>("LoadType"_hs);
}
```

The LoadType and SaveType take care of creating the component within the EnTT system and calling the Save and Load function of the type.

```cpp
template <typename Type>
void internal::SaveType(cereal::JSONOutputArchive& archive, const Entity entity)
{
    Type& component = Engine.ECS().Registry.get<Type>(entity);

    component.Save(archive, entity);
}

template <typename Type>
void internal::LoadType(cereal::JSONInputArchive& archive, const Entity entity)
{
    Type& component = Engine.ECS().CreateComponent<Type>(entity);

    try
    {
        component.Load(archive, entity);
    }
    catch (...)
    {
        Engine.ECS().Registry.remove<Type>(entity);
    }
}
```

All of this allows us to save and load back in components. My code became a foundation for saving/loading the scene. My teammates later on updated the code to have more data in it.

**Presence in Sprint Demo:**

_Show videos/images of the contribution in sprint demo_

Here is a video of saving and loading:

<video width="640" height="480" controls>
  <source src="/assets/images/Y2C/C2.mp4" type="video/mp4">
</video>


# Your Contributions' Impact on the Project

_Evaluate your impact on the project_

My contributions to the project are an inseparable part of the engine. They have been pushing the boundaries of the project.

The terrain is quite literally the ground the whole game is standing on. Without it, the game would not have any variations in terms of elevation or texture. It will not be the same at all without it. You wouldn't have any mountains, valleys, beaches, plains, or even any of the trees or rocks in the demos without this gigantic contribution.

The saving and loading of the scene is solely responsible for our ability to even build levels in the first place. This contribution allowed us to have different demos, being able to build scenes in the engine, and then able to return to them at any time. You wouldn't be able to have work which you can load back in at a later time without this contribution.


# Have I Improved My Programming Skills and Domain Knowledge?

_Evaluate your improvement in skill_

I have most certainly improved my programming skills and domain knowledge. What I have learned can be evidenced from the contributions that I have made.

Before, I never knew how to use compute shaders. This was something which I wanted to do in Block B from the start, to have my brushes use compute shaders to modify terrain, but I couldn't achieve this due to time constraints. Now, I have a clear understanding of them and they have been a core part of the terrain editor.

Before, I couldn't understand how to use the serialization or how to do runtime reflection of components, in fact, I have failed Block A due to this. But now, I know how to use them and have made a saving/loading system.

Before, the API and architecture I had for the terrain editor from Block B was horrible. After working in a team, I have learned how to make APIs which was to be used by other teammates, how to imagine and build sound architecture.

Before, I was very used to working solo, at best, I was working with other people on a GAME, not on an engine or a tool. Because of this, my way of working and structure was very different. After this, I have experience of working in a team and using the Git workflow to make the most of my productivity. 

Overall, I think this project has pushed my boundaries of my programming skills.
