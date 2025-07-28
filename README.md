# SushiRestaurant
A game test for NG+
Multiplayer cooking game prototype built in Unreal Engine 5.6+ as part of a technical assignment. Inspired by Overcooked, players must prepare and deliver sushi orders within a time limit using clean, modular C++ systems and replicated gameplay logic.

## Gameplay Overview

- Players control characters in a sushi restaurant.
- Orders are randomly generated and must be fulfilled before time runs out.
- Players collect, process, and deliver ingredients (e.g., slice fish, combine sushi).
- Game ends if any order fails to be completed on time.
- All major logic is written in C++ with minimal use of Blueprints.

## Features

- Third-person player character with Enhanced Input System (C++).
- Modular interaction system using line tracing and interface-driven logic.
- Ingredient actors with type and state enums, supporting interaction.
- Organized asset and code structure for clarity and scalability.
- Ready for multiplayer with Listen Server setup.

## Systems Implemented

- `ASushiRestaurantCharacter`: Handles movement, camera, and interaction input.
- `UInteractionComponent`: Reusable line trace interaction logic.
- `IInteractable`: Interface used for all interactable objects.
- `AIngredientActor`: Interactable ingredients with states (Raw, Sliced, Cooked, etc.).

More systems (cookware, recipe validation, order timer) are in progress.

## Project Structure

### Code
All C++ classes are inside:
Source/SushiRestaurant/
├── ActorComponent/
├── Actors/
├── Interface/

shell
Copiar
Editar

### Assets
All assets are organized under:

Content/SushiRestaurant/
├── Main Character/
├── Actors/
├── Environment/
├── Decoration/
├── Input/

shell
Copiar
Editar

## How to Run

### Host (Listen Server)
Open console and type:

open SushiRestaurant?listen

bash
Copiar
Editar

### Join (Client)
From another instance of the game, type:

open 127.0.0.1

yaml
Copiar
Editar

Note: Replace `127.0.0.1` with your host's IP address when testing over LAN.

## Author Notes

This project was built with focus on clean C++ architecture, Unreal Engine replication, and editor/asset hygiene. All logic shown is original and created within the assignment time window.

## Build Instructions

1. Open with Unreal Engine 5.6+
2. Compile the C++ code
3. Press `Play` in Editor or package for Windows
4. Use console commands to test multiplayer

---

© 2025 Roberto Jorge Cruz Soto - For technical assessment purposes only