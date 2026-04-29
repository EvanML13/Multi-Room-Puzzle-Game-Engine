# Project Summary
This academic project, designed for my Software Systems Development and Integration class, implements a terminal-based dungeon exploration game, combining Python (MVC design), C integration, and a curses-based UI. This project expands on a pre-existing engine by introducing persistence, a structured UI, and enhanced gameplay features.
The game is a grid-based dungeon crawler where the player navigates rooms, collects treasure, and explores a connected world. The game is rendered entirely in the terminal using the curses library and integrates with a C backend for core game logic.
The project emphasizes:
- Clean Model–View–Controller (MVC) architecture
- Cross-language integration (Python ↔ C via ctypes)
- Persistent player data using JSON
- Modular, testable design

## Core Features 
- **Movement System**: Navigate rooms using arrow keys or WASD
- **Curses-Based UI**: Fully interactive terminal interface with a Message bar (feedback and events), Game grid display, Player status bar (stats, progress), and Splash screens (start + end) 
