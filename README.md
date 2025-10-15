# ActionRPG 🗡️⚔️

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.6-blue.svg)](https://www.unrealengine.com/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-orange.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A modern Action RPG built with Unreal Engine 5, showcasing advanced game development techniques, asset integration, and polished gameplay mechanics. This project demonstrates expertise in C++ programming, Blueprint scripting, animation systems, and cross-platform build pipelines.

---

## 🚀 Technologies Used

-   **Unreal Engine 5.6** - Game engine for high-fidelity graphics and advanced features
-   **C++** - Core gameplay logic and performance-critical systems
-   **Blueprints** - Visual scripting for rapid prototyping and UI
-   **Gameplay Ability System (GAS)** - Advanced ability and attribute management
-   **Behavior Trees** - AI decision-making and enemy behaviors
-   **Animation Blueprints** - Character animation state machines
-   **Unreal Build Tool (UBT)** - Custom build configurations and automation
-   **Version Control** - Git for collaborative development
-   **Asset Integration** - Marketplace asset packs for rich content

## ✨ Key Features

### Combat System

-   **Martial Arts Mechanics**: Implemented traditional Korean martial arts combat with fluid animations
-   **Combo System**: Chain attacks with timing-based inputs for dynamic gameplay, including boss combo mechanics
-   **Stun Mechanics**: Enemy stun system for interrupting attacks and creating combat opportunities
-   **Damage & Health**: Robust health management and damage calculation systems
-   **Ability System**: Custom gameplay abilities for attacks, effects, and character progression
-   **Boss Encounters**: Advanced boss fights with spear attacks, random attack selection, and animation notifies

### AI & Enemies

-   **Behavior Trees**: Intelligent enemy AI with patrol, alert, and attack behaviors
-   **Detection Systems**: Configurable detection ranges and alerting mechanisms for responsive AI
-   **Boss Encounters**: Special boss areas with unique mechanics, spear attacks, and triggers
-   **Enemy Types**: Diverse enemy classes with different combat styles and stun vulnerabilities
-   **Targeting System**: Camera lock and input actions for precise enemy targeting

### Animation & Visuals

-   **Advanced Animation Library**: Integrated FreeAnimationLibrary for diverse character movements
-   **Motion Matching**: Smooth transitions between combat states
-   **Particle Effects**: Visual feedback for attacks and impacts
-   **Animation Notifies**: Custom notifies for combo windows and damage application

### Technical Implementation

-   **Modular Architecture**: Clean C++ code structure with reusable components
-   **Performance Optimization**: Efficient rendering and asset management, including optimized animation handling via Behavior Tree services
-   **Cross-Platform Builds**: Configured for Windows development and deployment
-   **Build Automation**: Custom VS Code tasks for streamlined development workflow
-   **UI System**: Loading screens, player UI, pause menus with resume/quit functionality, and centralized input management
-   **Gameplay Ability System (GAS)**: Advanced ability and attribute management with GameplayEffect assets

### Game Features

-   **RPG Elements**: Character progression and skill trees
-   **Interactive Environments**: Dynamic world with responsive objects
-   **Arena Gameplay**: Dedicated combat arena for testing mechanics
-   **Boss Areas**: Specialized zones for challenging boss encounters
-   **Stun Mechanics**: Combat interruptions for strategic gameplay

## 🛠️ Prerequisites

-   **Unreal Engine 5.6+**: Download via [Epic Games Launcher](https://www.unrealengine.com/en-US/download)
-   **Git**: For repository management
-   **Required Assets**:
    -   KoreanTraditionalMartialArts
    -   FreeAnimationLibrary
    -   UndergroundSciFi

## 📦 Installation

1. **Clone Repository**

    ```bash
    git clone https://github.com/ngmitam/ActionRPG.git
    cd ActionRPG
    ```

2. **Install Asset Packs**

    - Open Epic Games Launcher → Marketplace
    - Acquire and add to project: `KoreanTraditionalMartialArts`, `FreeAnimationLibrary`, & `UndergroundSciFi`

3. **Open Project**
    - Launch `ActionRPG.uproject` in Unreal Editor
    - Generate Visual Studio files if needed

## 📁 Project Structure

```
ActionRPG/
├── ActionRPG.code-workspace      # VS Code workspace configuration
├── ActionRPG.uproject            # Unreal Engine project file
├── LICENSE                       # MIT License
├── README.md                     # Project documentation
├── Source/                       # C++ source code
│   ├── ActionRPG/                # Main game module
│   │   ├── ActionRPG.Build.cs    # Build configuration
│   │   ├── ActionRPG.cpp         # Module implementation
│   │   ├── ActionRPG.h           # Module header
│   │   ├── Public/               # Header files (.h)
│   │   │   ├── MyCharacter.h     # Player character class
│   │   │   ├── MyEnemy.h         # Enemy base class
│   │   │   ├── MyBoss.h          # Boss enemy class
│   │   │   ├── MyAbilitySystemComponent.h  # GAS component
│   │   │   └── ...               # Other classes
│   │   └── Private/              # Implementation files (.cpp)
│   ├── ActionRPG.Target.cs       # Game target configuration
│   └── ActionRPGEditor.Target.cs # Editor target configuration
├── Content/                      # Unreal assets
│   ├── Maps/                     # Game levels
│   │   └── Arena/                # Combat arena level
│   ├── Core/                     # Core game assets
│   │   ├── AI/                   # AI-related assets and behaviors
│   │   │   ├── Animations/       # AI animation assets
│   │   │   ├── Boss/             # Boss enemy assets
│   │   │   ├── Enemy/            # Regular enemy assets
│   │   │   └── UI/               # AI-related UI elements
│   │   ├── Character/            # Player character assets
│   │   │   ├── Animations/       # Character animation assets
│   │   │   ├── GAS/              # Gameplay Ability System assets
│   │   │   ├── Input/            # Input mappings and actions
│   │   │   └── UI/               # Character-related UI elements
│   │   ├── FX/                   # Particle effects and visual effects
│   │   ├── BP_LoadingScreen.uasset # Loading screen Blueprint
│   │   ├── BP_MyGameMode.uasset  # Game mode Blueprint
│   │   └── BP_PauseMenu.uasset   # Pause menu Blueprint
│   ├── KoreanTraditionalMartialArts/ # Combat animations
│   ├── FreeAnimationLibrary/     # General animations
│   ├── UndergroundSciFi/         # Sci-fi environment assets
│   ├── Collections/              # Asset collections
│   ├── Developers/               # Developer-specific assets
│   ├── __ExternalActors__/       # External actor references
│   └── __ExternalObjects__/      # External object references
├── Config/                       # Configuration files
│   ├── DefaultEditor.ini         # Editor settings
│   ├── DefaultEngine.ini         # Engine settings
│   ├── DefaultGame.ini           # Game settings
│   ├── DefaultGameplayTags.ini   # Gameplay tags
│   ├── DefaultInput.ini          # Input settings
│   └── Layouts/                  # UI layouts
├── Binaries/                     # Compiled executables
│   ├── Win64/                    # Windows 64-bit builds
├── Plugins/                      # Unreal plugins
├── Saved/                        # Editor and runtime data
│   ├── Autosaves/                # Auto-saved levels
│   ├── Config/                   # Saved configuration
│   ├── Logs/                     # Log files
│   └── Shaders/                  # Compiled shaders
├── DerivedDataCache/             # Cached derived data
├── Intermediate/                 # Intermediate build files
└── .git/                         # Git repository (not shown in project)
```

## 🎮 Usage

### In Editor

-   Open in Unreal Editor
-   Press Play to test gameplay
-   Use build tasks for different configurations

### Packaged Game

-   Navigate to `Binaries/Win64/`
-   Run `ActionRPG.exe` for the packaged game

### Controls

-   **Movement**: WASD
-   **Camera**: Mouse
-   **Jump**: Left Ctrl
-   **Sprint**: Left Shift
-   **Attack**: Left Mouse Button
-   **Dodge**: Space
-   **Focus/Target Lock**: Right Mouse Button
-   **Pause Menu**: Escape

## 🔨 Building

Available VS Code tasks for different build configurations:

### Game Builds

-   `ActionRPG Win64 Debug Build` - Debug build for development debugging
-   `ActionRPG Win64 Debug Rebuild` - Clean and rebuild debug version
-   `ActionRPG Win64 Debug Clean` - Clean debug build files
-   `ActionRPG Win64 DebugGame Build` - DebugGame build for testing
-   `ActionRPG Win64 DebugGame Rebuild` - Clean and rebuild DebugGame version
-   `ActionRPG Win64 DebugGame Clean` - Clean DebugGame build files
-   `ActionRPG Win64 Development Build` - Standard development build
-   `ActionRPG Win64 Development Rebuild` - Clean and rebuild development version
-   `ActionRPG Win64 Development Clean` - Clean development build files
-   `ActionRPG Win64 Test Build` - Test build for automated testing
-   `ActionRPG Win64 Test Rebuild` - Clean and rebuild test version
-   `ActionRPG Win64 Test Clean` - Clean test build files
-   `ActionRPG Win64 Shipping Build` - Optimized release build
-   `ActionRPG Win64 Shipping Rebuild` - Clean and rebuild shipping version
-   `ActionRPG Win64 Shipping Clean` - Clean shipping build files

### Editor Builds

-   `ActionRPGEditor Win64 Debug Build` - Editor debug build
-   `ActionRPGEditor Win64 Debug Rebuild` - Clean and rebuild editor debug
-   `ActionRPGEditor Win64 Debug Clean` - Clean editor debug build files
-   `ActionRPGEditor Win64 DebugGame Build` - Editor DebugGame build
-   `ActionRPGEditor Win64 DebugGame Rebuild` - Clean and rebuild editor DebugGame
-   `ActionRPGEditor Win64 DebugGame Clean` - Clean editor DebugGame build files
-   `ActionRPGEditor Win64 Development Build` - Editor development build
-   `ActionRPGEditor Win64 Development Rebuild` - Clean and rebuild editor development
-   `ActionRPGEditor Win64 Development Clean` - Clean editor development build files

## 📚 What I Learned

This project enhanced my skills in:

-   **Game Architecture**: Designing scalable systems for RPG mechanics
-   **Gameplay Ability System**: Implementing complex ability frameworks
-   **AI Programming**: Behavior trees and enemy AI design
-   **Animation Systems**: Montages, notifies, and state machines
-   **C++ in Unreal**: Advanced UE patterns and performance optimization
-   **Performance Tuning**: Optimizing for smooth 60+ FPS gameplay
-   **Asset Pipeline**: Managing large asset libraries efficiently
-   **Build Systems**: Automating deployment across platforms
-   **Version Control**: Best practices for collaborative game development

## 🤝 Contributing

Open to feedback and improvements! Feel free to:

-   Report issues or bugs
-   Suggest new features or mechanics
-   Submit pull requests with code improvements
-   Share feedback on gameplay balance

For major changes, please open an issue first to discuss what you would like to change.

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

© 2025 Nguyen Minh Tam. All rights reserved.

## 🙏 Acknowledgments

-   Epic Games for Unreal Engine 5
-   Asset creators for KoreanTraditionalMartialArts, FreeAnimationLibrary, UndergroundSciFi
-   Unreal Engine community for invaluable resources

## 📞 Contact

**Nguyen Minh Tam**
[LinkedIn](https://linkedin.com/in/ngmitam) | [GitHub](https://github.com/ngmitam)
Email: ngmitamit@gmail.com

---

_This project showcases professional game development practices and is part of my portfolio demonstrating Unreal Engine expertise._
