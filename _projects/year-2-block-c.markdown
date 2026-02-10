---
layout: project
title: "Fire Ant Engine: Custom C++ Strategy Game Engine"
role: "Engine Programmer"

date: 2025-04-11
version: short
categories: [featured_projects]
image: /assets/images/Y2C/main_large.gif
description: "Fire Ant Engine is a continuation of my custom Terrain Editor. The team was joined by seven talented programmers. The project features a prefab system, a terrain editor, flow-fields for AI agent navigation, an in-engine UI editor, optimizations which allowed a support for a large terrain map and many units, and support on Windows and Nintendo Switch."
contributions: "Terrain Editor | Terrain Rendering | Height Editing | Texture Editing | Prop Placement | Scene Serialization | Prefabs"
tools: "C++, OpenGL, GLSL (Compute & Tesselation Shaders), EnTT, Cereal, ImGui"
team_size: "7 Programmers"
platforms: "Windows | Nintendo Switch"
duration: "8 Weeks (Feb 2025 - Apr 2025)"
---

## 🌄 Terrain Editor – My Contributions

### 🧩 What It Does

The **Terrain Editor** is a real-time terrain editing toolkit built directly into a custom C++ game engine. It allows users to sculpt terrain height, paint textures, scatter props, and create plateaus—all in an interactive, visual editing environment. It also integrates tightly with a serialization system to allow scene saving/loading.

This tool was critical to the Fire Ant engine, forming its foundation for terrain-based gameplay and visual fidelity.

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/fireant" target="_blank" rel="noopener">⚙️ See Code Examples</a>

---

## 🔨 My Contributions

---

### 🛠️ Terrain Editor Architecture

I restructured and extended the original Block B prototype into a robust, extensible tool using an object-oriented brush system built on a shared interface. Each brush (Height, Texture, Prop, Plateau) inherits from a common `TerrainBrush` base class, allowing seamless expansion.

**Key Features:**

* GUI-based brush selection and parameter adjustment.
* Scalable design via polymorphic brush interface.
* CPU-side caching of terrain data for efficient sampling and editing.
* Terrain intersection via raymarching for accurate brush placement.

**Challenges:**

* Making the system extensible for future brushes.
* Decoupling brush logic from UI and rendering pipeline.

---

### 🌋 Terrain Rendering

![image1](../../assets/images/Y2C/tesselation.gif)

I implemented a custom rendering pipeline for terrain using **OpenGL tessellation shaders**, which dynamically displace geometry based on a height map texture. A new rendering pass was added, separate from other objects, with custom `TerrainMaterial` and `TerrainMeshRenderer` classes.

**Key Features:**

* Tessellation shaders for real-time LOD-based displacement.
* Dynamic normal calculation in fragment shader.
* Separate terrain material supporting height maps.

**Challenges:**

* Isolating terrain logic from general-purpose rendering.
* Encoding/decoding 32-bit float heights via RGBA texture format.

---

### ⛰️ Height Brush

![image1](../../assets/images/Y2C/height_brush_raise.gif)

![image1](../../assets/images/Y2C/height_brush_lower.gif)

Rebuilt the existing CPU-based height tool into a **compute-shader powered GPU brush** capable of raising/lowering terrain. I added support for both circle-shaped and alpha-textured brushes.

**Key Features:**

* Fully GPU-accelerated with live heightmap updates.
* Real-time brush projection and feedback.
* Gaussian weight-based deformation for smooth results.
* Two brush types: mathematical circle & custom alpha mask.

**Challenges:**

* Transitioning from CPU logic to GPU compute workflow.
* Ensuring precision and edit locality on GPU-side textures.

---

### 🎨 Texture Brush

![image1](../../assets/images/Y2C/texture_brush.gif)

This brush paints the terrain’s **base albedo texture** using tiled patterns or flat colors. Similar to the height brush, it’s compute-shader based and supports the same brush types.

**Key Features:**

* Dynamic painting with real-time feedback.
* Tiling texture brush with intensity blending.
* Alpha-textured painting for precision editing.

**Challenges:**

* Handling texture wrapping and tiling logic in GLSL.
* Synchronizing between height and texture spaces.

---

### 🌳 Prop Brush

![image1](../../assets/images/Y2C/prop_brush.gif)

The prop brush lets users scatter 3D assets (like trees and rocks) across the terrain using a visual brush. The brush respects terrain normals and supports random rotation and placement density.

**Key Features:**

* Drag-and-drop asset selection via custom asset browser.
* Placement along terrain normals with random orientation.
* Fully integrated into scene save/load system.

**Challenges:**

* Accurate terrain height & normal sampling in 3D space.
* Balancing randomness and visual control.

---

### 🏔️ Plateau Brush

![image1](../../assets/images/Y2C/plateau_brush.gif)

A unique brush that flattens terrain toward a selected or user-defined height, allowing sculpting of roads, plateaus, or flat regions.

**Key Features:**

* Terrain sampling to select plateau target height.
* Falloff-controlled smoothing for natural edges.
* Both alpha and circular brush modes supported.

**Challenges:**

* Smoothing the edges for a natural transition.
* Sampling terrain height and applying height interpolation.

---

### 💾 Scene Saving & Loading System

![image1](../../assets/images/Y2C/prefabs.gif)

I developed a robust scene serialization system using **Cereal + EnTT Meta**, supporting recursive entity hierarchies and runtime component reflection.

**Key Features:**

* Fully JSON-based scene save/load.
* Transform hierarchy preservation with unique identifiers.
* Runtime reflection system via `EnTT Meta`.
* Component save/load logic decoupled via function bindings.

**Challenges:**

* Designing a runtime reflection architecture in C++.
* Managing parent-child relationships in serialized data.
* Ensuring extensibility for new component types.

---

## 🧠 Key Engineering Concepts

* **GPU Compute Shaders** for real-time texture modification.
* **Tessellation & Displacement Mapping** for terrain rendering.
* **Modular Brush Architecture** using polymorphism.
* **Scene Graph Hierarchy Traversal & Recursion** for save/load.
* **Runtime Component Reflection** with `EnTT Meta` + Cereal.

---

## 🎯 Impact

My contributions enabled:

* Full real-time terrain editing inside the engine.
* A scalable brush system reused across multiple engine modules.
* Level creation workflows through scene saving/loading.
* A technical and visual foundation for terrain-based gameplay.

Without these contributions, the team wouldn’t have had editable terrain, reusable prefab systems, or the ability to save and iterate on scenes—making them foundational to both the engine and project scope.
