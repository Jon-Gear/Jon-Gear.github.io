---
layout: project
title: "Custom C++ Game Engine"
date: 2024-11-01
categories: [projects]
image: /assets/images/Y2A/main.gif
description: "Custom C++ Engine was a solo project. The project features core engine functionality such as an ECS Architecture, an ImGui editor, a GLTF importer, a Resource manager, full scene serialization using Cereal, a grid-based tile editor, and support on Windows and PS5."
contributions: "ECS Design (EnTT) | Particle System | ImGui Editor & Gizmo Tooling | GLTF Runtime Loader | Resource Manager | Hierarchy System | Cross-Platform Engine Abstractions | JSON Serialization | Tile-Based Level Editor"
tools: "C++, OpenGL, GLSL, EnTT, ImGui, Cereal, STB, tinygltf, ImGuizmo"
team_size: "Solo"
platforms: "Windows | PS5"
duration: "8 Weeks (Sep 2024 - Nov 2024)"
---

## ⚙️ Custom C++ Game Engine – My Contributions

### 🔍 What It Does

This project is a fully custom-built C++ game engine designed to support real-time simulation, modular development, and cross-platform deployment. The engine provides a comprehensive runtime environment with systems for graphics, physics, input, audio, resource management, and user interface tooling. It follows an Entity-Component-System (ECS) architecture powered by EnTT, allowing for efficient, scalable scene management.

---

## 🔨 My Contributions

---

### 🎉 Advanced Particle System Framework

![image1](../../assets/images/Y2A/W1.gif)

In the first week, I developed a robust particle system using EnTT, enabling various particle effects like Confetti, Sparks, and Fire. The system supports both fixed and dynamic timestep updates and includes configurable emitters with features such as color modes (gradient, single, random), shape selection (cube, cylinder, sphere), lifetime, physics properties (mass, gravity), and spawn volume (cone shape). I also implemented a physics system to simulate gravity-influenced motion and a rendering system using a custom `MeshRendererXSR`.

**Key Features:**

* EnTT-based ECS structure
* Configurable particle behavior and emitter settings
* Physics simulation with per-particle velocity and mass
* Real-time rendering of particles with color interpolation

**Challenges:**

* Managing particle lifecycle with efficient pooling
* Implementing spawn volume and direction using cone geometry
* Synchronizing rendering and simulation in the engine loop

---

### 🪟 Inspector and Editor Tooling with ImGui

![image1](../../assets/images/Y2A/W2.gif)

I integrated a custom GUI using ImGui for simulation control and runtime inspection of scene entities. This included a scene hierarchy viewer, particle emitter editor, and transform gizmos via ImGuizmo. The system allows spawning and editing emitters, viewing debug visualizations, and manipulating objects within the editor viewport.

**Key Features:**

* Real-time simulation controls (play/pause/step)
* Scene hierarchy and entity inspection
* Interactive gizmos for transform manipulation
* Custom UI for emitter properties (color, mesh, physics)

**Challenges:**

* Recursive traversal and rendering of the scene graph
* Consistent UI state handling across play/edit modes
* Seamless integration of ImGuizmo with engine transforms

---

### 🥣 Serialization System and Platform Abstraction

![image1](../../assets/images/Y2A/W3.gif)

I implemented full serialization and deserialization of entities and components using the Cereal library. This included a custom system for saving/loading entire scene hierarchies as JSON. I also built cross-platform abstractions for input, file I/O, audio, and rendering using a mix of PIMPL, macros, and conditional compilation.

**Key Features:**

* Runtime registration of serializable components via EnTT Meta
* Save/load of nested hierarchies with custom scene nodes
* Component-level granularity for serialization
* Platform abstraction layers for PC and console builds

**Challenges:**

* Maintaining runtime reflection metadata across component types
* Resolving differences in platform APIs while maintaining clean interfaces
* Ensuring serialized data matches runtime expectations

---

### 🌳 Runtime Resource Management and GLTF Importer

![image1](../../assets/images/Y2A/W4.gif)

I created a runtime resource manager with support for loading and caching meshes and textures. The manager supports reference counting and lazy loading. I also developed a GLTF importer that loads complex models with proper mesh-texture mapping and hierarchy instantiation in the engine.

**Key Features:**

* Resource base class and ID-based caching
* On-demand loading of Mesh, Texture, and Model resources
* Support for hierarchy and transform loading from GLTF
* Integration with XSR for rendering

**Challenges:**

* Parsing GLTF data into engine-compatible formats
* Handling embedded textures and multiple mesh primitives
* Maintaining hierarchical transforms across nested nodes

---

### 🧊 Tile-Based Level Editor

![image1](../../assets/images/Y2A/W5.gif)

I implemented a functional tile-based level editor. This editor allows users to define grid sizes, import tile models via GLTF, assign them to grid tiles, and save/load entire levels. Tiles can be selected via mouse picking in the viewport and visually edited in the Inspector.

**Key Features:**

* Grid and tile component system
* Mouse-based selection and manipulation
* Model assignment and instantiation per tile
* Debug visualization of grid and selected tiles

**Challenges:**

* Implementing accurate raycasting for tile selection
* Managing tile identity and reuse via serialization
* Seamless integration with resource and scene systems

---

## 🧠 Key Engineering Concepts

* **Entity-Component-System (ECS):** Leveraged EnTT for scalable and performant game object management.
* **Serialization/Deserialization:** Used Cereal for structured JSON I/O and supported full scene graph persistence.
* **Runtime Reflection:** Enabled dynamic component registration and flexible save/load routines using EnTT Meta.
* **Cross-Platform Development:** Designed modular interfaces for input, audio, rendering, and file systems with platform-specific implementations.
* **Resource Management:** Built an extensible resource loader with caching and reference counting for runtime efficiency.
* **UI & Tooling:** Created powerful in-engine tools using ImGui/ImGuizmo to enable real-time editing, inspection, and debugging.

---

## 🎯 Impact

My contributions resulted in a highly modular and feature-rich custom game engine. The engine supports real-time simulation, flexible particle systems, intuitive editor tools, and cross-platform deployment. It streamlines asset handling, enables robust scene editing, and lays a strong foundation for future game development features. The project goes well beyond basic module expectations, showcasing advanced understanding in rendering, architecture, serialization, tooling, and systems design.