#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
#include <random>

#include "Constants.hpp"
#include "Types.hpp"
#include "MathUtils.hpp"
#include "BoardEntity.hpp"
#include "Snake.hpp"
#include "Ladder.hpp"

// ============================================================
//  Board.hpp
//  Manages the game board: tile rendering, entity generation,
//  power-up placement, and the morph/reshuffle animation state.
//
//  Board owns the Snake and Ladder collections via polymorphic
//  pointers so new entity types can be added without touching
//  Game or Renderer.
// ============================================================

class Board {
public:
    explicit Board(GameMode mode);

    // ---- Entity lifecycle ----------------------------------

    /**
     * @brief (Re-)generates all snakes, ladders, and power-ups.
     *
     * Called once at game start and again after every turn in
     * SLITHER_SHIFT and CHAOS modes.  Collision detection ensures
     * no two entities share a start or end square.
     */
    void generateEntities();

    /**
     * @brief Snapshots current positions then calls generateEntities().
     *
     * The captured positions are used by renderEntities() to lerp
     * between old and new layouts during the ENTITY_GROW phase.
     */
    void performReshuffle();

    // ---- Rendering -----------------------------------------

    /**
     * @brief Draws board background, tile grid, square numbers, and power-up badges.
     *
     * @param window  Render target.
     * @param tc      Active theme colours.
     * @param mode    Current game mode (controls power-up badge visibility).
     */
    void renderTiles(sf::RenderWindow& window,
                     const ThemeColors& tc,
                     GameMode           mode) const;

    /**
     * @brief Draws all snakes and ladders, interpolating if a reshuffle morph is active.
     *
     * Snake drawing also receives a slither-wave offset derived from morphPhaseT
     * to animate the body during the reshuffle.
     *
     * @param window  Render target.
     * @param tc      Active theme colours (not used directly, but passed for future use).
     * @param menuT   Accumulated time, used for warning-snake pulsing.
     */
    void renderEntities(sf::RenderWindow& window,
                        const ThemeColors& tc,
                        float              menuT) const;

    // ---- Accessors -----------------------------------------

    const std::vector<std::unique_ptr<Snake>>&  snakes()  const { return m_snakes;  }
    const std::vector<std::unique_ptr<Ladder>>& ladders() const { return m_ladders; }

    std::map<int, PowerUp>&       powerUps()       { return m_powerUps; }
    const std::map<int, PowerUp>& powerUps() const { return m_powerUps; }

    /// Returns the entity (snake or ladder) whose start square equals sq,
    /// or nullptr if none is found.
    BoardEntity* entityAtStart(int sq) const;

    // ---- Morph animation -----------------------------------

    void  advanceMorph(float dt);

    /// Clears the cached pre-reshuffle positions used during morph interpolation.
    /// Call this when advancing to the next turn after a reshuffle completes.
    void clearMorphSnapshot();

    float morphT()      const { return m_morphT;      }
    float morphPhaseT() const { return m_morphPhaseT; }
    bool  isMorphing()  const { return m_morphT < 1.f; }

    /// Warning snakes drawn in red to indicate sabotage traps.
    /// Elements are (headSquare, tailSquare) pairs.
    std::vector<std::pair<int,int>> warnSnakes;

    // ---- Setters -------------------------------------------
    void setMode(GameMode mode) { m_mode = mode; }
    void setTheme(BoardTheme t) { m_theme = t;   }

private:
    GameMode  m_mode;
    BoardTheme m_theme = BoardTheme::FOREST;

    std::vector<std::unique_ptr<Snake>>  m_snakes;
    std::vector<std::unique_ptr<Ladder>> m_ladders;
    std::map<int, PowerUp>               m_powerUps;

    // Old world-space positions captured before the last reshuffle,
    // used for morph interpolation.
    std::vector<PositionPair> m_oldSnakePos;
    std::vector<PositionPair> m_oldLadderPos;

    // Morph animation state
    float m_morphT      = 1.f;  // 0 = start of morph, 1 = complete
    float m_morphPhaseT = 0.f;  // accumulated wave phase for slither animation

    std::mt19937 m_rng;

    // ---- Internal helpers ----------------------------------
    bool squareAlreadyTaken(int start, int end,
                            const std::vector<int>& usedStarts,
                            const std::vector<int>& usedEnds) const;
};
