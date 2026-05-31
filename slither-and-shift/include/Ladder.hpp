#pragma once

#include "BoardEntity.hpp"

// ============================================================
//  Ladder.hpp
//  Concrete BoardEntity representing a ladder on the board.
//
//  A ladder has its base at `start()` (lower square) and its
//  top at `end()` (higher square).  The draw() implementation
//  renders two parallel rails with evenly-spaced rungs.
// ============================================================

class Ladder final : public BoardEntity {
public:
    Ladder(int baseSquare, int topSquare)
        : BoardEntity(baseSquare, topSquare) {}

    bool isSnake() const override { return false; }

    /// Returns the rail/rung colour palette for the given theme.
    static LadderPal palette(BoardTheme theme);

    /**
     * @brief Draws the ladder as two parallel rails with rungs.
     *
     * @param window   Render target.
     * @param startPos Base pixel position (interpolated during morph).
     * @param endPos   Top pixel position.
     * @param theme    Active theme for palette lookup.
     * @param index    Unused for ladders (all ladders share one palette).
     * @param alpha    Overall opacity.
     * @param slitherPhase Unused for ladders.
     */
    void draw(sf::RenderWindow& window,
              sf::Vector2f       startPos,
              sf::Vector2f       endPos,
              BoardTheme         theme,
              int                index,
              float              alpha        = 1.f,
              float              slitherPhase = 0.f) const override;
};
