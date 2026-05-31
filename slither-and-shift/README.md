# Slither & Shift — Premium Edition

A polished, feature-rich Snakes & Ladders game built with **C++17** and **SFML 2.5**.  
Supports 2–4 players with multiple game modes, animated board reshuffling, a sabotage system, and four distinct visual themes.

---

## Table of Contents

1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Building](#building)
5. [Running](#running)
6. [Controls](#controls)
7. [Gameplay](#gameplay)
8. [Game Modes](#game-modes)
9. [Project Structure](#project-structure)
10. [Class Diagram](#class-diagram)
11. [Architecture Decisions](#architecture-decisions)

---

## Features

- **2–4 player local multiplayer** with distinct animal tokens (Cat, Dog, Rabbit, Fox)
- **Three game modes**: Classic, Slither & Shift, Chaos
- **Animated movement**: step-by-step token hops, Bezier-curve snake slides, ladder climbs
- **Board reshuffle animation**: snakes and ladders smoothly morph to new positions each turn
- **Power-up system** (Chaos mode): Shield and Double-Roll pickups
- **Sabotage mechanic** (Chaos mode): trap opponents with a hidden snake
- **Four visual themes**: Enchanted Forest, Sunbaked Dunes, Deep Abyss, Neon Twilight
- **Bounce-back rule**: overshooting square 100 sends you backwards
- **Per-player statistics**: snakes hit, ladders climbed, sabotages used

---

## Requirements

| Dependency | Version  | Notes                                |
|------------|----------|--------------------------------------|
| C++ compiler | C++17  | GCC 9+, Clang 10+, or MSVC 2019+    |
| CMake      | 3.16+    |                                      |
| SFML       | 2.5.x    | Graphics, Window, System components  |

A font file (`arial.ttf` or a Liberation Sans / Helvetica fallback) must be accessible at runtime. The game tries several common system paths automatically.

---

## Installation

### Ubuntu / Debian
```bash
sudo apt-get update
sudo apt-get install cmake libsfml-dev
```

### macOS (Homebrew)
```bash
brew install cmake sfml
```

### Windows
1. Download SFML 2.5 from [sfml-dev.org](https://www.sfml-dev.org/download.php)
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Set `SFML_DIR` to `<sfml-root>/lib/cmake/SFML` when configuring

---

## Building

```bash
# 1. Clone / extract the project
cd slither-and-shift

# 2. Create a build directory
mkdir build && cd build

# 3. Configure
cmake ..

# 4. Build
cmake --build . --config Release

# On Windows with a specific SFML path:
cmake .. -DSFML_DIR="C:/SFML-2.5.1/lib/cmake/SFML"
cmake --build . --config Release
```

---

## Running

```bash
# From the build directory:
./SlitherAndShift        # Linux / macOS
SlitherAndShift.exe      # Windows
```

Ensure `arial.ttf` (or a compatible font) is present alongside the executable, **or** that one of the system fallback paths is available:

- `/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf` (Linux)
- `/System/Library/Fonts/Helvetica.ttc` (macOS)
- `C:\Windows\Fonts\arial.ttf` (Windows)

---

## Controls

| Key / Input      | Action                                  |
|------------------|-----------------------------------------|
| **Space**        | Roll the dice (during your turn)        |
| **R**            | Restart the current game                |
| **Escape**       | Cancel sabotage target selection        |
| **Left click**   | Navigate menus, pick targets, click buttons |

---

## Gameplay

1. Each player starts on square **1** and races to reach square **100**.
2. On your turn press **Space** to roll the dice. Your token hops square-by-square to the result.
3. **Ladders** lift you to a higher square. **Snakes** slide you to a lower one.
4. **Bounce-back**: rolling past 100 sends you backwards by the excess (e.g. roll 7 from square 97 → land on 90).
5. The first player to land on or pass square **100** wins.

---

## Game Modes

### Classic
Standard Snakes & Ladders. Board layout is fixed for the entire game.

### Slither & Shift
After **every turn** all snakes and ladders relocate to new random positions. Watch the board reshape itself with a smooth morph animation — including snake body slithering — between turns.

### Chaos Mode
Everything in Slither & Shift **plus**:

| Feature        | Description |
|----------------|-------------|
| **Shield**     | Collected from special tiles. Blocks one snake bite *or* one sabotage attempt. |
| **x2 Roll**    | Collected from special tiles. Rolls two dice on your next turn — values are summed. |
| **Sabotage**   | Spend your turn to drop an opponent's token by 12–28 squares on *their* next turn. If they have a Shield it absorbs the hit instead. |

---

## Project Structure

```
slither-and-shift/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
│
├── include/                # Public headers (one per module)
│   ├── Constants.hpp       # constexpr constants, enums, theme/animal lookup tables
│   ├── Types.hpp           # Shared data structures (Player, EntityData, PositionPair, …)
│   ├── MathUtils.hpp       # Inline geometry + colour helpers (vlen, cubicBez, easeInOut, …)
│   ├── BoardEntity.hpp     # Abstract base class for Snake and Ladder
│   ├── Snake.hpp           # Concrete snake entity (derives BoardEntity)
│   ├── Ladder.hpp          # Concrete ladder entity (derives BoardEntity)
│   ├── Renderer.hpp        # Free-function drawing interface (tokens, snakes, ladders, dice)
│   ├── Dice.hpp            # Dice roll-animation state machine
│   ├── Board.hpp           # Board: entity generation, reshuffle, tile + entity rendering
│   └── Game.hpp            # Top-level application class (window, state machine, main loop)
│
└── src/                    # Implementation files
    ├── main.cpp            # Entry point
    ├── Snake.cpp           # Snake::palette() + Snake::draw()
    ├── Ladder.cpp          # Ladder::palette() + Ladder::draw()
    ├── Renderer.cpp        # All SFML drawing primitives and composite renderers
    ├── Dice.cpp            # Dice::start() + Dice::update()
    ├── Board.cpp           # Entity generation, morph animation, tile/entity rendering
    └── Game.cpp            # State machine, event handling, update, all render sub-routines
```

---

## Class Diagram

```
┌─────────────────────────────────────────────────────────┐
│                        Game                             │
│  ─────────────────────────────────────────────────────  │
│  - m_window : sf::RenderWindow                          │
│  - m_board  : Board                                     │
│  - m_dice   : Dice                                      │
│  - m_players: vector<Player>                            │
│  - m_state  : GameState  (enum)                         │
│  - m_mode   : GameMode   (enum)                         │
│  - m_animPhase: AnimPhase (enum)                        │
│  ─────────────────────────────────────────────────────  │
│  + run()                                                │
│  - handleEvents() / update(dt) / render()               │
│  - startGame() / finalizeRoll() / advanceTurn()         │
│  - applySabotage() / startMorphPhase()                  │
└─────────────────────────────────────────────────────────┘
             │ owns                        │ owns
             ▼                             ▼
┌────────────────────────┐      ┌──────────────────────┐
│         Board          │      │         Dice          │
│  ──────────────────────│      │  ────────────────────  │
│  - m_snakes  : unique_ptr[]   │  - rolling : bool     │
│  - m_ladders : unique_ptr[]   │  - faceA/B : int      │
│  - m_powerUps: map<int,PU>    │  - finalTotal: int    │
│  ──────────────────────│      │  ────────────────────  │
│  + generateEntities()  │      │  + start(doubleRoll)  │
│  + performReshuffle()  │      │  + update(dt, double) │
│  + renderTiles()       │      └──────────────────────┘
│  + renderEntities()    │
│  + advanceMorph(dt)    │
└────────────────────────┘
       │ owns (polymorphic)
       ▼
┌─────────────────────────────────┐
│         BoardEntity (abstract)  │
│  ───────────────────────────────│
│  # m_start : int                │
│  # m_end   : int                │
│  ───────────────────────────────│
│  + start() / end()              │
│  + isSnake() = 0                │
│  + draw(...) = 0                │
└─────────────────────────────────┘
              ▲
     ┌────────┴─────────┐
     │                  │
┌────────────┐    ┌────────────┐
│   Snake    │    │   Ladder   │
│  ──────────│    │  ──────────│
│  isSnake() │    │  isSnake() │
│  draw()    │    │  draw()    │
│  palette() │    │  palette() │
└────────────┘    └────────────┘


Free functions (Renderer.hpp / .cpp)
─────────────────────────────────────
drawThickSeg()       — primitive line segment
drawAnimal()         — dispatches to drawCat/Dog/Rabbit/Fox
drawSnakeFull()      — Bezier-curved snake with shadow + head
drawLadderFull()     — two rails with rungs + shadow
drawDiceFace()       — die face with pips + highlight

Inline utilities (MathUtils.hpp)
─────────────────────────────────────
vlen / vnorm         — vector length + normalisation
cubicBez()           — cubic Bezier evaluation
easeInOut()          — smooth-step easing
easeOutBack()        — overshoot easing
withAlpha / lerpColor / lerpV
squareToCentre()     — board-square number → pixel centre
snakeBezCP()         — snake Bezier control point calculation
```

---

## Architecture Decisions

- **`BoardEntity` hierarchy** — `Snake` and `Ladder` derive from a common abstract base so the `Board` can store them in a single `vector<unique_ptr<BoardEntity>>` sub-list and call `draw()` polymorphically without switching on a type flag.

- **Free-function renderer** — Drawing code lives in `Renderer.hpp/.cpp` as free functions rather than methods, keeping SFML knowledge out of the game-logic classes and making individual routines independently testable.

- **Inline `MathUtils.hpp`** — Geometry helpers are small, called frequently, and have no state, so they are defined `inline` in the header to allow inlining at call sites without an extra translation unit.

- **`Board` owns reshuffle state** — The morph-lerp snapshot (`m_oldSnakePos`, `m_oldLadderPos`) and animation parameters (`m_morphT`, `m_morphPhaseT`) live in `Board` rather than `Game`. This keeps the "how to draw an animated reshuffle" concern self-contained.

- **`constexpr` constants** — All "magic numbers" (tile size, grid size, animation durations, token size) are `constexpr` in `Constants.hpp`. Changing the board from 10×10 to another size, or tweaking animation timing, requires editing exactly one file.

- **`std::unique_ptr` for entities** — Smart pointers ensure entities are destroyed automatically and support the polymorphic `BoardEntity` base without slicing.
