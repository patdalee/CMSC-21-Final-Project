#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>

#include "Constants.hpp"
#include "Types.hpp"
#include "MathUtils.hpp"
#include "Board.hpp"
#include "Dice.hpp"

// ============================================================
//  Game.hpp
//  Top-level application class — owns the window, all game state,
//  and drives the main loop.
//
//  Responsibilities:
//    • Window creation and frame-rate control
//    • State machine (SELECT_SCREEN → PLAYING → GAME_OVER)
//    • Event handling and input routing per state
//    • Per-frame update (dice, animation phases, morph)
//    • Delegated rendering (board, entities, sidebar, menus)
//    • Turn logic (roll, step-hop, entity-slide, sabotage)
// ============================================================

class Game {
public:
    Game();

    /// Runs the main loop until the window is closed.
    void run();

private:
    // ---- Window & resources --------------------------------
    sf::RenderWindow m_window;
    sf::Font         m_font;
    sf::Clock        m_clock;

    // ---- Game state ----------------------------------------
    GameState  m_state        = GameState::SELECT_SCREEN;
    GameMode   m_mode         = GameMode::CLASSIC;
    BoardTheme m_theme        = BoardTheme::FOREST;
    int        m_selPlayerCnt = 2;

    std::vector<Player> m_players;
    int                 m_curPlayer = 0;
    int                 m_lastRoll  = 0;
    int                 m_winnerID  = -1;
    std::string         m_statusMsg;

    // ---- Board & dice --------------------------------------
    Board m_board;
    Dice  m_dice;

    // ---- Animation state -----------------------------------
    AnimPhase m_animPhase    = AnimPhase::IDLE;
    float     m_animProgress = 0.f;
    float     m_animDuration = HOP_DURATION;

    // Step-hop sub-animation (token hops one square at a time)
    int m_stepFromSq  = 1;   ///< Square the token is leaving this hop
    int m_stepToSq    = 1;   ///< Square the token is arriving at this hop
    int m_stepTarget  = 1;   ///< Final square after all hops complete
    int m_stepDir     = 1;   ///< +1 forward, -1 backward (bounce-back)

    // Entity-slide sub-animation (snake/ladder path)
    sf::Vector2f m_slideFrom, m_slideTo;
    sf::Vector2f m_slideCp1,  m_slideCp2;   ///< Bezier control points (snakes)
    bool         m_slideBezier  = false;
    float        m_slideDuration = SLIDE_DURATION;

    // Pending entity result from finalizeRoll
    bool m_pendHitSnake  = false;
    bool m_pendHitLadder = false;
    int  m_pendFinalSq   = 0;
    int  m_pendEntityIdx = -1;

    // Bounce-back target (> 0 when roll overshoots square 100)
    int  m_bounceBackTarget = -1;

    // Sabotage state (Chaos mode)
    bool m_sabotageTargetMode = false;
    bool m_slideAndThenRoll   = false;  ///< Victim slides, but keeps their turn

    // ---- UI button shapes ----------------------------------
    std::vector<sf::RectangleShape> m_btnPlayers, m_btnModes, m_btnThemes;
    sf::RectangleShape m_btnStart, m_btnHowTo, m_btnExit;
    sf::RectangleShape m_btnBack, m_btnRestart, m_btnExitGame;
    sf::RectangleShape m_btnSabotage, m_btnSabotageCancel;
    std::vector<sf::RectangleShape> m_btnSabotageTargets;

    float        m_menuT = 0.f;          ///< Accumulated time for pulse animations
    std::mt19937 m_rng;

    // ========================================================
    //  Private helpers
    // ========================================================

    // ---- Font loading --------------------------------------
    bool loadFont();

    // ---- Menu UI builders ----------------------------------
    void buildMenuUI();
    void buildGameUI();
    void rebuildSabotageTargetBtns(float sidebarX, float startY);

    // ---- Game lifecycle ------------------------------------
    void startGame(int playerCount);
    void requestRoll();
    void finalizeRoll(int total);

    // ---- Step-hop animation --------------------------------
    void startNextStep();
    void onLandedAtTarget();

    // ---- Entity-slide animation ----------------------------
    void startEntitySlide();

    // ---- Turn management -----------------------------------
    void startMorphPhase();
    void advanceTurn();
    void checkVictory();

    // ---- Sabotage (Chaos mode) -----------------------------
    void applySabotage(int victimIdx);
    void endSabotagerTurn();

    // ---- Main loop methods ---------------------------------
    void handleEvents();
    void update(float dt);
    void render();

    // ---- Per-state renderers -------------------------------
    void renderMenu       (const ThemeColors& tc);
    void renderHowToPlay  (const ThemeColors& tc);
    void renderGameplay   (const ThemeColors& tc);

    // ---- Render sub-helpers --------------------------------
    void renderSidebar    (const ThemeColors& tc, float sidebarX);
    void renderPlayerCard (const ThemeColors& tc, float sidebarX,
                           int playerIdx, float cardTopY,
                           float cardHeight, bool chaosModeActive);
    void renderDicePanel  (const ThemeColors& tc, float sidebarX,
                           float dicePanelY, float dicePanelH);
    void renderSabotageUI (float sidebarX, float startY);
    void renderStatusBar  (const ThemeColors& tc);
    void renderAnimatedToken();

    // ---- Drawing utility -----------------------------------
    void drawText(const std::string& text, float x, float y,
                  unsigned charSize, sf::Color color,
                  bool centreX = false, bool bold = false);

    sf::Vector2f tokenPos(int playerIdx, int square) const;
};
