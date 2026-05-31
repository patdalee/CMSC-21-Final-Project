#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

// ============================================================
//  Types.hpp
//  Plain data structures shared across multiple modules.
//  Keeping them here avoids circular include chains.
// ============================================================

// ---- Colour palettes for rendered entities -----------------

/// Colour palette for drawing a single snake (body, belly stripe, head).
struct SnakePal {
    sf::Color body, belly, head;
};

/// Colour palette for drawing a ladder (rails and rungs).
struct LadderPal {
    sf::Color rail, rung;
};

// ---- Player data -------------------------------------------

/**
 * @brief Holds all mutable state for a single player.
 *
 * Initialised by Game::startGame() and updated throughout the match.
 * Power-up flags (hasShield, hasDoubleRoll) are only meaningful in
 * GameMode::CHAOS.
 */
struct Player {
    int        id             = 0;
    int        currentSquare  = 1;
    sf::Color  color;
    AnimalType animal         = AnimalType::CAT;

    // ---- Statistics ----------------------------------------
    int        snakesHit      = 0;
    int        laddersClimbed = 0;
    int        sabotagesUsed  = 0;

    // ---- Chaos-mode state ----------------------------------
    bool       hasShield      = false;
    bool       hasDoubleRoll  = false;

    /// True when this player has been sabotaged and must slide
    /// at the start of their next turn.
    bool       isSabotaged    = false;

    /// The square the player will slide to if isSabotaged is true.
    int        sabotageFallTo = 0;
};

// ---- Board entity snapshot (used during morph animation) ---

/**
 * @brief A start/end world-position pair captured before a reshuffle.
 *
 * Used to linearly interpolate between old and new entity positions
 * during the ENTITY_GROW animation phase.
 */
struct PositionPair {
    sf::Vector2f startPos;
    sf::Vector2f endPos;
};

// ---- Raw entity data (shared by Snake and Ladder) ----------

/**
 * @brief Minimal data describing one snake or ladder on the board.
 *
 * The BoardEntity class wraps this with virtual draw behaviour.
 * It is kept as a plain struct so Board can store homogeneous vectors
 * before the class hierarchy is needed.
 */
struct EntityData {
    int  start  = 0;   ///< Square where the entity begins (head/base)
    int  end    = 0;   ///< Square where the entity ends (tail/top)
    bool isSnake = true;
};
