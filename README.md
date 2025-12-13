# Smart_Plant_Pot

Smart Plant Pot firmware repository.

This repository follows a **monorepo** structure and contains multiple
ESP-IDF firmware projects related to a smart plant pot system.

## Structure
- `Node/`    – ESP32 firmware for plant pot sensor nodes
- `Gateway/` – ESP32 / gateway firmware (WIP)

## Notes
- Each folder is an independent ESP-IDF project
- Single Git repository, no submodules
- Build and flash per project directory
