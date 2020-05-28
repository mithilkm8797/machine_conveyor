# Description and Controls
This plugin adds a C++ class that simulates VR input and camera movement. For this a Player Pawn (SimulatorPawn) is provided, which add two virtual controllers that can be moved and rotated with the mouse. To switch from camera movement to controller movement either tab through the components or choose them explicitly through the keys 1, 2 and 3.
Additionally, the controller keys `Trigger`, `Menu` and `Grip` are given via events. All standard keys can be remapped in the project settings.

Extend this plugin freely to fit your needs! It only provides a base for your interactions.

# Contents

| Item | Name | Description |
| ---- | ---- | ----------- |
| SimulatorPawn | VRSimulatorPawn.cpp & VRSimulatorPawn.h | The Pawn class that provides two controllers and a camera. It handles input events and should be the base for your pawn |
| TutorialPawn | TutorialPawn.uasset | A blueprint Pawn that derives from `SimulatorPawn`, which shows a simple pick up and let go action.|
| Vive Controller Model | ControllerModel.uasset & ControllerMaterial.uasset  | This model is copied from the engine content together with its material |
| VRSimulator Module | VRSimulator.cpp & VRSimulator.h | Class that is executed on plugin load. Registers Actions (Keys) is they are not already registered |

# Usage / Installation
Use this plugin in your project to simulate VR hardware. And derive your pawn from the SimulatorPawn class. An example of this can be seen in the TutorialPawn. Make sure the chosen pawn is used by creating a gamemode, choosing your pawn and choosing the gamemode.

### Copy Repo (Default in your projects)
Download this Repository as ZIP and unzip it into the `Plugins` directory of your project. This allows you to easily change this plugin on your own. 
### Git Submodule
Execute `git submodule add https://devhub.vr.rwth-aachen.de/VR-Lab/2020/vr-simulator.git Plugins/VRSimulator` in your projects root. This allows you to a give your advanced simulator back and get updates from us easily if something changes.
