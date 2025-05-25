---
layout: project
title: "Pitfall Recreation"
date: 2023-11-03
categories: [projects]
image: /assets/images/Y1A/main.gif
description: "A recreation of 1982 game Pitfall"
contributions: "AABB Collision System | 2D Physics | Rope Physics "
tools: "C++"
team_size: "Solo"
platforms: "Windows"
duration: "8 Weeks (Sep 2023 - Nov 2023)"
---


## 🧗🏻‍♂️ Pitfall

*Pitfall* is a 2D platformer game developed as part of a Year 1 game programming module, focused on building core gameplay systems from the ground up using C++. The game features classic platforming mechanics including responsive player movement, jumping, collision detection, and a dynamic rope-swinging system for navigating challenging terrain. Levels are designed using the Tiled map editor and loaded into the game via a custom CSV parser, enabling flexible and scalable level creation. A modular camera system ensures smooth tracking and scene transitions, while a game state manager handles menus, gameplay, and pause logic cleanly. The project emphasizes custom physics, efficient spatial systems, and interactive gameplay elements, all structured within a maintainable codebase reflecting professional development practices.

---

## 🔨 My Contributions

---

### 💢 AABB Collision System

**Purpose:**
Implements axis-aligned bounding box (AABB) collision detection to support fundamental physics interactions between entities in the game world.

**Key Features:**

* Efficient collision checks using bounding boxes.
* Custom `Vector2` class to handle position and direction calculations.
* Used for character-environment interactions like wall detection and ground collision.

**Challenges:**

* Handling simultaneous collisions from multiple directions (e.g. ground and wall at the same time).
* Implementing friction and gravity consistently across different object types.
* Balancing realism with responsiveness for gameplay feel.

---

### 🪢 Rope Physics

**Purpose:**
Simulates rope-like movement enabling swinging mechanics, adding verticality and challenge to level navigation.

**Key Features:**

* Implemented as a physics-based pendulum using gravity and anchor points.
* Player can grab onto the rope and build momentum to swing across obstacles.
* Adds dynamic interaction with level geometry.

**Challenges:**

* Creating believable rope physics without external libraries.
* Preventing unnatural behavior when switching between rope and grounded movement.
* Fine-tuning to ensure gameplay balance and fluid transitions.

---

### 🎥 Camera System (Screen, World, and Local Space Handling)

**Purpose:**
Manages what the player sees on-screen, translating between different coordinate systems to ensure proper rendering and player focus.

**Key Features:**

* Seamlessly tracks player position within large tilemaps.
* Converts between screen space, world space, and local space.
* Avoids abrupt camera movement with smooth following logic.

**Challenges:**

* Debugging offset issues between different spaces.
* Ensuring all systems (rendering, collision, input) stay in sync with the camera’s current view.
* Maintaining performance when rendering only visible entities.

---

### 🗣️ Game State Manager

**Purpose:**
Controls transitions between different game states like main menu, playing, paused, and game over.

**Key Features:**

* Clearly separated logic for UI vs gameplay.
* Finite state machine (FSM) structure to keep code organized and modular.
* Prevents player control or logic updates when the game is paused or in menus.

**Challenges:**

* Managing input routing across states (e.g., UI vs game input).
* Avoiding state corruption from asynchronous or delayed transitions.
* Debugging edge cases during fast or unexpected transitions (e.g., rope interaction during pause).

---

### 🏃 Controllable Player Character

**Purpose:**
Core player entity that reacts to input, interacts with the environment, and embodies game mechanics like movement, jumping, and swinging.

**Key Features:**

* Responsive input mapping for keyboard controls.
* Movement constrained by AABB collisions.
* Integration with rope physics and interaction triggers.

**Challenges:**

* Balancing jump and move feel to avoid “floatiness.”
* Resolving edge cases in movement when colliding from multiple directions.
* Syncing animation states with physical interactions.

---

### 🗺️ CSV Map Parser – TiledManager

**Purpose:**
Automates the loading of levels designed in Tiled, converting CSV data into in-game tilemaps and entities.

**Key Features:**

* Parses `.csv` output from Tiled map editor.
* Dynamically instantiates objects based on tile data (e.g., solid tiles, spikes, ropes).
* Provides flexible level design workflow with rapid iteration.

**Challenges:**

* Managing different types of layers (collision, visual, objects).
* Ensuring correct alignment between Tiled’s grid and in-game rendering.
* Debugging mismatched tile data or misconfigured CSVs.

---

### 🧠 Key Engineering Concepts

* **AABB Collision Detection** for efficient and reliable environment interaction
* **Physics Simulation** using vectors and manual integration for rope dynamics
* **Finite State Machines (FSM)** to manage game states and transitions
* **Camera Space Management** (screen, world, and local) for dynamic scene control
* **Custom Input Handling** for responsive character control
* **CSV Parsing and Tilemap Integration** for dynamic level loading from external tools
* **Modular Code Architecture** for reusability and scalability

---

### 🎯 Overall Impact

Working on *Pitfall* gave me hands-on experience building essential game systems from scratch. I deepened my understanding of low-level physics, collision handling, and memory-conscious C++ development. This project challenged me to think structurally, debug complex interactions, and translate game design into reliable code. Most importantly, it taught me how to build modular systems that are both functional and maintainable — a critical step in my growth as a game developer.
