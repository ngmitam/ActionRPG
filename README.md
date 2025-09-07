# ActionRPG

A modern Action RPG built with the power of Unreal Engine 5.

---

## Prerequisites

Before you begin, ensure you have the following installed and set up:

-   **Unreal Engine 5:** This project is developed for UE5 (preferably version 5.x). You can download it via the [Epic Games Launcher](https://www.unrealengine.com/en-US/download).
-   **Git:** Required for cloning the repository.
-   **Required Asset Packs:** This project has required dependencies on the following asset packs. See the installation instructions below for how to add them to the project.
    -   **KoreanTraditionalMartialArts**
    -   **FreeAnimationLibrary**

## Installation

Follow these steps to get the project up and running on your local machine.

1.  **Clone the Repository**
    Open your terminal or command prompt and clone this repository to your local machine.

    ```bash
    git clone https://github.com/ngmitam/ActionRPG.git
    cd ActionRPG
    ```

2.  **Install Required Asset Packs**
    The **KoreanTraditionalMartialArts** and **FreeAnimationLibrary** asset packs are required dependencies. You must add them to the project through the Epic Games Launcher.

    a. Open the **Epic Games Launcher**.

    b. Navigate to the **Marketplace / Fab** tab.

    c. Search for and acquire each of the following packs: - `KoreanTraditionalMartialArts` - `FreeAnimationLibrary`

    d. Go to your **Library** (Vault) section within the launcher.

    e. Find an asset pack and click "**Add to Project**".

    f. A new window will appear showing your compatible Unreal Engine projects. Select your cloned `ActionRPG` project from the list and confirm. The launcher will now copy the asset files directly into the project's `Content` directory.

    > **Important:** You must repeat this "Add to Project" process for **both** required asset packs.

3.  **Open the Project**
    Navigate to the project directory where you cloned the repository and open the `ActionRPG.uproject` file. This will launch the project in the Unreal Editor.

    > **Note:** The first time you open the project, Unreal Engine may need to compile shaders, which can take some time.

4.  **Generate Project Files (If using C++)**
    If the project contains C++ code, you may need to right-click the `.uproject` file and select "Generate Visual Studio project files" before opening it.

## Usage

Once the project is open in the Unreal Editor:

1.  Let any background processes (like shader compilation) finish.
2.  Press the **Play** button in the main toolbar to start the game in the editor.
