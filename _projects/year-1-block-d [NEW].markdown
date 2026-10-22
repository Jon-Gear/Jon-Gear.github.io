# IgKnighted — Final Portfolio Page Structure

Project: IgKnighted — Unreal Engine 5 Gameplay Programming Target roles: Gameplay Programmer / Unreal Engine Developer / C++ Gameplay Programmer

The page should read like a technical case study, moving from the overall Unreal Engine architecture into specific gameplay systems, implementation decisions, and engineering challenges.

I recommend 9 major sections, with subsections to keep the page organized and easy to navigate.

## 01. Project Overview

Introduce IgKnighted and establish your responsibilities.

### 1.1 About IgKnighted

- Game description, genre, development team, and development duration.
- Core gameplay loop: two-player cooperative arena shooter with three zones.
- Gameplay trailer, screenshots, and downloadable game.

### 1.2 My Role & Contributions

- Gameplay programming responsibilities.
- Implemented mechanics and systems in the original project.
- Technologies used: Unreal Engine 5, Blueprints, Enhanced Input, Animation Blueprints.

### 1.3 Technical Highlights

- Brief overview of multiplayer, combat, AI, abilities, game progression, and animation.
- Introduction to the retrospective C++/Blueprint architecture discussed throughout the page.

## 02. Gameplay Framework & Core Architecture

Demonstrate understanding of Unreal Engine's Gameplay Framework, object lifecycles, and system responsibilities.

### 2.1 Overall Game Architecture

- Complete Gameplay Framework class diagram.
- Relationships between GameInstance, GameMode, GameState, PlayerController, PlayerState, Pawns, and AIControllers.
- C++ base classes and Blueprint-derived classes in the proposed reference design.

### 2.2 GameInstance & Game Configuration

- Main menu and game startup.
- Player count selection.
- Difficulty configuration and dynamic difficulty toggle.
- Tutorial selection and transitions into gameplay.

### 2.3 GameMode & GameState

- Match initialization and gameplay rules.
- Shared five-life system.
- Victory and defeat conditions.
- Current zone, match state, and enemy tracking.

### 2.4 PlayerController, PlayerState & Pawn

- Ownership and responsibilities of each class.
- Player spawning and possession.
- Individual score and multiplier tracking.
- Shared team state versus individual player state.

## 03. Player Systems & Local Multiplayer

Explain player control, input handling, character behavior, and the complete player lifecycle.

### 3.1 Local Multiplayer Architecture

- Creating and configuring two local players.
- Controller management and Pawn possession.
- Handling multiple input devices.
- Player initialization and respawning.

### 3.2 Enhanced Input System

- Input Actions and Input Mapping Contexts.
- Movement, aiming, shooting, and dashing.
- Keyboard and controller support.
- Independent input handling for both players.

### 3.3 Player Character Architecture

- Character composition and reusable Actor Components.
- CharacterMovement, CombatComponent, and DashComponent.
- Separation of player control, gameplay logic, and presentation.

### 3.4 Dash & Invulnerability

- Dash movement and cooldowns.
- Temporary damage immunity.
- Enhanced dash and enemy collision damage.
- Dash state management and edge cases.

### 3.5 Player Death & Respawning

- Instant-death damage model.
- Shield protection and shield breaking.
- Shared life consumption.
- Respawn lifecycle and Game Over handling.

## 04. Combat Systems & Enemy AI

Showcase gameplay programming through projectile combat, AI behavior, targeting, and damage interactions.

### 4.1 Combat Architecture

- Reusable combat components.
- Weapon firing and projectile spawning.
- Collision detection, damage processing, and combat events.
- C++ and Blueprint responsibilities.

### 4.2 Projectile & Damage System

- Fire projectiles and enemy water attacks.
- Projectile movement, collisions, and destruction.
- Damage validation against shield and dash states.
- Preventing duplicate damage events.

### 4.3 Enemy AI Architecture

- AIController and EnemyCharacter responsibilities.
- Shared enemy base class and specialized archetypes.
- Target acquisition and player selection.
- Behavior Trees, Blackboards, and simpler state-machine alternatives.

### 4.4 Enemy Archetypes

- Crawler: Player pursuit and contact damage.
- Scorpion: Ranged targeting, distance management, and projectiles.
- Water Mollusk: Stationary water-stream attack and area denial.

### 4.5 AI Design Decisions

- Reusable AI behaviors versus enemy-specific logic.
- Target selection in a two-player game.
- Attack cooldowns, navigation, and performance considerations.

## 05. Gameplay Abilities, Power-Ups & Scoring

Demonstrate modular gameplay mechanics, effect management, and state-driven systems.

### 5.1 Power-Up Architecture

- Pickup Actors and overlap events.
- Applying, tracking, and removing effects.
- Effect duration and stacking rules.

### 5.2 Individual Power-Ups

- Shield: Absorbs one hit.
- Power Shot: Adds two additional projectiles.
- Enhanced Dash: Additional dash capacity and enemy elimination.
- Matchsticks: Score collection.

### 5.3 Gameplay Ability System — Reference Design

- Ability System Component and Attribute Sets.
- Gameplay Abilities and Gameplay Effects.
- Gameplay Tags, cooldowns, and duration-based effects.
- Comparison between lightweight Blueprint/component systems and GAS.

### 5.4 Scoring & Multipliers

- Individual player scores.
- Score multiplier management.
- Matchstick collection.
- Event-driven score updates and HUD synchronization.

## 06. Level Design, Progression & Difficulty Systems

Explain the game's progression architecture and how gameplay systems coordinate across a persistent world.

### 6.1 Persistent Level Architecture

- One gameplay map containing three zones.
- ZoneManager and zone-specific Actors.
- Encounter configuration and world progression.

### 6.2 Enemy Spawning & Encounter Management

- EnemySpawner responsibilities.
- Encounter activation and enemy tracking.
- Enemy death events and zone completion.
- Preventing premature zone completion.

### 6.3 Zone Progression

- Zone unlocking and barriers.
- Advancing to the next encounter.
- Completing the third zone and triggering victory.

### 6.4 Difficulty System

- Easy, Medium, and Hard presets.
- Difficulty configuration and gameplay parameters.
- Enemy spawning and encounter scaling.

### 6.5 Dynamic Difficulty — Reference Design

- Tracking player performance.
- Adjusting encounter difficulty.
- Difficulty boundaries and balancing considerations.

## 07. Animation, UI & Gameplay Feedback

Demonstrate how gameplay logic interacts with animation, interface elements, and player feedback.

### 7.1 Animation Blueprint Integration

- Idle, walking, shooting, and death animations.
- Animation states and transitions.
- Synchronization between character actions and animation.

### 7.2 Gameplay Events & Visual Feedback

- Projectile effects.
- Damage and shield-break feedback.
- Dash effects and invulnerability visualization.
- Enemy death and zone completion feedback.

### 7.3 User Interface Architecture

- Main menu and game configuration UI.
- HUD and player-specific information.
- Lives, scores, multipliers, and power-up indicators.
- Victory and Game Over screens.

### 7.4 Event-Driven UI Updates

- Gameplay events and Blueprint Event Dispatchers.
- C++ delegates in the reference architecture.
- Updating HUD state without unnecessary polling.

## 08. Engineering Practices & Technical Decisions

Demonstrate software engineering maturity beyond implementing gameplay mechanics.

### 8.1 C++ & Blueprint Architecture

- C++ base classes versus Blueprint-derived assets.
- Unreal reflection system: `UCLASS`, `UPROPERTY`, `UFUNCTION`.
- Reusable Actor Components, interfaces, and delegates.
- Data-driven configuration and maintainability.

### 8.2 Debugging & Problem-Solving

- Local multiplayer controller assignment and possession issues.
- Player input lifecycle problems.
- Collectable count inconsistencies.
- Zone barrier and enemy elimination tracking.
- Repeated projectile/hazard damage.

### 8.3 Testing & Edge Cases

- Two-player gameplay testing.
- Respawning and shared-life edge cases.
- Power-up stacking and expiration.
- AI targeting and collision validation.

### 8.4 Performance & Optimization

- Actor spawning and destruction.
- Projectile lifetime management.
- AI update frequency.
- Tick usage versus event-driven systems.
- Unreal profiling and debugging tools.

### 8.5 Architectural Trade-offs

- Components versus inheritance.
- C++ versus Blueprints.
- Custom abilities versus Gameplay Ability System.
- Behavior Trees versus state machines.
- Maintainability versus implementation complexity.

## 09. Results, Retrospective & Lessons Learned

Conclude the project by highlighting the finished game, engineering knowledge, and development experience.

### 9.1 Final Product

- Gameplay trailer and screenshots.
- Playable Windows build.
- Key gameplay features delivered.

### 9.2 Technical Challenges & Solutions

- Most significant development problems.
- Decisions made during the original project.
- What worked and what could be improved.

### 9.3 Retrospective Architecture

- How the original Blueprint systems could be structured in C++ today.
- Improvements to modularity, maintainability, and extensibility.
- Distinction between original implementations and reconstructed/reference designs.

### 9.4 Key Takeaways

- Unreal Engine Gameplay Framework knowledge.
- Gameplay systems and AI programming.
- Local multiplayer architecture.
- Gameplay engineering and collaborative development.

