#include "Game.hpp"

// ============================================================
//  main.cpp
//  Entry point — constructs the Game and hands control to its
//  main loop.  All window creation, resource loading, and state
//  initialisation happen inside the Game constructor.
// ============================================================

int main()
{
    Game game;
    game.run();
    return 0;
}
