#include "Game.hpp"
#include "Renderer.hpp"
#include "MathUtils.hpp"
#include "Snake.hpp"
#include "Ladder.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>

// ============================================================
//  Game.cpp
//  Top-level application: window, main loop, state machine,
//  event handling, update, and all rendering.
// ============================================================

// ---- Construction ------------------------------------------

Game::Game()
    : m_board(GameMode::CLASSIC)
    , m_rng(static_cast<unsigned>(std::time(nullptr)))
{
    sf::ContextSettings cs;
    cs.antiAliasingLevel = 8;

        m_window.create(
        sf::VideoMode({WIN_W, WIN_H}),
        "Slither and Shift - Premium Edition",
        sf::Style::Titlebar | sf::Style::Close,
        sf::State::Windowed,
        cs
    );

    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    loadFont();
    buildMenuUI();
}

// ---- Font loading (tries several common paths) -------------

bool Game::loadFont()
{
    const char* paths[] = {
        "arial.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf"
    };
    for (const char* p : paths)
        if (m_font.openFromFile(p)) return true;
    return false;
}

// ---- Main loop ---------------------------------------------

void Game::run()
{
    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        dt = std::min(dt, 0.033f);   // cap at ~30 fps equivalent to avoid spiral-of-death

        handleEvents();
        update(dt);
        render();
    }
}

// ============================================================
//  UI builders
// ============================================================

void Game::buildMenuUI()
{
    m_btnPlayers.clear();
    m_btnModes.clear();
    m_btnThemes.clear();

    // Column X positions for the three sections
    constexpr float lX = 100.f, mX = 430.f, rX = 770.f;

    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape b({ 260.f, 52.f });
        b.setPosition({lX, 235.f + i * 68.f});
        b.setFillColor({ 45, 50, 68 });
        b.setOutlineThickness(2.f);
        b.setOutlineColor({ 85, 90, 115 });
        m_btnPlayers.push_back(b);
    }
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape b({ 260.f, 52.f });
        b.setPosition({mX, 235.f + i * 68.f});
        b.setFillColor({ 45, 50, 68 });
        b.setOutlineThickness(2.f);
        b.setOutlineColor({ 85, 90, 115 });
        m_btnModes.push_back(b);
    }
    for (int i = 0; i < 4; ++i) {
        sf::RectangleShape b({ 260.f, 48.f });
        b.setPosition({rX, 235.f + i * 60.f});
        b.setFillColor({ 45, 50, 68 });
        b.setOutlineThickness(2.f);
        b.setOutlineColor({ 85, 90, 115 });
        m_btnThemes.push_back(b);
    }

    m_btnStart.setSize({ 300.f, 66.f });
    m_btnStart.setPosition({WIN_W / 2.f - 150.f, 450.f});
    m_btnStart.setFillColor({ 42, 145, 80 });
    m_btnStart.setOutlineThickness(3.f);
    m_btnStart.setOutlineColor({ 255, 255, 255, 180 });

    m_btnHowTo.setSize({ 260.f, 50.f });
    m_btnHowTo.setPosition({WIN_W / 2.f - 130.f, 532.f});
    m_btnHowTo.setFillColor({ 60, 80, 105 });
    m_btnHowTo.setOutlineThickness(1.f);
    m_btnHowTo.setOutlineColor({ 100, 130, 160 });

    m_btnExit.setSize({ 180.f, 46.f });
    m_btnExit.setPosition({WIN_W / 2.f - 90.f, 596.f});
    m_btnExit.setFillColor({ 130, 45, 45 });
    m_btnExit.setOutlineThickness(1.f);
    m_btnExit.setOutlineColor({ 200, 90, 90 });

    m_btnBack.setSize({ 200.f, 56.f });
    m_btnBack.setPosition({WIN_W / 2.f - 100.f, WIN_H - 110.f});
    m_btnBack.setFillColor({ 70, 80, 100 });
    m_btnBack.setOutlineThickness(1.f);
    m_btnBack.setOutlineColor({ 120, 140, 170 });
}

void Game::buildGameUI()
{
    m_btnExitGame.setSize({ 140.f, 38.f });
    m_btnExitGame.setPosition({static_cast<float>(WIN_W) - 165.f, static_cast<float>(WIN_H) - 60.f});
    m_btnExitGame.setFillColor({ 120, 40, 40 });
    m_btnExitGame.setOutlineThickness(1.f);
    m_btnExitGame.setOutlineColor({ 200, 90, 90 });

    m_btnRestart.setSize({ 140.f, 38.f });
    m_btnRestart.setPosition({static_cast<float>(WIN_W) - 325.f, static_cast<float>(WIN_H) - 60.f});
    m_btnRestart.setFillColor({ 40, 110, 70 });
    m_btnRestart.setOutlineThickness(1.f);
    m_btnRestart.setOutlineColor({ 90, 190, 130 });
}

void Game::rebuildSabotageTargetBtns(float sidebarX, float startY)
{
    m_btnSabotageTargets.clear();

    const int   pc      = static_cast<int>(m_players.size());
    const float btnH    = (pc > 2) ? 30.f : 38.f;
    const float spacing = (pc > 2) ? 36.f : 46.f;
    const float cancelH = (pc > 2) ? 26.f : 34.f;

    float y = startY;
    for (int i = 0; i < pc; ++i) {
        if (i == m_curPlayer)             continue;
        if (m_players[i].isSabotaged)     continue;
        if (m_players[i].currentSquare <= 5) continue;

        sf::RectangleShape b({ static_cast<float>(SIDEBAR_W) - 44.f, btnH });
        b.setPosition({sidebarX + 22.f, y});
        b.setFillColor({ 100, 28, 28 });
        b.setOutlineThickness(2.f);
        b.setOutlineColor(m_players[i].color);
        m_btnSabotageTargets.push_back(b);
        y += spacing;
    }

    m_btnSabotageCancel.setSize({ static_cast<float>(SIDEBAR_W) - 44.f, cancelH });
    m_btnSabotageCancel.setPosition({sidebarX + 22.f, y + 4.f});
    m_btnSabotageCancel.setFillColor({ 50, 55, 75 });
    m_btnSabotageCancel.setOutlineThickness(1.f);
    m_btnSabotageCancel.setOutlineColor({ 110, 120, 150 });
}

// ============================================================
//  Game lifecycle
// ============================================================

void Game::startGame(int playerCount)
{
    playerCount    = std::max(2, std::min(4, playerCount));
    m_selPlayerCnt = playerCount;

    m_players.clear();

    // Shuffle animal assignments so players get random animals each game.
    std::vector<AnimalType> pool = {
        AnimalType::CAT, AnimalType::DOG, AnimalType::RABBIT, AnimalType::FOX
    };
    std::shuffle(pool.begin(), pool.end(), m_rng);

    const sf::Color playerColors[4] = {
        { 235,  75,  75 },  // red
        {  70, 150, 240 },  // blue
        { 235, 210,  55 },  // yellow
        { 175,  75, 235 }   // purple
    };

    for (int i = 0; i < playerCount; ++i) {
        Player p;
        p.id            = i + 1;
        p.currentSquare = 1;
        p.color         = playerColors[i];
        p.animal        = pool[i];
        m_players.push_back(p);
    }

    m_curPlayer    = 0;
    m_lastRoll     = 0;
    m_winnerID     = -1;
    m_statusMsg    = "Player 1's turn  —  press [SPACE] to roll!";
    m_animPhase    = AnimPhase::IDLE;

    m_sabotageTargetMode = false;
    m_slideAndThenRoll   = false;
    m_bounceBackTarget   = -1;

    m_board.setMode(m_mode);
    m_board.setTheme(m_theme);
    m_board.warnSnakes.clear();
    m_board.generateEntities();
    buildGameUI();

    m_state = GameState::PLAYING;
}

// ============================================================
//  Roll and movement logic
// ============================================================

void Game::requestRoll()
{
    // Guard: only accept a roll when idle and not already rolling.
    if (m_animPhase != AnimPhase::IDLE || m_dice.rolling || m_players.empty())
        return;

    m_dice.start(m_players[m_curPlayer].hasDoubleRoll);
    m_statusMsg = "Player " + std::to_string(m_players[m_curPlayer].id)
                  + " is rolling...";
}

void Game::finalizeRoll(int total)
{
    Player& p  = m_players[m_curPlayer];
    m_lastRoll = total;

    // Consume the double-roll power-up after use.
    p.hasDoubleRoll = false;

    m_statusMsg = "Player " + std::to_string(p.id)
                  + " rolled " + std::to_string(total) + "!";

    m_stepFromSq = p.currentSquare;
    m_stepDir    = 1;
    m_stepToSq   = m_stepFromSq;

    const int raw = p.currentSquare + total;

    if (raw > 100) {
        // Player overshot 100: walk forward to 100, then bounce backwards.
        // Snake/ladder checks at the bounce destination happen in onLandedAtTarget().
        m_stepTarget       = 100;
        m_bounceBackTarget = 200 - raw;   // mirror distance past 100

        m_pendHitSnake  = false;
        m_pendHitLadder = false;
        m_pendEntityIdx = -1;
        m_pendFinalSq   = m_bounceBackTarget;
    } else {
        m_stepTarget       = raw;
        m_bounceBackTarget = -1;

        m_pendHitSnake  = false;
        m_pendHitLadder = false;
        m_pendEntityIdx = -1;
        m_pendFinalSq   = m_stepTarget;

        // Pre-compute whether the landing square triggers a snake or ladder.
        // This is determined now so the correct slide can begin immediately
        // after the hop animation finishes.
        const auto& snakes  = m_board.snakes();
        const auto& ladders = m_board.ladders();

        for (int i = 0; i < static_cast<int>(snakes.size()); ++i) {
            if (snakes[i]->start() == m_stepTarget) {
                m_pendHitSnake  = true;
                m_pendEntityIdx = i;
                m_pendFinalSq   = snakes[i]->end();
                break;
            }
        }
        if (!m_pendHitSnake) {
            for (int i = 0; i < static_cast<int>(ladders.size()); ++i) {
                if (ladders[i]->start() == m_stepTarget) {
                    m_pendHitLadder = true;
                    m_pendEntityIdx = i;
                    m_pendFinalSq   = ladders[i]->end();
                    break;
                }
            }
        }
    }

    startNextStep();
}

// ---- Step-hop animation ------------------------------------

void Game::startNextStep()
{
    if (m_stepFromSq == m_stepTarget) {
        // All hops complete — resolve the landing square.
        onLandedAtTarget();
        return;
    }

    m_stepToSq    = m_stepFromSq + m_stepDir;
    m_animPhase   = AnimPhase::STEP_HOP;
    m_animProgress = 0.f;
    m_animDuration = HOP_DURATION;
}

// ---- Landing logic -----------------------------------------

void Game::onLandedAtTarget()
{
    Player& p = m_players[m_curPlayer];
    p.currentSquare = m_stepTarget;

    // ---- Bounce-back: overshot 100, now step backwards ----
    if (m_bounceBackTarget >= 0) {
        const int realTarget    = m_bounceBackTarget;
        m_bounceBackTarget      = -1;
        m_statusMsg += "  Too far! Bouncing back to square "
                       + std::to_string(realTarget) + "!";

        // Re-compute snake/ladder hit at the actual bounce destination.
        m_pendHitSnake  = false;
        m_pendHitLadder = false;
        m_pendEntityIdx = -1;
        m_pendFinalSq   = realTarget;

        const auto& snakes  = m_board.snakes();
        const auto& ladders = m_board.ladders();
        for (int i = 0; i < static_cast<int>(snakes.size()); ++i) {
            if (snakes[i]->start() == realTarget) {
                m_pendHitSnake  = true;
                m_pendEntityIdx = i;
                m_pendFinalSq   = snakes[i]->end();
                break;
            }
        }
        if (!m_pendHitSnake) {
            for (int i = 0; i < static_cast<int>(ladders.size()); ++i) {
                if (ladders[i]->start() == realTarget) {
                    m_pendHitLadder = true;
                    m_pendEntityIdx = i;
                    m_pendFinalSq   = ladders[i]->end();
                    break;
                }
            }
        }

        // Walk backwards from 100 to the bounce destination.
        m_stepFromSq = 100;
        m_stepTarget = realTarget;
        m_stepDir    = -1;
        m_stepToSq   = m_stepFromSq;
        startNextStep();
        return;
    }

    // ---- Power-up collection (Chaos mode) ------------------
    auto& powerUps = m_board.powerUps();
    if (m_mode == GameMode::CHAOS && powerUps.count(p.currentSquare)) {
        const PowerUp picked = powerUps[p.currentSquare];
        powerUps.erase(p.currentSquare);

        if (picked == PowerUp::SHIELD) {
            p.hasShield = true;
            m_statusMsg += "  *** SHIELD picked up! Blocks one snake or sabotage! ***";
        } else {
            p.hasDoubleRoll = true;
            m_statusMsg += "  *** DOUBLE ROLL picked up! Roll two dice next turn! ***";
        }
    }

    // ---- Snake or ladder resolution ------------------------
    if (m_pendHitSnake) {
        if (p.hasShield) {
            // Shield absorbs one snake bite.
            p.hasShield = false;
            m_statusMsg += "  Shield blocked the snake!";
            checkVictory();
            startMorphPhase();
        } else {
            startEntitySlide();
        }
    } else if (m_pendHitLadder) {
        startEntitySlide();
    } else {
        checkVictory();
        startMorphPhase();
    }
}

// ---- Entity-slide animation --------------------------------

void Game::startEntitySlide()
{
    Player& p = m_players[m_curPlayer];

    m_slideFrom    = squareToCentre(p.currentSquare);
    m_slideTo      = squareToCentre(m_pendFinalSq);
    m_animProgress = 0.f;
    m_slideDuration = SLIDE_DURATION;
    m_animPhase    = AnimPhase::ENTITY_SLIDE;

    if (m_pendHitSnake) {
        // Bezier curve simulates riding the snake's body.
        m_slideBezier = true;
        p.snakesHit++;
        m_statusMsg += "  A snake! Back to square " + std::to_string(m_pendFinalSq) + "!";
        snakeBezCP(m_slideFrom, m_slideTo, m_slideCp1, m_slideCp2);
    } else {
        // Ladder: straight linear slide upward.
        m_slideBezier = false;
        p.laddersClimbed++;
        m_statusMsg += "  Up the ladder to square " + std::to_string(m_pendFinalSq) + "!";
    }
}

// ============================================================
//  Sabotage (Chaos mode)
// ============================================================

void Game::applySabotage(int victimIdx)
{
    Player& victim   = m_players[victimIdx];
    Player& attacker = m_players[m_curPlayer];

    m_sabotageTargetMode = false;

    // If victim has a shield, it absorbs the sabotage attempt.
    if (victim.hasShield) {
        victim.hasShield = false;
        m_statusMsg = "Player " + std::to_string(attacker.id)
                    + " tried to sabotage Player " + std::to_string(victim.id)
                    + "!  Their SHIELD blocked it! Turn lost.";
        endSabotagerTurn();
        return;
    }

    // Calculate how far the victim will fall.
    const int dropAmount    = 12 + std::rand() % 17;
    const int landingSquare = std::max(1, victim.currentSquare - dropAmount);

    victim.isSabotaged    = true;
    victim.sabotageFallTo = landingSquare;
    attacker.sabotagesUsed++;

    // Register a warning snake overlay on the board so other players
    // can see the active trap during the attacker's remaining turns.
    m_board.warnSnakes.push_back({ victim.currentSquare, landingSquare });

    m_statusMsg = "Player " + std::to_string(attacker.id)
                + " SABOTAGED Player " + std::to_string(victim.id) + "!"
                  "  A trap awaits them!  Turn passed to next player.";

    endSabotagerTurn();
}

void Game::endSabotagerTurn()
{
    // In non-classic modes, the board reshuffles even after a sabotage turn.
    if (m_mode != GameMode::CLASSIC)
        m_board.performReshuffle();

    m_animPhase    = AnimPhase::SABOTAGE_PAUSE;
    m_animProgress = 0.f;
    m_animDuration = SABOTAGE_PAUSE;
}

// ============================================================
//  Turn management
// ============================================================

void Game::startMorphPhase()
{
    if (m_mode != GameMode::CLASSIC) {
        // SLITHER_SHIFT and CHAOS: reshuffle the board each turn.
        m_board.performReshuffle();
        m_animPhase    = AnimPhase::ENTITY_GROW;
        m_animProgress = 0.f;
        m_animDuration = MORPH_DURATION;
    } else {
        // CLASSIC: skip directly to the next player's turn.
        advanceTurn();
    }
}

void Game::advanceTurn()
{
    m_animPhase = AnimPhase::IDLE;
    m_board.clearMorphSnapshot();   // releases pre-reshuffle position snapshots

    // Check victory before advancing.
    Player& cur = m_players[m_curPlayer];
    if (cur.currentSquare >= 100) {
        cur.currentSquare = 100;
        m_winnerID        = cur.id;
        m_state           = GameState::GAME_OVER;
        return;
    }

    m_curPlayer = (m_curPlayer + 1) % static_cast<int>(m_players.size());
    Player& next = m_players[m_curPlayer];

    // Remove any expired warning-snake traps that targeted this player's square.
    auto& ws = m_board.warnSnakes;
    for (int i = static_cast<int>(ws.size()) - 1; i >= 0; --i)
        if (ws[i].first == next.currentSquare)
            ws.erase(ws.begin() + i);

    // If the next player was sabotaged, immediately begin their penalty slide.
    // They slide down first, then still get to roll this turn.
    if (next.isSabotaged) {
        next.isSabotaged = false;

        m_pendHitSnake  = true;
        m_pendHitLadder = false;
        m_pendFinalSq   = next.sabotageFallTo;
        m_pendEntityIdx = -1;

        m_statusMsg = "Player " + std::to_string(next.id)
                    + " was SABOTAGED!  Sliding to square "
                    + std::to_string(m_pendFinalSq)
                    + "  —  then they still roll!";

        m_slideFrom       = squareToCentre(next.currentSquare);
        m_slideTo         = squareToCentre(m_pendFinalSq);
        snakeBezCP(m_slideFrom, m_slideTo, m_slideCp1, m_slideCp2);
        m_slideBezier     = true;
        m_animProgress    = 0.f;
        m_animDuration    = 0.90f;
        m_slideDuration   = 0.90f;
        m_slideAndThenRoll = true;  // victim keeps their turn after the slide
        m_animPhase       = AnimPhase::ENTITY_SLIDE;
        next.snakesHit++;
        return;
    }

    m_statusMsg = "Player " + std::to_string(next.id)
                + "'s turn  —  press [SPACE] to roll!";
}

void Game::checkVictory()
{
    Player& p = m_players[m_curPlayer];
    if (p.currentSquare >= 100)
        p.currentSquare = 100;
}

// ============================================================
//  Utility
// ============================================================

void Game::drawText(const std::string& text, float x, float y,
                    unsigned charSize, sf::Color color,
                    bool centreX, bool bold)
{
    sf::Text t(m_font, text, charSize);
    t.setFillColor(color);
    
    if (centreX)
        t.setPosition({x - t.getLocalBounds().size.x / 2.f, y});
    else
        t.setPosition({x, y});
        
    if (bold)
        t.setStyle(sf::Text::Style::Bold);
        
    m_window.draw(t);
}


sf::Vector2f Game::tokenPos(int playerIdx, int square) const
{
    return squareToCentre(square) + SLOT_OFFSETS[playerIdx % 4];
}

// ============================================================
//  Event handling
// ============================================================

void Game::handleEvents()
{
    while (const std::optional ev = m_window.pollEvent()) {
        if (ev->is<sf::Event::Closed>())
            m_window.close();

        // ---- Menu ------------------------------------------
        if (m_state == GameState::SELECT_SCREEN) {
            if (ev->is<sf::Event::MouseButtonPressed>()
                && ev->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f mp(
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.x),
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.y));

                for (int i = 0; i < 3; ++i)
                    if (m_btnPlayers[i].getGlobalBounds().contains(mp))
                        m_selPlayerCnt = i + 2;
                for (int i = 0; i < 3; ++i)
                    if (m_btnModes[i].getGlobalBounds().contains(mp))
                        m_mode = static_cast<GameMode>(i);
                for (int i = 0; i < 4; ++i)
                    if (m_btnThemes[i].getGlobalBounds().contains(mp))
                        m_theme = static_cast<BoardTheme>(i);

                if (m_btnStart.getGlobalBounds().contains(mp))
                    startGame(m_selPlayerCnt);
                if (m_btnHowTo.getGlobalBounds().contains(mp))
                    m_state = GameState::HOW_TO_PLAY;
                if (m_btnExit.getGlobalBounds().contains(mp))
                    m_window.close();
            }

        // ---- How-to-play -----------------------------------
        } else if (m_state == GameState::HOW_TO_PLAY) {
            if (ev->is<sf::Event::MouseButtonPressed>()
                && ev->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f mp(
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.x),
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.y));
                if (m_btnBack.getGlobalBounds().contains(mp))
                    m_state = GameState::SELECT_SCREEN;
            }

        // ---- Playing ---------------------------------------
        } else if (m_state == GameState::PLAYING) {
            if (ev->is<sf::Event::MouseButtonPressed>()
                && ev->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f mp(
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.x),
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.y));

                if (m_btnExitGame.getGlobalBounds().contains(mp)) {
                    m_sabotageTargetMode = false;
                    m_state = GameState::SELECT_SCREEN;
                    buildMenuUI();
                }
                if (m_btnRestart.getGlobalBounds().contains(mp))
                    startGame(m_selPlayerCnt);

                // Sabotage button (Chaos mode only, when idle)
                if (m_mode == GameMode::CHAOS
                    && m_animPhase == AnimPhase::IDLE
                    && !m_dice.rolling
                    && !m_sabotageTargetMode)
                {
                    if (m_btnSabotage.getGlobalBounds().contains(mp)) {
                        // Verify at least one valid target exists
                        bool hasTarget = false;
                        for (int i = 0; i < static_cast<int>(m_players.size()); ++i)
                            if (i != m_curPlayer
                                && !m_players[i].isSabotaged
                                && m_players[i].currentSquare > 5)
                            { hasTarget = true; break; }

                        if (hasTarget)
                            m_sabotageTargetMode = true;
                        else
                            m_statusMsg = "No valid targets! All opponents are too low or already sabotaged.";
                    }
                }

                // Sabotage target selection
                if (m_sabotageTargetMode) {
                    if (m_btnSabotageCancel.getGlobalBounds().contains(mp)) {
                        m_sabotageTargetMode = false;
                    } else {
                        int btnIdx = 0;
                        for (int i = 0; i < static_cast<int>(m_players.size()); ++i) {
                            if (i == m_curPlayer)             continue;
                            if (m_players[i].isSabotaged)     continue;
                            if (m_players[i].currentSquare <= 5) continue;
                            if (btnIdx < static_cast<int>(m_btnSabotageTargets.size())
                                && m_btnSabotageTargets[btnIdx].getGlobalBounds().contains(mp))
                            {
                                applySabotage(i);
                                break;
                            }
                            btnIdx++;
                        }
                    }
                }
            }

            // Space = roll dice
            if (ev->is<sf::Event::KeyPressed>() && ev->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space)
            {
                if (!m_sabotageTargetMode)
                    requestRoll();
            }

            // Escape = cancel sabotage target selection
            if (ev->is<sf::Event::KeyPressed>() && ev->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
                m_sabotageTargetMode = false;

        // ---- Game over -------------------------------------
        } else if (m_state == GameState::GAME_OVER) {
            if (ev->is<sf::Event::MouseButtonPressed>()
                && ev->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f mp(
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.x),
                    static_cast<float>(ev->getIf<sf::Event::MouseButtonPressed>()->position.y));
                if (m_btnRestart.getGlobalBounds().contains(mp))
                    startGame(m_selPlayerCnt);
                if (m_btnBack.getGlobalBounds().contains(mp)) {
                    m_state = GameState::SELECT_SCREEN;
                    buildMenuUI();
                }
            }
            if (ev->is<sf::Event::KeyPressed>() && ev->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R)
                startGame(m_selPlayerCnt);
        }
    }
}

// ============================================================
//  Per-frame update
// ============================================================

void Game::update(float dt)
{
    m_menuT += dt;

    // ---- Dice update ---------------------------------------
    if (m_dice.rolling && !m_players.empty()) {
        m_dice.update(dt, m_players[m_curPlayer].hasDoubleRoll);
        if (!m_dice.rolling && m_dice.finalTotal > 0)
            finalizeRoll(m_dice.finalTotal);
    }

    // ---- Board morph (snake slither during reshuffle) ------
    m_board.advanceMorph(dt);

    if (m_animPhase == AnimPhase::IDLE) return;

    m_animProgress += dt / m_animDuration;

    // ---- Step-hop ------------------------------------------
    if (m_animPhase == AnimPhase::STEP_HOP) {
        if (m_animProgress >= 1.f) {
            // Commit the hop and begin the next one.
            m_stepFromSq = m_stepToSq;
            m_players[m_curPlayer].currentSquare = m_stepFromSq;
            m_animProgress = 0.f;
            startNextStep();
        }

    // ---- Entity slide --------------------------------------
    } else if (m_animPhase == AnimPhase::ENTITY_SLIDE) {
        if (m_animProgress * m_slideDuration >= m_slideDuration) {
            m_players[m_curPlayer].currentSquare = m_pendFinalSq;
            checkVictory();

            if (m_slideAndThenRoll) {
                // Sabotage penalty: victim slides, then keeps their turn.
                m_slideAndThenRoll = false;
                m_pendHitSnake     = false;
                m_animPhase        = AnimPhase::IDLE;
                m_statusMsg = "Player " + std::to_string(m_players[m_curPlayer].id)
                            + " slid to square " + std::to_string(m_pendFinalSq)
                            + "  —  press [SPACE] to roll!";
            } else {
                startMorphPhase();
            }
        }

    // ---- Entity grow (reshuffle morph) ---------------------
    } else if (m_animPhase == AnimPhase::ENTITY_GROW) {
        if (m_animProgress >= 1.f)
            advanceTurn();

    // ---- Sabotage pause ------------------------------------
    // Input is fully blocked while this phase is active.
    } else if (m_animPhase == AnimPhase::SABOTAGE_PAUSE) {
        if (m_animProgress >= 1.f)
            advanceTurn();
    }
}

// ============================================================
//  Rendering
// ============================================================

void Game::render()
{
    const ThemeColors& tc = THEMES.at(m_theme);

    // Gradient background (two-vertex quad drawn as two triangles)
    sf::Vertex bg[] = {
        { { 0.f,              0.f              }, tc.bgTop    },
        { { static_cast<float>(WIN_W), 0.f              }, tc.bgTop    },
        { { static_cast<float>(WIN_W), static_cast<float>(WIN_H) }, tc.bgBottom },
        { { 0.f,              static_cast<float>(WIN_H) }, tc.bgBottom }
    };
    m_window.clear(tc.bgBottom);
    m_window.draw(bg, 4, sf::PrimitiveType::TriangleFan);

    switch (m_state) {
    case GameState::SELECT_SCREEN: renderMenu(tc);      break;
    case GameState::HOW_TO_PLAY:   renderHowToPlay(tc); break;
    default:                       renderGameplay(tc);  break;
    }

    m_window.display();
}

// ============================================================
//  State-specific renderers
// ============================================================

void Game::renderMenu(const ThemeColors& tc)
{
    // Title
    drawText("SLITHER & SHIFT", WIN_W / 2.f + 3.f, 48.f, 48, { 0, 0, 0, 100 }, true, true);
    drawText("SLITHER & SHIFT", WIN_W / 2.f,        45.f, 48, tc.accent,        true, true);
    drawText("Premium Snakes & Ladders", WIN_W / 2.f, 105.f, 17, { 180, 192, 208 }, true);

    // Section header lines
    auto drawSectionHeader = [&](const std::string& label, float x) {
        sf::RectangleShape line({ 260.f, 2.f });
        line.setPosition({x, 218.f});
        line.setFillColor(tc.accent);
        m_window.draw(line);
        drawText(label, x + 130.f, 194.f, 14, tc.accent, true, true);
    };
    drawSectionHeader("PLAYERS",   100.f);
    drawSectionHeader("GAME MODE", 430.f);
    drawSectionHeader("THEME",     770.f);

    // Player-count buttons
    const char* playerLabels[] = { "2 Players", "3 Players", "4 Players" };
    for (int i = 0; i < 3; ++i) {
        const bool active = (m_selPlayerCnt == i + 2);
        m_btnPlayers[i].setFillColor(active ? tc.accent : sf::Color{ 45, 50, 68 });
        m_btnPlayers[i].setOutlineColor(active
            ? sf::Color{ 255, 255, 255, 200 }
            : sf::Color{ 85, 90, 115 });
        m_window.draw(m_btnPlayers[i]);
        drawText(playerLabels[i],
                 m_btnPlayers[i].getPosition().x + 130.f,
                 m_btnPlayers[i].getPosition().y + 14.f,
                 15, active ? sf::Color{ 20, 20, 20 } : sf::Color{ 210, 220, 230 }, true);
    }

    // Game-mode buttons
    const char* modeLabels[] = { "Classic", "Slither & Shift", "Chaos Mode" };
    for (int i = 0; i < 3; ++i) {
        const bool active = (m_mode == static_cast<GameMode>(i));
        m_btnModes[i].setFillColor(active ? tc.accent : sf::Color{ 45, 50, 68 });
        m_btnModes[i].setOutlineColor(active
            ? sf::Color{ 255, 255, 255, 200 }
            : sf::Color{ 85, 90, 115 });
        m_window.draw(m_btnModes[i]);
        drawText(modeLabels[i],
                 m_btnModes[i].getPosition().x + 130.f,
                 m_btnModes[i].getPosition().y + 14.f,
                 15, active ? sf::Color{ 20, 20, 20 } : sf::Color{ 210, 220, 230 }, true);
    }

    // Theme buttons
    const char* themeLabels[] = {
        "Enchanted Forest", "Sunbaked Dunes", "Deep Abyss", "Neon Twilight"
    };
    for (int i = 0; i < 4; ++i) {
        const bool active = (m_theme == static_cast<BoardTheme>(i));
        m_btnThemes[i].setFillColor(active ? tc.accent : sf::Color{ 45, 50, 68 });
        m_btnThemes[i].setOutlineColor(active
            ? sf::Color{ 255, 255, 255, 200 }
            : sf::Color{ 85, 90, 115 });
        m_window.draw(m_btnThemes[i]);
        drawText(themeLabels[i],
                 m_btnThemes[i].getPosition().x + 130.f,
                 m_btnThemes[i].getPosition().y + 13.f,
                 13, active ? sf::Color{ 20, 20, 20 } : sf::Color{ 210, 220, 230 }, true);
    }

    // Start button with pulsing glow
    const float pulse = 0.5f + 0.5f * std::sin(m_menuT * 3.f);
    sf::RectangleShape glow({ 312.f, 78.f });
    glow.setPosition({WIN_W / 2.f - 156.f, 447.f});
    glow.setFillColor(withAlpha(tc.accent, pulse * 0.22f));
    m_window.draw(glow);

    m_window.draw(m_btnStart);
    drawText("START MATCH",
             m_btnStart.getPosition().x + 150.f,
             m_btnStart.getPosition().y + 18.f,
             22, { 255, 255, 255 }, true, true);

    m_window.draw(m_btnHowTo);
    drawText("HOW TO PLAY",
             m_btnHowTo.getPosition().x + 130.f,
             m_btnHowTo.getPosition().y + 14.f,
             15, { 210, 225, 240 }, true);

    m_window.draw(m_btnExit);
    drawText("EXIT GAME",
             m_btnExit.getPosition().x + 90.f,
             m_btnExit.getPosition().y + 12.f,
             14, { 220, 180, 180 }, true);
}

void Game::renderHowToPlay(const ThemeColors& tc)
{
    drawText("HOW TO PLAY", WIN_W / 2.f, 60.f, 36, tc.accent, true, true);

    sf::RectangleShape div({ 600.f, 2.f });
    div.setPosition({WIN_W / 2.f - 300.f, 105.f});
    div.setFillColor(tc.accent);
    m_window.draw(div);

    // Rule lines — header lines start at column 0 (no indent)
    const std::vector<std::string> lines = {
        "OBJECTIVE",
        "  Be the first player to reach Square 100.",
        "", "CLASSIC MODE",
        "  Ladders take you up, snakes slide you down.",
        "  Board layout stays fixed all game.",
        "", "SLITHER & SHIFT",
        "  After every turn, snakes and ladders move",
        "  to new positions on the board!",
        "", "CHAOS MODE",
        "  Like Slither & Shift, but with power-ups and sabotage!",
        "", "  SHIELD   -  blocks ONE snake bite OR one Sabotage attempt.",
        "  x2 ROLL  -  roll two dice next turn for double movement.",
        "", "  SABOTAGE -  spend your turn to trap an opponent.",
        "  They slide down a snake on their next turn.",
        "  If they have a SHIELD, it absorbs the sabotage!",
        "", "CONTROLS",
        "  [SPACE]  -  Roll the dice",
        "  [R]      -  Restart at any time",
        "  [ESC]    -  Cancel sabotage target selection",
    };

    float y = 125.f;
    for (const auto& line : lines) {
        const bool isHeader = (!line.empty() && line[0] != ' ');
        drawText(line, 130.f, y,
                 isHeader ? 15u : 13u,
                 isHeader ? tc.accent : sf::Color{ 200, 210, 225 },
                 false, isHeader);
        y += isHeader ? 22.f : 18.f;
    }

    m_window.draw(m_btnBack);
    drawText("BACK TO MENU",
             m_btnBack.getPosition().x + 100.f,
             m_btnBack.getPosition().y + 16.f,
             15, { 210, 225, 240 }, true);
}

void Game::renderGameplay(const ThemeColors& tc)
{
    // ---- Board tiles + entities ----------------------------
    m_board.renderTiles   (m_window, tc, m_mode);
    m_board.renderEntities(m_window, tc, m_menuT);

    // ---- Square number labels (drawn by Game since it owns the font) --
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        const int sq  = i + 1;
        const int row = i / GRID_SIZE;
        int       col = i % GRID_SIZE;
        if (row % 2 != 0) col = (GRID_SIZE - 1) - col;
        const int dr = (GRID_SIZE - 1) - row;
        drawText(std::to_string(sq),
                 static_cast<float>(BOARD_X + col * TILE_SIZE + 4),
                 static_cast<float>(BOARD_Y + dr  * TILE_SIZE + 3),
                 10, withAlpha(tc.boardBorder, 0.48f));
    }

    // ---- Power-up badge text (Chaos mode) ------------------
    if (m_mode == GameMode::CHAOS) {
        for (int i = 0; i < TOTAL_SQUARES; ++i) {
            const int sq  = i + 1;
            const int row = i / GRID_SIZE;
            int       col = i % GRID_SIZE;
            if (row % 2 != 0) col = (GRID_SIZE - 1) - col;
            const int dr = (GRID_SIZE - 1) - row;
            if (m_board.powerUps().count(sq)) {
                const PowerUp pu = m_board.powerUps().at(sq);
                drawText(pu == PowerUp::SHIELD ? "SHIELD" : "x2 ROLL",
                         static_cast<float>(BOARD_X + col * TILE_SIZE + 36),
                         static_cast<float>(BOARD_Y + dr  * TILE_SIZE + 49),
                         8, { 255, 255, 255 }, true);
            }
        }
    }

    // ---- Sabotage trap "TRAP" label above warning snakes ---
    const float warnPulse = 0.7f + 0.3f * std::sin(m_menuT * 6.f);
    for (const auto& ws : m_board.warnSnakes)
        drawText("TRAP", squareToCentre(ws.first).x,
                 squareToCentre(ws.first).y - 26.f,
                 9, withAlpha({ 255, 80, 80 }, warnPulse), true, true);

    // ---- Sabotage indicator rings on sabotaged players -----
    for (int i = 0; i < static_cast<int>(m_players.size()); ++i) {
        if (!m_players[i].isSabotaged) continue;
        const sf::Vector2f pos = tokenPos(i, m_players[i].currentSquare);
        sf::CircleShape ring(TOKEN_SIZE + 4.f);
        ring.setOrigin({TOKEN_SIZE + 4.f, TOKEN_SIZE + 4.f});
        ring.setPosition(pos);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.5f);
        ring.setOutlineColor(withAlpha({ 255, 50, 50 }, warnPulse));
        m_window.draw(ring);
    }

    // ---- Static player tokens (all except the active animated one) ----
    for (int i = 0; i < static_cast<int>(m_players.size()); ++i) {
        // The active player's token is drawn separately during animation phases.
        const bool animatedSeparately = (i == m_curPlayer)
            && (m_animPhase == AnimPhase::STEP_HOP
                || m_animPhase == AnimPhase::ENTITY_SLIDE);
        if (animatedSeparately) continue;

        const sf::Vector2f pos = tokenPos(i, m_players[i].currentSquare);
        drawAnimal(m_window, pos, m_players[i].color, m_players[i].animal, TOKEN_SIZE);

        sf::Text lbl(m_font, "P" + std::to_string(m_players[i].id), 8);
        lbl.setFillColor({ 255, 255, 255, 200 });
        lbl.setPosition({pos.x - lbl.getLocalBounds().size.x / 2.f,
                        pos.y + TOKEN_SIZE + 2.f});
        m_window.draw(lbl);
    }

    // ---- Animated current-player token ---------------------
    renderAnimatedToken();

    // ---- Sidebar -------------------------------------------
    const float sidebarX = static_cast<float>(BOARD_X + GRID_SIZE * TILE_SIZE + 20);
    renderSidebar(tc, sidebarX);

    // ---- Bottom bar (status message, restart, exit) --------
    renderStatusBar(tc);
}

void Game::renderAnimatedToken()
{
    if (m_players.empty()) return;

    sf::Vector2f ap;

    if (m_animPhase == AnimPhase::STEP_HOP) {
        // Hop: lerp from step-from to step-to with a small parabolic arc.
        const float        t    = easeInOut(std::min(1.f, m_animProgress));
        const sf::Vector2f from = tokenPos(m_curPlayer, m_stepFromSq);
        const sf::Vector2f to   = tokenPos(m_curPlayer, m_stepToSq);
        ap = lerpV(from, to, t);
        ap.y -= std::sin(t * 3.14159f) * 9.f;   // parabolic bounce height

    } else if (m_animPhase == AnimPhase::ENTITY_SLIDE) {
        // Slide: Bezier (snake) or linear (ladder)
        const float tSlide = easeInOut(std::min(1.f, m_animProgress));
        ap = m_slideBezier
            ? cubicBez(m_slideFrom, m_slideCp1, m_slideCp2, m_slideTo, tSlide)
            : lerpV(m_slideFrom, m_slideTo, tSlide);

    } else {
        return;  // No animated token in other phases
    }

    drawAnimal(m_window, ap,
               m_players[m_curPlayer].color,
               m_players[m_curPlayer].animal,
               TOKEN_SIZE);

    sf::Text lbl(m_font, "P" + std::to_string(m_players[m_curPlayer].id), 8);
    lbl.setFillColor({ 255, 255, 255, 200 });
    lbl.setPosition({ap.x - lbl.getLocalBounds().size.x / 2.f,
                    ap.y + TOKEN_SIZE + 2.f});
    m_window.draw(lbl);
}

// ---- Sidebar -----------------------------------------------

void Game::renderSidebar(const ThemeColors& tc, float sidebarX)
{
    const float sidebarHeight  = static_cast<float>(GRID_SIZE * TILE_SIZE);
    const bool  chaosMode      = (m_mode == GameMode::CHAOS);
    const int   pCount         = static_cast<int>(m_players.size());
    const float cardHeight     = chaosMode ? (pCount <= 2 ? 94.f : 74.f) : 76.f;
    const float cardSpacing    = chaosMode ? (pCount <= 2 ? 104.f : 82.f) : 88.f;

    // Sidebar background
    sf::RectangleShape sidebarBg({ static_cast<float>(SIDEBAR_W), sidebarHeight });
    sidebarBg.setPosition({sidebarX, static_cast<float>(BOARD_Y)});
    sidebarBg.setFillColor(
        sf::Color(tc.sidebarBg.r, tc.sidebarBg.g, tc.sidebarBg.b, 235));
    sidebarBg.setOutlineThickness(2.f);
    sidebarBg.setOutlineColor(tc.accent);
    m_window.draw(sidebarBg);

    // Top accent bar
    sf::RectangleShape topBar({ static_cast<float>(SIDEBAR_W), 4.f });
    topBar.setPosition({sidebarX, static_cast<float>(BOARD_Y)});
    topBar.setFillColor(tc.accent);
    m_window.draw(topBar);

    drawText("SCOREBOARD",
             sidebarX + SIDEBAR_W / 2.f, static_cast<float>(BOARD_Y) + 14.f,
             18, tc.accent, true, true);
    drawText(tc.name,
             sidebarX + SIDEBAR_W / 2.f, static_cast<float>(BOARD_Y) + 37.f,
             11, { 150, 165, 185 }, true);

    // Player cards
    for (int i = 0; i < pCount; ++i) {
        const float cardY = static_cast<float>(BOARD_Y) + 60.f + i * cardSpacing;
        renderPlayerCard(tc, sidebarX, i, cardY, cardHeight, chaosMode);
    }

    // Dice panel
    const float dicePanelY = static_cast<float>(BOARD_Y) + 60.f + pCount * cardSpacing + 12.f;
    const float dicePanelH = (chaosMode && pCount > 2) ? 108.f : 132.f;
    renderDicePanel(tc, sidebarX, dicePanelY, dicePanelH);

    // Sabotage UI (Chaos mode)
    if (chaosMode) {
        const float sabotageY = dicePanelY + dicePanelH + 10.f;
        renderSabotageUI(sidebarX, sabotageY);
    }
}

void Game::renderPlayerCard(const ThemeColors& tc, float sidebarX,
                             int playerIdx, float cardTopY,
                             float cardHeight, bool chaosMode)
{
    const Player& p = m_players[playerIdx];

    // Active-turn highlight applies even during animations (so players know
    // whose turn it is throughout the board-shift animation)
    const bool isActiveTurn = (playerIdx == m_curPlayer)
                              && (m_animPhase != AnimPhase::SABOTAGE_PAUSE);
    const bool canAct = (m_animPhase == AnimPhase::IDLE && playerIdx == m_curPlayer);

    // Card background
    sf::RectangleShape card(
        { static_cast<float>(SIDEBAR_W) - 24.f, cardHeight });
    card.setPosition({sidebarX + 12.f, cardTopY});
    card.setFillColor(isActiveTurn
        ? sf::Color{ 52, 62, 85 }
        : sf::Color{ 37, 42, 60 });
    card.setOutlineThickness(isActiveTurn ? 2.5f : 1.f);
    card.setOutlineColor(isActiveTurn ? tc.accent : sf::Color{ 65, 73, 100 });
    m_window.draw(card);

    // Active-turn left-edge stripe
    if (isActiveTurn) {
        sf::RectangleShape stripe({ 4.f, cardHeight });
        stripe.setPosition({sidebarX + 12.f, cardTopY});
        stripe.setFillColor(tc.accent);
        m_window.draw(stripe);
    }

    // Animal icon
    drawAnimal(m_window,
               { sidebarX + 42.f, cardTopY + 38.f },
               p.color, p.animal, 15.f);

    // Player name + stats
    const std::string label = "P" + std::to_string(p.id)
                              + "  " + ANIMAL_NAMES.at(p.animal);
    drawText(label, sidebarX + 72.f, cardTopY + 8.f,
             13, { 225, 235, 248 }, false, isActiveTurn);
    drawText("Square: " + std::to_string(p.currentSquare) + "/100",
             sidebarX + 72.f, cardTopY + 28.f, 11, { 165, 180, 200 });
    drawText("Snakes: " + std::to_string(p.snakesHit)
             + "   Ladders: " + std::to_string(p.laddersClimbed),
             sidebarX + 72.f, cardTopY + 44.f, 10, { 128, 145, 170 });

    // Power-up badges (Chaos mode)
    if (chaosMode) {
        float badgeX = sidebarX + 18.f;
        const float badgeY = cardTopY + 61.f;

        auto drawBadge = [&](const std::string& lbl,
                              sf::Color bgColor, sf::Color textColor)
        {
            const float badgeW = lbl.size() * 7.5f + 10.f;
            sf::RectangleShape badge({ badgeW, 18.f });
            badge.setPosition({badgeX, badgeY});
            badge.setFillColor(bgColor);
            badge.setOutlineThickness(1.f);
            badge.setOutlineColor(textColor);
            m_window.draw(badge);
            drawText(lbl, badgeX + badgeW / 2.f, badgeY + 2.f,
                     10, textColor, true, true);
            badgeX += badgeW + 6.f;
        };

        if (p.hasShield)
            drawBadge("SHIELD", { 20, 60, 140 }, { 130, 200, 255 });
        if (p.hasDoubleRoll)
            drawBadge("x2 ROLL", { 140, 90, 10 }, { 255, 220, 80 });
        if (p.isSabotaged) {
            const float tp = 0.55f + 0.45f * std::sin(m_menuT * 6.f);
            drawBadge("TRAPPED!", withAlpha({ 100, 10, 10 }, tp), { 255, 90, 90 });
        }
    }

    // Turn-status badge (top-right corner of card)
    if (isActiveTurn) {
        std::string statusText;
        sf::Color   statusColor;

        if (m_animPhase == AnimPhase::ENTITY_SLIDE && m_pendHitSnake) {
            statusText  = "SLIDING!";
            statusColor = { 255, 100, 100 };
        } else if (m_animPhase == AnimPhase::STEP_HOP) {
            statusText  = "MOVING...";
            statusColor = { 180, 220, 255 };
        } else if (m_animPhase == AnimPhase::ENTITY_GROW) {
            statusText  = "BOARD SHIFT";
            statusColor = { 200, 180, 255 };
        } else if (canAct && m_dice.rolling) {
            statusText  = "ROLLING...";
            statusColor = { 255, 215, 70 };
        } else if (canAct && m_sabotageTargetMode) {
            statusText  = "PICK TARGET";
            statusColor = { 255, 80, 80 };
        } else if (canAct) {
            statusText  = "YOUR TURN";
            statusColor = { 130, 255, 160 };
        }

        if (!statusText.empty())
            drawText(statusText,
                     sidebarX + SIDEBAR_W - 82.f, cardTopY + 8.f,
                     8, statusColor, true, true);
    }
}

void Game::renderDicePanel(const ThemeColors& tc, float sidebarX,
                            float dicePanelY, float dicePanelH)
{
    const float diceCenterX = sidebarX + SIDEBAR_W / 2.f;

    sf::RectangleShape panel(
        { static_cast<float>(SIDEBAR_W) - 24.f, dicePanelH });
    panel.setPosition({sidebarX + 12.f, dicePanelY});
    panel.setFillColor({ 32, 38, 56, 242 });
    panel.setOutlineThickness(1.5f);
    panel.setOutlineColor(tc.accent);
    m_window.draw(panel);

    drawText("DICE", diceCenterX, dicePanelY + 8.f, 12, { 148, 162, 182 }, true, true);

    const bool  isDoubleRoll = (!m_players.empty()
                                && m_players[m_curPlayer].hasDoubleRoll);
    const float diceWobble   = m_dice.rolling ? m_dice.wobble : 0.f;
    const float diceY        = dicePanelY + (dicePanelH > 115.f ? 68.f : 56.f);
    const float resultY      = dicePanelY + dicePanelH - 18.f;

    if (isDoubleRoll) {
        drawDiceFace(m_window, { diceCenterX - 36.f, diceY }, 50.f,
                     m_dice.faceA,  diceWobble);
        drawDiceFace(m_window, { diceCenterX + 36.f, diceY }, 50.f,
                     (m_dice.faceB > 0 ? m_dice.faceB : 1), -diceWobble);
        if (!m_dice.rolling && m_lastRoll > 0)
            drawText("Total: " + std::to_string(m_lastRoll),
                     diceCenterX, resultY, 11, tc.accent, true, true);
    } else {
        drawDiceFace(m_window, { diceCenterX, diceY }, 56.f,
                     m_dice.faceA, diceWobble);
        if (!m_dice.rolling && m_lastRoll > 0)
            drawText("Rolled: " + std::to_string(m_lastRoll),
                     diceCenterX, resultY, 11, tc.accent, true, true);
    }
}

void Game::renderSabotageUI(float sidebarX, float startY)
{
    if (m_players.empty()) return;

    const bool idle     = (m_animPhase == AnimPhase::IDLE && !m_dice.rolling);
    const bool canSab   = idle && !m_sabotageTargetMode;
    const bool anyAlive = [&] {
        for (int i = 0; i < static_cast<int>(m_players.size()); ++i)
            if (i != m_curPlayer
                && !m_players[i].isSabotaged
                && m_players[i].currentSquare > 5)
                return true;
        return false;
    }();

    if (!m_sabotageTargetMode) {
        // Show the sabotage button when idle
        m_btnSabotage.setSize({ static_cast<float>(SIDEBAR_W) - 24.f, 36.f });
        m_btnSabotage.setPosition({sidebarX + 12.f, startY});
        m_btnSabotage.setFillColor(
            (canSab && anyAlive) ? sf::Color{ 130, 28, 28 }
                                 : sf::Color{ 60, 40, 40 });
        m_btnSabotage.setOutlineThickness(1.5f);
        m_btnSabotage.setOutlineColor({ 200, 60, 60 });
        m_window.draw(m_btnSabotage);
        drawText("⚡ SABOTAGE",
                 sidebarX + SIDEBAR_W / 2.f, startY + 9.f,
                 13, (canSab && anyAlive)
                     ? sf::Color{ 255, 120, 120 }
                     : sf::Color{ 120, 80, 80 },
                 true, true);
    } else {
        // Show target buttons and cancel
        rebuildSabotageTargetBtns(sidebarX, startY);

        drawText("Pick a target:", sidebarX + SIDEBAR_W / 2.f, startY - 16.f,
                 11, { 255, 80, 80 }, true, true);

        int btnIdx = 0;
        for (int i = 0; i < static_cast<int>(m_players.size()); ++i) {
            if (i == m_curPlayer)             continue;
            if (m_players[i].isSabotaged)     continue;
            if (m_players[i].currentSquare <= 5) continue;

            if (btnIdx < static_cast<int>(m_btnSabotageTargets.size())) {
                m_window.draw(m_btnSabotageTargets[btnIdx]);
                const std::string tl = "P" + std::to_string(m_players[i].id)
                                     + " — sq " + std::to_string(m_players[i].currentSquare);
                drawText(tl,
                         m_btnSabotageTargets[btnIdx].getPosition().x
                             + (SIDEBAR_W - 44.f) / 2.f,
                         m_btnSabotageTargets[btnIdx].getPosition().y + 6.f,
                         11, m_players[i].color, true, true);
            }
            btnIdx++;
        }
        m_window.draw(m_btnSabotageCancel);
        drawText("Cancel",
                 m_btnSabotageCancel.getPosition().x + (SIDEBAR_W - 44.f) / 2.f,
                 m_btnSabotageCancel.getPosition().y + 6.f,
                 10, { 180, 190, 210 }, true);
    }
}

void Game::renderStatusBar(const ThemeColors& tc)
{
    // Status message strip
    sf::RectangleShape statusBar(
        { static_cast<float>(WIN_W), 40.f });
    statusBar.setPosition({0.f, static_cast<float>(WIN_H) - 68.f});
    statusBar.setFillColor({ 12, 15, 24, 200 });
    m_window.draw(statusBar);

    drawText(m_statusMsg, WIN_W / 2.f,
             static_cast<float>(WIN_H) - 61.f,
             12, { 215, 225, 242 }, true);

    // Restart + exit buttons
    m_window.draw(m_btnRestart);
    drawText("RESTART",
             m_btnRestart.getPosition().x + 70.f,
             m_btnRestart.getPosition().y + 10.f,
             13, { 200, 240, 210 }, true);

    m_window.draw(m_btnExitGame);
    drawText("MENU",
             m_btnExitGame.getPosition().x + 70.f,
             m_btnExitGame.getPosition().y + 10.f,
             13, { 240, 200, 200 }, true);

    // Game-over overlay
    if (m_state == GameState::GAME_OVER) {
        sf::RectangleShape overlay(
            { static_cast<float>(WIN_W), static_cast<float>(WIN_H) });
        overlay.setFillColor({ 0, 0, 0, 155 });
        m_window.draw(overlay);

        drawText("WINNER!", WIN_W / 2.f, WIN_H / 2.f - 80.f,
                 56, tc.accent, true, true);
        drawText("Player " + std::to_string(m_winnerID) + " wins the game!",
                 WIN_W / 2.f, WIN_H / 2.f - 15.f,
                 24, { 230, 240, 255 }, true, true);

        // Final stats
        float statY = WIN_H / 2.f + 35.f;
        for (const auto& p : m_players) {
            drawText("P" + std::to_string(p.id)
                     + " — Snakes: "  + std::to_string(p.snakesHit)
                     + "  Ladders: " + std::to_string(p.laddersClimbed),
                     WIN_W / 2.f, statY, 14, { 180, 200, 225 }, true);
            statY += 24.f;
        }

        m_window.draw(m_btnRestart);
        drawText("PLAY AGAIN",
                 m_btnRestart.getPosition().x + 70.f,
                 m_btnRestart.getPosition().y + 10.f,
                 13, { 200, 240, 210 }, true);

        m_window.draw(m_btnBack);
        drawText("BACK TO MENU",
                 m_btnBack.getPosition().x + 100.f,
                 m_btnBack.getPosition().y + 16.f,
                 15, { 210, 225, 240 }, true);
    }
}
