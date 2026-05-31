#include "Snake.hpp"
#include "Renderer.hpp"

// ============================================================
//  Snake.cpp
//  Implements Snake::palette() and Snake::draw().
// ============================================================

SnakePal Snake::palette(BoardTheme theme, int index)
{
    // Each theme provides two alternating palettes (even/odd index)
    // to visually distinguish snakes sharing the same board.
    switch (theme) {
    case BoardTheme::FOREST:
        return (index % 2 == 0)
            ? SnakePal{ {30,145,30},  {140,240,140}, {20,100,20}  }
            : SnakePal{ {0,110,85},   {90,210,165},  {0,80,60}    };

    case BoardTheme::DESERT:
        return (index % 2 == 0)
            ? SnakePal{ {215,100,28}, {248,165,90},  {165,75,20}  }
            : SnakePal{ {170,85,42},  {225,185,135}, {130,60,30}  };

    case BoardTheme::OCEAN:
        return (index % 2 == 0)
            ? SnakePal{ {0,110,155},  {120,235,240}, {0,75,120}   }
            : SnakePal{ {30,30,120},  {0,195,255},   {20,20,90}   };

    case BoardTheme::NIGHT:
        return (index % 2 == 0)
            ? SnakePal{ {145,40,230}, {242,128,240}, {100,25,180} }
            : SnakePal{ {80,0,145},   {255,0,128},   {60,0,110}   };
    }

    // Fallback (should never be reached)
    return { {0,200,0}, {0,255,0}, {0,150,0} };
}

void Snake::draw(sf::RenderWindow& window,
                 sf::Vector2f       startPos,
                 sf::Vector2f       endPos,
                 BoardTheme         theme,
                 int                index,
                 float              alpha,
                 float              slitherPhase) const
{
    // startPos = head, endPos = tail for snakes
    drawSnakeFull(window, startPos, endPos, palette(theme, index), alpha, slitherPhase);
}
