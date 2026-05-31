#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Types.hpp"
#include "MathUtils.hpp"

// ============================================================
//  BoardEntity.hpp
//  Abstract base class for Snake and Ladder.
//
//  Provides the shared start/end square data and declares the
//  polymorphic draw() interface.  Derive Snake and Ladder from
//  this to enable uniform storage in a single container and
//  uniform rendering in a range-based loop.
// ============================================================

class BoardEntity {
public:
    BoardEntity(int startSquare, int endSquare)
        : m_start(startSquare), m_end(endSquare) {}

    virtual ~BoardEntity() = default;

    // ---- Accessors -----------------------------------------

    int start() const { return m_start; }
    int end()   const { return m_end;   }

    /// Returns true for Snake, false for Ladder.
    virtual bool isSnake() const = 0;

    // ---- Rendering -----------------------------------------

    /**
     * @brief Draws this entity at the given world positions.
     *
     * Positions are passed explicitly so the caller can interpolate
     * between old and new positions during a board-reshuffle morph.
     *
     * @param window        Target render window.
     * @param startPos      Pixel position for the entity's start point.
     * @param endPos        Pixel position for the entity's end point.
     * @param theme         Active board theme (controls colour palette).
     * @param index         Entity index within its sibling list (used for
     *                      alternating colour palettes on snakes).
     * @param alpha         Overall opacity ∈ [0, 1].
     * @param slitherPhase  Non-zero during a reshuffle morph to add a
     *                      sinusoidal body-wave to snake curves.
     */
    virtual void draw(sf::RenderWindow& window,
                      sf::Vector2f       startPos,
                      sf::Vector2f       endPos,
                      BoardTheme         theme,
                      int                index,
                      float              alpha        = 1.f,
                      float              slitherPhase = 0.f) const = 0;

protected:
    int m_start;
    int m_end;
};
