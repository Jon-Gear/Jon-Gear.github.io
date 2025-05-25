---
layout: project
title: "3D Endless Runner: Cross-Platform Development on Raspberry Pi"
date: 2024-01-26
categories: [projects]
image: /assets/images/Y1B/main.gif
description: "A 3D endless runner using Bullet Physics and OpenGL on a Raspberry Pi"
contributions: "3D Rendering using OpenGL | Raspberry Pi Cross-Platform Development | Physics System using Bullet"
tools: "C++ | OpenGL ES | Bullet Physics"
team_size: "Solo"
platforms: "Windows | Linux"
duration: "8 Weeks (Nov 2023 - Jan 2024)"
---

## 🏃 3D Endless Runner: Cross-Platform Development on Raspberry Pi

3D Endless Runner: Cross-Platform Development on Raspberry Pi is a custom-built 3D game designed to run seamlessly on both desktop PCs and Raspberry Pi 4. It showcases real-time rendering, physics simulation, and responsive input systems, all developed from the ground up using C++, OpenGL ES, and Bullet Physics. The game features an endless runner format with procedurally loaded environments, where the player navigates through a dynamic 3D world. The project demonstrates advanced technical implementation under constrained hardware conditions, highlighting modular engine design, performance optimization, and platform-agnostic development practices.

---

## 🔨 My Contributions

---

### 🥧 Cross-Platform Project Setup

**Key Features:**

* Configured the Raspberry Pi 4 as a build target using Visual Studio Linux Development with remote deployment.
* Installed necessary dependencies for Bullet Physics and OpenGL ES using apt packages and ARM-compatible libraries.
* Ensured project compatibility with both Windows and Linux systems using conditional compilation and consistent file structure.

**Challenges & Solutions:**

* **Challenge**: Understanding and managing library dependencies across different architectures (x86 on PC vs ARM on Pi).

  * **Solution**: Studied how Bullet and OpenGL ES are built for ARM systems. Used guidance from peers and tutorials to install and link libraries appropriately.
* **Challenge**: Ensuring the same code runs on both platforms.

  * **Solution**: Created modular code with abstraction layers for platform-specific components (e.g., window creation, input).

---

### 🌵 3D Model Loading and Rendering

**Key Features:**

* Implemented `Model` and `Mesh` classes for loading `.obj` files and associated textures.
* Integrated texture loading and applied basic material properties using shaders.
* Created a **custom transform class** to manage 3D object transformations (translation, rotation, scaling).
* Rendered complex 3D models like the LearnOpenGL backpack on Raspberry Pi.

**Challenges & Solutions:**

* **Challenge**: Rendering models efficiently on the Raspberry Pi, which has limited GPU power.

  * **Solution**: Used simplified shaders and optimized model formats. Stripped unnecessary vertex attributes and textures.
* **Challenge**: Understanding how to build the model-view-projection pipeline from scratch.

  * **Solution**: Studied transformation matrices and implemented matrix math for model/view/projection generation using LearnOpenGL as reference.

---

### 🔫 Bullet Physics Integration

**Key Features:**

* Integrated Bullet Physics 3.06 and created a dynamic physics world.
* Built a system to convert `.obj` mesh data into collision shapes.
* Implemented rigid body behavior for interactable objects (e.g., falling backpack).
* Simulated gravity, collisions, and object dynamics in real-time.

**Challenges & Solutions:**

* **Challenge**: Bullet has no built-in event system for collisions.

  * **Solution**: Manually compared object states between frames to detect interactions.
* **Challenge**: Connecting physics objects to render transforms.

  * **Solution**: Created a unified `GameObject` system to synchronize transform updates between physics and graphics pipelines.

---

### 💡 Shader System & Lighting

**Key Features:**

* Wrote vertex and fragment shaders compatible with OpenGL ES 2.0.
* Created a programmable rendering pipeline with support for lighting.
* Implemented diffuse and ambient lighting for improved visual depth.

**Challenges & Solutions:**

* **Challenge**: Debugging GLSL shaders on Raspberry Pi with no meaningful visual feedback.

  * **Solution**: Used logging and error checking extensively. Simplified shaders to the bare minimum and iteratively added complexity.
* **Challenge**: Understanding GPU pipeline and how to manage buffers and uniforms.

  * **Solution**: Followed tutorials from LearnOpenGL and received feedback from lectures to deepen understanding.

---

### 🪟 In-Game Debug Tools with Dear ImGui

**Key Features:**

* Integrated Dear ImGui to display runtime debugging information such as FPS, object counts, and camera data.
* Built a toggleable overlay interface for development and testing.

**Challenges & Solutions:**

* **Challenge**: Adapting ImGui to work with a custom OpenGL context on Raspberry Pi.

  * **Solution**: Studied how ImGui renders to framebuffers and connected its rendering to our existing pipeline.

---

### 🎥 Camera and Input System

**Key Features:**

* Developed a custom camera system with orbit and FPS-style movement.
* Improved upon an existing Input Manager from a previous project, adding support for multiple input types and smoother camera control.

**Challenges & Solutions:**

* **Challenge**: Synchronizing input and camera controls while maintaining performance.

  * **Solution**: Time-boxed input polling and used delta time to update movement consistently.

---

### 🏗️ Base Game Architecture

**Key Features:**

* Created a modular `GameObject` system to handle rendering, physics, and updates in a unified way.
* Built a central `GameManager` to orchestrate game loops, object spawning, and cleanup.
* Introduced object pooling to manage memory usage effectively.

**Challenges & Solutions:**

* **Challenge**: Designing a clean architecture that is flexible yet efficient on constrained hardware.

  * **Solution**: Refactored frequently based on test results and performance metrics. Focused on reducing memory allocations during gameplay.

---

### 🧠 Key Engineering Concepts

* Cross-compilation and deployment for ARM-based systems (Raspberry Pi 4)
* Real-time 3D rendering using OpenGL ES 2.0
* Model loading and custom transformation matrices (model/view/projection)
* Physics integration with Bullet (rigid bodies, collision shapes, simulation loop)
* GPU shader programming (vertex & fragment shaders, lighting models)
* Debug UI integration with Dear ImGui
* Modular engine architecture and resource management
* Synchronizing physics and rendering systems in real time

---

### 🎯 Overall Impact

Working on this project gave me hands-on experience with low-level systems programming, graphics pipeline architecture, and cross-platform development. I learned how to build a functioning 3D game engine from scratch, optimize for limited hardware, and troubleshoot complex integration issues across rendering and physics systems. It also improved my ability to debug, document, and structure code for maintainability—skills that are critical in both game development and embedded systems programming.