---
layout: project
title: "IgKnighted"
date: 2024-06-28
categories: [projects]
image: /assets/images/Y1D/main.jpg
description: "Dive into the heart of a forgotten, flooded temple as powerful knights forged from fire. In this twinstick shooter bullet hell, you will fight off waves of enemies in order to progress through the temple and become the most powerful fighter of all! Work together or embark alone."
contributions: "Input Mapping | Local Multiplayer | Enemy Attacks (Shooting & Aiming) | Player Damage | Collectables | Power-Ups | In-Engine Animations"
tools: "Unreal Engine 5"
team_size: "4 Programmers | 3 Designers | 5 Artists"
platforms: "Windows"
duration: "8 Weeks (May 2024 - Jun 2024)"
---

## 🔥 IgKnighted – A Stylized Local Multiplayer Arena Shooter

[Click here to visit our game page!](https://buas.itch.io/igknighted)

IgKnighted is a fast-paced, couch co-op arena shooter built in Unreal Engine 5 using Blueprints. Designed for local multiplayer mayhem, players face off against waves of enemies while collecting power-ups, dodging enemy fire, and racing for the highest score. Inspired by classic arcade energy and modern stylized visuals, the game features dynamic combat, painterly post-processing, and a responsive control scheme. Developed collaboratively by a multidisciplinary team, IgKnighted blends tight gameplay mechanics with polished visuals and smart system design—all brought together through agile development and hands-on iteration.

---

## 🔨 My Contributions

---

### 🎮 Input Mapping

![image1](../../assets/images/Y1D/4.png)

**What it Does:**
Mapped controller and keyboard inputs to player actions like movement and shooting.

**Key Features & Challenges:**

* Implemented a clean **Input Action Mapping system** via Unreal's new Enhanced Input system.
* Replaced deprecated input code to modern standards.
* Required handling of multiple controllers for **local multiplayer**—a major UX focus.

**Impact on the Game:**

* Enabled plug-and-play support for multiple input devices.
* Established a scalable input foundation for future expansion (e.g., remappable keys).

---

### 👥 Local Multiplayer

![image1](../../assets/images/Y1D/1.png)


**What it Does:**
Allows two players to play simultaneously on the same machine with separate controllers.

**Key Features & Challenges:**

* Built custom **GameMode Blueprint** logic to support multiple players.
* Handled player spawning, possession, and UI differentiation.
* Took **2 full days**, more than expected, due to intricacies of Unreal’s player handling and controller indexing.

**Impact on the Game:**

* Made the game a shared, couch-friendly experience.
* Established a robust framework for multiplayer, a core gameplay feature.

---

### 👾 Base Enemy Attack (Shooting & Aiming)

![image1](../../assets/images/Y1D/5.png)

**What it Does:**
Enemies detect players, aim, and shoot projectiles.

**Key Features & Challenges:**

* Rewrote enemy perception logic to reduce redundancy and improve clarity.
* Implemented **aiming and projectile logic** in Blueprints, synced with animation triggers.

**Impact on the Game:**

* Created combat tension and challenge.
* Set the stage for more complex AI behavior and difficulty scaling.

---

### ❤️ Player Damage

![image1](../../assets/images/Y1D/2.png)

**What it Does:**
Enables player health tracking and death when hit by enemies.

**Key Features & Challenges:**

* Synchronized hit detection with enemy projectiles.
* Triggered visual/audio feedback and death sequence.
* Required balancing to keep combat fair.

**Impact on the Game:**

* Introduced stakes to gameplay.
* Provided a functional health system used in all combat scenarios.

---

### ✨ Collectables (Spawning, Score, UI)

![image1](../../assets/images/Y1D/3.png)


**What it Does:**
Implements items that players can collect during gameplay to increase score.

**Key Features & Challenges:**

* Procedural spawning system during waves.
* Score tracking system integrated into the HUD.
* UI dynamically updates when collectables are picked up.

**Impact on the Game:**

* Adds a secondary game loop and reward mechanism.
* Encourages movement and exploration during combat.

---

### ⚡ Power-Ups (Extended)

![image1](../../assets/images/Y1D/6.png)


**What it Does:**
Power-ups grant temporary advantages (e.g., speed, attack boost), but now with smarter logic.

**Key Features & Challenges:**

* Prevented players from picking up the **same power-up multiple times**.
* Extended the base system to support stacking effects and timed buffs.

**Impact on the Game:**

* Balanced and deepened the gameplay strategy.
* Reduced redundancy and clutter in the level by avoiding wasted pickups.

---

### 🕺 Player Animations

![image1](../../assets/images/Y1D/7.png)


**What it Does:**
Integrated character animations like idle, walk, shoot, and death.

**Key Features & Challenges:**

* Learned and implemented UE5’s animation system using **Animation Blueprints**.
* Aligned animation states with gameplay actions (e.g., shooting, being hit).
* Faced a learning curve but found the system intuitive after initial practice.

**Impact on the Game:**

* Brought the player character to life.
* Enhanced the game’s polish and visual feedback significantly.

---

## 🧠 Overall Impact

Your work provided **core systems** used by all team members:

* Established clean, readable Blueprint code used as a reference by peers.
* Contributed features fundamental to gameplay: multiplayer, combat, progression.
* Integrated technical systems with UI and art to support design goals.
* Increased team productivity by resolving bugs and improving clarity.

---

Would you like this turned into a formatted portfolio piece or PDF summary next?
