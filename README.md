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
- **Player Profile (Persistence)**: Player data is saved and loaded via JSON including Games played, Max treasure collected, Rooms completed, and Last played timestamp
- **Game Runner**: CLI-based launcher with configurable inputs

## Extended Features Implemented
- **Collect All Treasure**: The game includes a global victory condition after the player has collected every treasure in the world displayed in the UI (e.g., 15 / 20 treasures collected), Upon collecting all treasure the game ends with a victory screen and displays final stats and player profile
- **Enhanced UI**: Improved visual experience using color-coded elements for better readability and distinct representations for player, walls, treasure, and portals
- **Mini-Map System**: Displays the entire dungeon layout as a graph showing the current player position, visited rooms, and connections between rooms, updating dynamically as the player explores

## Architecture: MVC Pattern
- **Model**: Player, game state wrappers, interfaces with C backend via bindings
- **View**: GameUI (curses rendering, input handling)
- **Controller**: GameEngine (game flow, logic coordination)

### C Integration
- Core game logic handled in C while Python communicates using ctypes, ensuring performance and separation of concerns

### Persistence (JSON)
- Player profiles stored in /assets, automatically created if missing, updated after every run

### Terminal UI (curses)
- Responsive layout (adapts to terminal size), structured rendering: Header (messages), Game grid, Footer (stats + controls)

## How To Run
1. Prerequisites: Python 3.x and terminal that supports curses (Linux/macOS recommended)
2. Run the Game: From the python/ directory: python run_game.py --config <path_to_config.ini> --profile <path_to_profile.json>
   - (e.g., python run_game.py --config ../assets/world.ini --profile ../assets/player_profile.json)
4. First-Time Setup: If the profile file does not exist uou’ll be prompted to enter your player name, and a new profile will be created automatically

## Controls
- Arrow Keys / WASD -> Move player
- r -> Reset game
- q -> Quit

## Design Highlights
- **Encapsulation**: Clear separation between layers
- **Modularity**: Independent components for testing
- **Error Handling**: C errors mapped to Python exceptions
- **Scalability**: Easy to extend with new features

- April 29th, 2026
- Evan Leonard
  
