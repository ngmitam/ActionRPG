# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2025-10-15

### Added

#### AI & Navigation System

-   **Minimap Feature**: Added minimap capture and widget functionality for better player navigation
-   **Enhanced Enemy Detection**: Implemented line-of-sight detection for more realistic AI behavior
-   **Random Patrol Movement**: Added random movement patterns during enemy patrol states
-   **Individual Player Detection**: Enemies now detect players individually without global alert system
-   **Navigation System Integration**: Added NavigationSystem dependency for improved pathfinding

#### Configuration System

-   **Centralized Game Configuration**: Introduced FGameConfig struct for managing game-wide settings
-   **Combat Configuration**: Centralized combat-related values (damage, ranges, timing) in configuration system
-   **Configurable UI Elements**: Made UI colors, thresholds, and display settings configurable

#### Visual Effects

-   **Attack Particle Effects**: Added Niagara particle effects for attack impacts
-   **Enhanced Visual Feedback**: Improved visual feedback for combat interactions

### Changed

#### AI System Refactoring

-   **Removed Alert Mechanism**: Eliminated global alert system in favor of individual detection
-   **Behavior Tree Optimization**: Streamlined BTTask_FindPlayer to handle patrol and detection in one task
-   **Boss Area Triggers**: Enhanced boss area activation system with improved trigger logic

#### Gameplay Ability System (GAS) Integration

-   **Attribute Management**: Fully integrated GAS for health and stamina management across all characters
-   **Ability Components**: Refactored attribute components to use GAS attributes instead of simple floats
-   **Damage Effects**: Improved damage application through GAS effects

#### Map & Environment

-   **Map Switch**: Changed default game map from Arena to UndergroundSciFi_Demo for better environment showcase
-   **Asset Integration**: Updated asset references for UndergroundSciFi environment

#### Code Architecture

-   **Modular Initialization**: Added helper methods for GAS initialization to prevent re-initialization
-   **Code Cleanup**: Removed unused classes (BTTask_Alert, BTTask_Patrol, MyBossAttackNotify)
-   **Attribute System**: Removed BaseDamage attribute and streamlined attribute management
-   **Knockback Mechanics**: Added knockback effects for enemies on attack hits

#### Build System

-   **Dependencies**: Added Niagara and NavigationSystem dependencies to build configuration
-   **Asset Management**: Updated .gitignore for new asset types

### Technical Details

-   **Engine**: Unreal Engine 5.6
-   **Language**: C++ 17
-   **License**: MIT
-   **Plugins**: GameplayAbilities, AbilitySystemGameFeatureActions, GameplayGraph, GameplayInteractions, GameplayBehaviorSmartObjects, GameplayStateTree, ModelingToolsEditorMode, Niagara

## [0.0.1] - 2025-10-03

### Added

#### Combat System

-   Martial arts mechanics with traditional Korean martial arts combat and fluid animations
-   Combo system with timing-based inputs for dynamic gameplay
-   Boss combo mechanics with advanced attack patterns
-   Stun mechanics for interrupting enemy attacks
-   Robust health management and damage calculation systems
-   Custom gameplay abilities for attacks, effects, and character progression
-   Boss encounters with spear attacks and random attack selection
-   Animation notifies for combo windows and damage application

#### AI & Enemies

-   Behavior trees for intelligent enemy AI with patrol, alert, and attack behaviors
-   Configurable detection ranges and alerting mechanisms
-   Boss areas with unique mechanics, spear attacks, and triggers
-   Diverse enemy classes with different combat styles
-   Stun vulnerabilities for strategic combat
-   Camera lock and targeting system for precise enemy targeting

#### Animation & Visuals

-   Integration of FreeAnimationLibrary for diverse character movements
-   Motion matching for smooth transitions between combat states
-   Custom animation notifies for gameplay events

#### Technical Implementation

-   Modular C++ architecture with reusable components
-   Gameplay Ability System (GAS) integration for abilities and attributes
-   Ability System Component for character capabilities
-   Attribute Set for health, stamina, and movement attributes
-   GameplayEffect assets for ability modifications
-   Performance optimization for efficient rendering and asset management
-   Optimized animation handling via Behavior Tree services
-   Cross-platform build configuration for Windows
-   Custom VS Code tasks for streamlined development workflow

#### UI System

-   Loading screens for level transitions
-   Player UI for health and stamina display
-   Pause menu with resume/quit functionality

#### Game Features

-   RPG elements with character progression framework
-   Interactive environments with responsive objects
-   Arena gameplay for testing combat mechanics
-   Boss areas for challenging encounters
-   Stun mechanics for strategic combat interruptions

#### Build System

-   Multiple build configurations (Debug, DebugGame, Development, Test, Shipping)
-   Separate editor build configurations
-   Automated build tasks for different platforms
-   Clean and rebuild options for all configurations

#### Project Structure

-   Source code organization with Public/Private directories
-   Content organization with Maps, Core assets, and asset packs
-   Configuration files for editor, engine, game, gameplay tags, and input
-   Asset integration for KoreanTraditionalMartialArts and FreeAnimationLibrary

### Technical Details

-   **Engine**: Unreal Engine 5.6
-   **Language**: C++ 17
-   **License**: MIT
-   **Plugins**: GameplayAbilities, AbilitySystemGameFeatureActions, GameplayGraph, GameplayInteractions, GameplayBehaviorSmartObjects, GameplayStateTree, ModelingToolsEditorMode

### Controls

-   Movement: WASD
-   Camera: Mouse
-   Jump: Left Ctrl
-   Sprint: Left Shift
-   Attack: Left Mouse Button
-   Dodge: Space
-   Focus/Target Lock: Right Mouse Button
-   Pause Menu: Escape

[0.1.0]: https://github.com/ngmitam/ActionRPG/releases/tag/0.1.0
[0.0.1]: https://github.com/ngmitam/ActionRPG/releases/tag/0.0.1
