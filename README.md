# Tower Defense — OOP Semester Project
### FAST NUCES | Object Oriented Programming

A fully-featured Tower Defense game built in C++ using the raylib graphics library, developed as a semester project demonstrating Object Oriented Programming concepts including Inheritance, Polymorphism, Encapsulation, and Operator Overloading.

---

## Gameplay Overview

Place towers on the map to stop waves of enemies from reaching the exit. Earn gold by defeating enemies and spend it to buy or upgrade towers. You lose a life each time an enemy escapes. Survive all 5 waves to win.

---

## Features

- 3 playable maps (S-Curve, Zigzag, Spiral)
- 4 enemy types: Basic, Fast, Tank, Shielded
- 5 tower types: Cannon, Sniper, Machine Gun, Slow, Bomb
- 5 progressive waves, each harder than the last
- Tower upgrade system (2 levels per tower)
- Gold economy and lives system
- Persistent high score saving
- Background music
- In-game shop with tower descriptions and stats

---

## GUI Library — raylib

This project uses **raylib**, a simple C library for game programming.

### Installing raylib

**Windows:**
1. Download the MinGW installer from [https://github.com/raysan5/raylib/releases](https://github.com/raysan5/raylib/releases)
2. Run the installer — it sets up raylib and MinGW together automatically
3. Alternatively, install via [w64devkit](https://github.com/skeeto/w64devkit) and place raylib headers/libs manually

**macOS:**
```bash
brew install raylib
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install libraylib-dev
```
Or build from source: [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)

---

## How to Compile and Run

### Windows (MinGW / g++)
```bash
g++ main.cpp enemy.cpp tower.cpp map.cpp wave.cpp -std=c++17 -I"C:/raylib/include" -L"C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o TowerDefense.exe
./TowerDefense.exe
```

### macOS
```bash
clang++ main.cpp enemy.cpp tower.cpp map.cpp wave.cpp -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -o TowerDefense
./TowerDefense
```

### Linux
```bash
g++ main.cpp enemy.cpp tower.cpp map.cpp wave.cpp -std=c++17 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o TowerDefense
./TowerDefense
```

> Make sure all `.cpp` and `.h` files are in the same directory before compiling.

---

## How to Play

| Action | Control |
|---|---|
| Select a tower from the shop | Click on the right panel |
| Place a tower | Click on a green (grass) cell |
| Upgrade a tower | Click a placed tower → click Upgrade |
| Start the next wave | Click the Start Wave button |
| Select a map | From the main menu |

- Towers **cannot** be placed on the path
- Each tower has a range circle shown when selected
- Gold is earned by killing enemies; each enemy type gives different gold

---

## OOP Concepts Used

| Concept | Where |
|---|---|
| Abstract base class | `Entity` (pure virtual `update()` and `render()`) |
| Inheritance | `Enemy` and `Tower` inherit from `Entity`; all subtypes inherit from those |
| Runtime Polymorphism | `Enemy*` and `Tower*` base pointers used throughout `Game` struct |
| Encapsulation | All members are `protected`/`private` with public getters |
| Operator Overloading | `<` and `==` overloaded in `Entity` based on HP |
| Constructors/Destructors | All classes initialize via constructors; destructors defined |

---

## Class Hierarchy

```
Entity  (abstract)
├── Enemy  (abstract)
│   ├── BasicEnemy
│   ├── FastEnemy
│   ├── TankEnemy
│   └── ShieldedEnemy
└── Tower  (abstract)
    ├── cannon_tower
    ├── sniper_tower
    ├── machinegun_tower
    ├── slow_tower
    └── bomb_tower
```

---

## Known Issues and Limitations

- The build command requires all source files to be compiled together manually; there is no Makefile or CMake setup included
- Background music and assets must be present in the working directory at runtime or the game runs without them silently
- No `FlyingEnemy` type implemented; `ShieldedEnemy` is used as the custom fifth enemy type instead
- Memory cleanup (deleting heap-allocated enemies, towers, and projectiles) is partially implemented; some edge cases on game reset may leak memory
- The project has only been tested on macOS; Windows and Linux builds should work but are not fully verified

---

## Project Structure

```
├── main.cpp          # Game loop, input handling, rendering orchestration
├── entity.h          # Abstract base class for all game entities
├── enemy.h / .cpp    # Enemy base class and all enemy subtypes
├── tower.h / .cpp    # Tower base class, projectile, and all tower subtypes
├── map.h   / .cpp    # Grid system, path definitions, drawing
├── wave.h  / .cpp    # Wave data and spawn configurations
├── game.h            # Central game state struct
├── constants.h       # Screen dimensions, grid size, array limits
└── README.md
```

---

## Development Notes

Built using C++17. Tested with clang++ on macOS. All graphical rendering is done through raylib's immediate-mode drawing API with no external assets required for core gameplay.
