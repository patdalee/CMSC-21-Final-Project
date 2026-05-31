#include "Ladder.hpp"
#include "Renderer.hpp"

// ============================================================
//  Ladder.cpp
//  Implements Ladder::palette() and Ladder::draw().
// ============================================================

LadderPal Ladder::palette(BoardTheme theme)
{
    switch (theme) {
    case BoardTheme::FOREST: return { {140,72,20},  {210,140,65}  };
    case BoardTheme::DESERT: return { {185,155,120},{235,215,195} };
    case BoardTheme::OCEAN:  return { {65,130,185}, {220,168,32}  };
    case BoardTheme::NIGHT:  return { {48,78,78},   {0,255,255}   };
    }
    return { {120,120,120}, {200,200,200} };
}

void Ladder::draw(sf::RenderWindow& window,
                  sf::Vector2f       startPos,
                  sf::Vector2f       endPos,
                  BoardTheme         theme,
                  int                /*index*/,
                  float              alpha,
                  float              /*slitherPhase*/) const
{
    // startPos = base, endPos = top for ladders
    drawLadderFull(window, startPos, endPos, palette(theme), alpha);
}
