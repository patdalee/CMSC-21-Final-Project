#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

// ============================================================
//  Constants.hpp
//  All compile-time constants, enums, and global lookup tables.
//  Replace "magic numbers" throughout the code with these names.
// ============================================================

// ---- Board geometry ----------------------------------------
constexpr int GRID_SIZE     = 10;    // cells per row/column
constexpr int TILE_SIZE     = 72;    // pixels per tile
constexpr int TOTAL_SQUARES = 100;   // 10x10 board

constexpr int NUM_SNAKES  = 6;
constexpr int NUM_LADDERS = 6;

// Board top-left corner offset from window origin
constexpr int BOARD_X   = 45;
constexpr int BOARD_Y   = 35;
constexpr int SIDEBAR_W = 360;      // pixel width of right-hand sidebar

// Derived window size
constexpr int WIN_W = BOARD_X + GRID_SIZE * TILE_SIZE + SIDEBAR_W + 35;
constexpr int WIN_H = BOARD_Y + GRID_SIZE * TILE_SIZE + 70;

// ---- Player token ------------------------------------------
constexpr float TOKEN_SIZE = 17.f;  // radius in pixels

// Sub-tile offsets so multiple tokens on the same square don't overlap.
// Arranged in a 2x2 grid relative to the tile centre.
const static sf::Vector2f SLOT_OFFSETS[4] = {
    {-12.f, -12.f}, { 12.f, -12.f},
    {-12.f,  12.f}, { 12.f,  12.f}
};

// ---- Animation timing defaults -----------------------------
constexpr float HOP_DURATION      = 0.10f;  // seconds per step-hop
constexpr float SLIDE_DURATION    = 0.80f;  // seconds for snake/ladder slide
constexpr float MORPH_DURATION    = 0.65f;  // seconds for board reshuffle morph
constexpr float SABOTAGE_PAUSE    = 1.20f;  // seconds of pause after sabotage
constexpr float MORPH_WAVE_SPEED  = 3.5f;   // rad/s of slither wave during reshape

// ---- Game state machine ------------------------------------
enum class GameState {
    SELECT_SCREEN,  // main menu
    HOW_TO_PLAY,    // rules screen
    PLAYING,        // active game
    GAME_OVER       // winner declared
};

// ---- Game mode variants ------------------------------------
enum class GameMode {
    CLASSIC,        // fixed board, no special rules
    SLITHER_SHIFT,  // board reshuffles after each turn
    CHAOS           // reshuffle + power-ups + sabotage
};

// ---- Chaos-mode power-ups ----------------------------------
enum class PowerUp {
    NONE,
    SHIELD,      // blocks one snake bite or one sabotage
    DOUBLE_ROLL  // rolls two dice next turn
};

// ---- Visual theme of the board -----------------------------
enum class BoardTheme {
    FOREST,
    DESERT,
    OCEAN,
    NIGHT
};

// ---- Player token animal type ------------------------------
enum class AnimalType {
    CAT,
    DOG,
    RABBIT,
    FOX
};

// ---- Animation phase state machine -------------------------
// Controls which kind of interpolation is running this frame.
enum class AnimPhase {
    IDLE,           // waiting for input
    STEP_HOP,       // token is hopping square-by-square
    ENTITY_SLIDE,   // token is sliding along a snake or ladder path
    ENTITY_GROW,    // board is reshuffling (morph lerp plays out)
    SABOTAGE_PAUSE  // short pause after sabotage resolves before turn advances
};

// ============================================================
//  Theme colour palettes
//  One ThemeColors entry per BoardTheme value.
// ============================================================
struct ThemeColors {
    sf::Color   lightTile, darkTile, boardBorder;
    sf::Color   bgTop, bgBottom, accent, sidebarBg;
    std::string name;
};

// Global lookup table – keyed by BoardTheme
inline const std::map<BoardTheme, ThemeColors> THEMES = {
    { BoardTheme::FOREST, { {210,235,185},{140,185,110},{80,130,60},
                            {20,32,18},{38,60,30},{95,215,100},{22,40,28},
                            "Enchanted Forest" } },
    { BoardTheme::DESERT, { {250,235,200},{225,195,150},{180,135,90},
                            {50,38,28},{78,58,42},{240,185,100},{55,42,30},
                            "Sunbaked Dunes" } },
    { BoardTheme::OCEAN,  { {195,228,248},{140,195,220},{65,115,158},
                            {18,28,48},{28,48,78},{80,200,240},{18,30,55},
                            "Deep Abyss" } },
    { BoardTheme::NIGHT,  { {120,120,155},{80,80,115},{55,55,90},
                            {12,12,28},{22,22,48},{175,115,225},{15,15,38},
                            "Neon Twilight" } }
};

// Display names for each animal token
inline const std::map<AnimalType, std::string> ANIMAL_NAMES = {
    { AnimalType::CAT,    "Cat"    },
    { AnimalType::DOG,    "Dog"    },
    { AnimalType::RABBIT, "Rabbit" },
    { AnimalType::FOX,    "Fox"    }
};
