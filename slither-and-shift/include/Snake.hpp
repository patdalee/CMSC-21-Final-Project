#pragma once

#include "BoardEntity.hpp"

// ============================================================
//  Snake.hpp
//  Concrete BoardEntity representing a snake on the board.
//
//  A snake has its head at `start()` (higher square) and its
//  tail at `end()` (lower square).  The draw() implementation
//  renders an animated, Bezier-curved snake body using the
//  theme-appropriate colour palette.
// ============================================================

class Snake final : public BoardEntity {
public:
    Snake(int headSquare, int tailSquare)
        : BoardEntity(headSquare, tailSquare) {}

    bool isSnake() const override { return true; }

    /**
     * @brief Returns the colour palette for snake at index within theme.
     *
     * Alternates between two palettes (even/odd index) to visually
     * distinguish different snakes on the same board.
     */
    static SnakePal palette(BoardTheme theme, int index);

    /**
     * @brief Draws the snake as a thick Bezier curve with head details.
     *
     * @param window        Render target.
     * @param startPos      Head pixel position (interpolated during morph).
     * @param endPos        Tail pixel position.
     * @param theme         Active theme for palette lookup.
     * @param index         Snake index (selects even/odd palette).
     * @param alpha         Overall opacity for fade-in/out effects.
     * @param slitherPhase  Wave amplitude/phase for slither animation.
     */
    void draw(sf::RenderWindow& window,
              sf::Vector2f       startPos,
              sf::Vector2f       endPos,
              BoardTheme         theme,
              int                index,
              float              alpha        = 1.f,
              float              slitherPhase = 0.f) const override;
};
