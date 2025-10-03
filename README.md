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
-   **Combo System**: Chain attacks with timing-based inputs for dynamic gameplay
-   **Damage & Health**: Robust health management and damage calculation systems
-   **Ability System**: Custom gameplay abilities for attacks, effects, and character progression

### AI & Enemies

-   **Behavior Trees**: Intelligent enemy AI with patrol, alert, and attack behaviors
-   **Boss Encounters**: Special boss areas with unique mechanics and triggers
-   **Enemy Types**: Diverse enemy classes with different combat styles

### Animation & Visuals

-   **Advanced Animation Library**: Integrated FreeAnimationLibrary for diverse character movements
-   **Motion Matching**: Smooth transitions between combat states
-   **Particle Effects**: Visual feedback for attacks and impacts
-   **Animation Notifies**: Custom notifies for combo windows and damage application

### Technical Implementation

-   **Modular Architecture**: Clean C++ code structure with reusable components
-   **Performance Optimization**: Efficient rendering and asset management
-   **Cross-Platform Builds**: Configured for Windows development and deployment
-   **Build Automation**: Custom VS Code tasks for streamlined development workflow
-   **UI System**: Loading screens, player UI, and pause menus

### Game Features

-   **RPG Elements**: Character progression and skill trees
-   **Interactive Environments**: Dynamic world with responsive objects
-   **Arena Gameplay**: Dedicated combat arena for testing mechanics

## 🛠️ Prerequisites

-   **Unreal Engine 5.6+**: Download via [Epic Games Launcher](https://www.unrealengine.com/en-US/download)
-   **Git**: For repository management
-   **Required Assets**:
    -   KoreanTraditionalMartialArts
    -   FreeAnimationLibrary

## 📦 Installation

1. **Clone Repository**

    ```bash
    git clone https://github.com/ngmitam/ActionRPG.git
    cd ActionRPG
    ```

2. **Install Asset Packs**

    - Open Epic Games Launcher → Marketplace
    - Acquire and add to project: `KoreanTraditionalMartialArts` & `FreeAnimationLibrary`

3. **Open Project**
    - Launch `ActionRPG.uproject` in Unreal Editor
    - Generate Visual Studio files if needed

## 📁 Project Structure

```
ActionRPG/
├── Source/                          # C++ source code
│   ├── ActionRPG/                   # Main game module
│   │   ├── Public/                  # Header files (.h)
│   │   │   ├── MyCharacter.h        # Player character class
│   │   │   ├── MyEnemy.h           # Enemy base class
│   │   │   ├── MyBoss.h            # Boss enemy class
│   │   │   ├── MyAbilitySystemComponent.h  # GAS component
│   │   │   └── ...                 # Other classes
│   │   └── Private/                # Implementation files (.cpp)
│   ├── ActionRPG.Target.cs         # Game target configuration
│   └── ActionRPGEditor.Target.cs   # Editor target configuration
├── Content/                        # Unreal assets
│   ├── Maps/Arena/                 # Game levels
│   ├── Core/                       # Core game assets
│   │   ├── AI/                     # AI-related assets and behaviors
│   │   │   ├── Animations/         # AI animation assets
│   │   │   ├── Boss/               # Boss enemy assets
│   │   │   ├── Enemy/              # Regular enemy assets
│   │   │   └── UI/                 # AI-related UI elements
│   │   ├── Character/              # Player character assets
│   │   │   ├── Animations/         # Character animation assets
│   │   │   ├── GAS/                # Gameplay Ability System assets
│   │   │   ├── Input/              # Input mappings and actions
│   │   │   └── UI/                 # Character-related UI elements
│   │   ├── BP_LoadingScreen.uasset # Loading screen Blueprint
│   │   ├── BP_MyGameMode.uasset    # Game mode Blueprint
│   │   └── BP_PauseMenu.uasset     # Pause menu Blueprint
│   ├── KoreanTraditionalMartialArts/  # Combat animations
│   └── FreeAnimationLibrary/       # General animations
├── Binaries/                       # Compiled executables
├── Plugins/                        # Unreal plugins
├── Saved/                          # Editor and runtime data
└── ActionRPG.uproject              # Project file
```

## 🎮 Usage

-   Open in Unreal Editor
-   Press Play to test gameplay
-   Use build tasks for different configurations

## 🔨 Building

Available VS Code tasks:

-   `ActionRPG Win64 Development Build` - Standard development build
-   `ActionRPG Win64 Shipping Build` - Optimized release build
-   `ActionRPGEditor Win64 Development Build` - Editor build

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

## 🙏 Acknowledgments

-   Epic Games for Unreal Engine 5
-   Asset creators for KoreanTraditionalMartialArts and FreeAnimationLibrary
-   Unreal Engine community for invaluable resources

## 📞 Contact

**Nguyen Minh Tam**
[LinkedIn](https://linkedin.com/in/ngmitam) | [GitHub](https://github.com/ngmitam)
Email: ngmitamit@gmail.com

---

_This project showcases professional game development practices and is part of my portfolio demonstrating Unreal Engine expertise._
