#include "Board.hpp"
#include "Renderer.hpp"
#include "MathUtils.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// ============================================================
//  Board.cpp
//  Implements entity generation, reshuffle animation, and all
//  board rendering (tiles, snakes, ladders, power-up badges).
// ============================================================

Board::Board(GameMode mode)
    : m_mode(mode)
    , m_rng(std::random_device{}())
{}

// ---- Collision guard ---------------------------------------

bool Board::squareAlreadyTaken(int start, int end,
                                const std::vector<int>& usedStarts,
                                const std::vector<int>& usedEnds) const
{
    // A square cannot serve as both the start and end of the same entity.
    if (start == end) return true;

    // Avoid nearly-vertical runs on the same board column, which look bad
    // and can confuse players about direction of travel.
    if ((start % 10 == end % 10) && (std::abs(start - end) < 11)) return true;

    for (int sq : usedStarts)
        if (sq == start || sq == end) return true;
    for (int sq : usedEnds)
        if (sq == start || sq == end) return true;

    return false;
}

// ---- Entity generation -------------------------------------

void Board::generateEntities()
{
    m_snakes.clear();
    m_ladders.clear();
    m_powerUps.clear();

    std::vector<int> usedStarts, usedEnds;

    // ---- Ladders -------------------------------------------
    // A ladder's base must be at least one row below its top.
    for (int idx = 0; idx < NUM_LADDERS; ++idx) {
        for (int attempt = 0; attempt < 400; ++attempt) {
            const int base = 2 + std::rand() % 84;
            const int row  = (base - 1) / 10;
            if (row >= 9) continue;  // no room above

            const int minTop = (row + 1) * 10 + 1;
            const int maxTop = std::min(100, (row + 4) * 10);
            if (maxTop <= minTop) continue;

            const int top = minTop + std::rand() % (maxTop - minTop);
            if (!squareAlreadyTaken(base, top, usedStarts, usedEnds)) {
                m_ladders.push_back(std::make_unique<Ladder>(base, top));
                usedStarts.push_back(base);
                usedEnds.push_back(top);
                break;
            }
        }
    }

    // ---- Snakes --------------------------------------------
    // A snake's head must be at least one row above its tail.
    for (int idx = 0; idx < NUM_SNAKES; ++idx) {
        for (int attempt = 0; attempt < 400; ++attempt) {
            const int head = 15 + std::rand() % 84;
            const int row  = (head - 1) / 10;
            if (row <= 0) continue;  // no room below

            const int minTail = std::max(1, (row - 3) * 10);
            const int maxTail = row * 10;
            if (maxTail <= minTail) continue;

            const int tail = minTail + std::rand() % (maxTail - minTail);
            if (!squareAlreadyTaken(head, tail, usedStarts, usedEnds)) {
                m_snakes.push_back(std::make_unique<Snake>(head, tail));
                usedStarts.push_back(head);
                usedEnds.push_back(tail);
                break;
            }
        }
    }

    // ---- Power-ups (Chaos mode only) -----------------------
    if (m_mode == GameMode::CHAOS) {
        for (int i = 0; i < 8; ++i) {
            const int sq = 5 + std::rand() % 90;
            const bool taken = std::find(usedStarts.begin(), usedStarts.end(), sq)
                               != usedStarts.end();
            if (!taken)
                m_powerUps[sq] = (std::rand() % 2 == 0)
                                 ? PowerUp::SHIELD
                                 : PowerUp::DOUBLE_ROLL;
        }
    }
}

// ---- Reshuffle (snapshot + regenerate) ---------------------

void Board::performReshuffle()
{
    // Capture current world positions so the morph lerp can start from them.
    m_oldSnakePos.clear();
    m_oldLadderPos.clear();

    for (const auto& s : m_snakes)
        m_oldSnakePos.push_back({ squareToCentre(s->start()), squareToCentre(s->end()) });
    for (const auto& l : m_ladders)
        m_oldLadderPos.push_back({ squareToCentre(l->start()), squareToCentre(l->end()) });

    generateEntities();

    // Reset morph progress: update() will tick morphT from 0 → 1.
    m_morphT      = 0.f;
    m_morphPhaseT = 0.f;
}

// ---- Morph snapshot clear ----------------------------------

void Board::clearMorphSnapshot()
{
    m_oldSnakePos.clear();
    m_oldLadderPos.clear();
}

// ---- Morph animation tick ----------------------------------

void Board::advanceMorph(float dt)
{
    if (m_morphT >= 1.f) return;

    m_morphT      = std::min(1.f, m_morphT + dt / MORPH_DURATION);
    m_morphPhaseT += dt * MORPH_WAVE_SPEED;
}

// ---- Entity lookup -----------------------------------------

BoardEntity* Board::entityAtStart(int sq) const
{
    for (const auto& s : m_snakes)
        if (s->start() == sq) return s.get();
    for (const auto& l : m_ladders)
        if (l->start() == sq) return l.get();
    return nullptr;
}

// ---- Tile rendering ----------------------------------------

void Board::renderTiles(sf::RenderWindow& window,
                        const ThemeColors& tc,
                        GameMode           mode) const
{
    // Board background shadow
    sf::RectangleShape shadow(
        { static_cast<float>(GRID_SIZE * TILE_SIZE + 10),
          static_cast<float>(GRID_SIZE * TILE_SIZE + 10) });
    shadow.setPosition({BOARD_X - 2.f, BOARD_Y + 5.f});
    shadow.setFillColor({ 0, 0, 0, 70 });
    window.draw(shadow);

    // Board background
    sf::RectangleShape bg(
        { static_cast<float>(GRID_SIZE * TILE_SIZE),
          static_cast<float>(GRID_SIZE * TILE_SIZE) });
    bg.setPosition({BOARD_X, BOARD_Y});
    bg.setFillColor(tc.darkTile);
    bg.setOutlineThickness(5.f);
    bg.setOutlineColor(tc.boardBorder);
    window.draw(bg);

    // Individual tiles
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        const int sq  = i + 1;
        const int row = i / GRID_SIZE;
        int       col = i % GRID_SIZE;

        // Boustrophedon: odd rows run right-to-left
        if (row % 2 != 0) col = (GRID_SIZE - 1) - col;
        const int dr = (GRID_SIZE - 1) - row;

        sf::RectangleShape tile(
            { static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) });
        
tile.setPosition({
            static_cast<float>(BOARD_X + col * TILE_SIZE),
            static_cast<float>(BOARD_Y + dr  * TILE_SIZE)});

        // Alternate light/dark; tint square 100 with accent, square 1 with green.
        sf::Color tileCol = ((row + col) % 2 == 0) ? tc.lightTile : tc.darkTile;
        if (sq == 100) tileCol = lerpColor(tileCol, tc.accent,    0.38f);
        if (sq == 1)   tileCol = lerpColor(tileCol, { 195,255,195 }, 0.22f);
        tile.setFillColor(tileCol);
        window.draw(tile);

        // Square number label
       static sf::Font defaultFont;
sf::Text numLabel(defaultFont, std::to_string(sq), 10);
        numLabel.setFont(defaultFont);
        numLabel.setString(std::to_string(sq));
        numLabel.setCharacterSize(10);
        // Note: font is owned by Game; square labels are small so we skip them
        // if no font is available (Board does not hold a font reference by design).

        // Power-up badge (Chaos mode)
        if (mode == GameMode::CHAOS && m_powerUps.count(sq)) {
            const PowerUp pu = m_powerUps.at(sq);
            sf::RectangleShape badge({ 56.f, 18.f });
            badge.setPosition({
                static_cast<float>(BOARD_X + col * TILE_SIZE + 8),
                static_cast<float>(BOARD_Y + dr  * TILE_SIZE + 48)});
            badge.setFillColor(pu == PowerUp::SHIELD
                ? sf::Color{ 22,  90, 172, 210 }
                : sf::Color{ 172, 100,  12, 210 });
            badge.setOutlineThickness(1.f);
            badge.setOutlineColor({ 0, 0, 0, 75 });
            window.draw(badge);
        }
    }

    // Grid-line overlay (subtle)
    for (int r = 0; r <= GRID_SIZE; ++r) {
        sf::RectangleShape line(
            { static_cast<float>(GRID_SIZE * TILE_SIZE), 1.f });
        line.setPosition({
            static_cast<float>(BOARD_X),
            static_cast<float>(BOARD_Y + r * TILE_SIZE)});
        line.setFillColor({ 0, 0, 0, 20 });
        window.draw(line);
    }
    for (int c = 0; c <= GRID_SIZE; ++c) {
        sf::RectangleShape line(
            { 1.f, static_cast<float>(GRID_SIZE * TILE_SIZE) });
        line.setPosition({
            static_cast<float>(BOARD_X + c * TILE_SIZE),
            static_cast<float>(BOARD_Y)});
        line.setFillColor({ 0, 0, 0, 20 });
        window.draw(line);
    }
}

// ---- Entity rendering (with morph lerp) --------------------

void Board::renderEntities(sf::RenderWindow& window,
                           const ThemeColors& /*tc*/,
                           float menuT) const
{
    const float morphEased = easeInOut(std::min(1.f, m_morphT));

    // ---- Ladders -------------------------------------------
    for (int i = 0; i < static_cast<int>(m_ladders.size()); ++i) {
        sf::Vector2f s = squareToCentre(m_ladders[i]->start());
        sf::Vector2f e = squareToCentre(m_ladders[i]->end());

        // Interpolate from old to new position during a reshuffle morph
        if (!m_oldLadderPos.empty() && i < static_cast<int>(m_oldLadderPos.size())) {
            s = lerpV(m_oldLadderPos[i].startPos, s, morphEased);
            e = lerpV(m_oldLadderPos[i].endPos,   e, morphEased);
        }

        m_ladders[i]->draw(window, s, e, m_theme, i, 1.f, 0.f);
    }

    // ---- Snakes (with slither wave during reshuffle) -------
    // The slither amplitude follows a bell-curve: 0 at start and end of morph,
    // peaked in the middle — simulating a snake wriggling into place.
    const float slitherAmt = (m_morphT < 1.f)
        ? std::sin(m_morphT * 3.14159f) * 1.f
        : 0.f;

    for (int i = 0; i < static_cast<int>(m_snakes.size()); ++i) {
        sf::Vector2f s = squareToCentre(m_snakes[i]->start());
        sf::Vector2f e = squareToCentre(m_snakes[i]->end());

        if (!m_oldSnakePos.empty() && i < static_cast<int>(m_oldSnakePos.size())) {
            s = lerpV(m_oldSnakePos[i].startPos, s, morphEased);
            e = lerpV(m_oldSnakePos[i].endPos,   e, morphEased);
        }

        m_snakes[i]->draw(window, s, e, m_theme, i, 1.f, slitherAmt * m_morphPhaseT);
    }

    // ---- Warning snakes (sabotage traps, pulsing red) ------
    const float warnPulse = 0.7f + 0.3f * std::sin(menuT * 6.f);
    const SnakePal warnPal{ { 210, 30, 30 }, { 255, 120, 100 }, { 160, 15, 15 } };

    for (const auto& ws : warnSnakes) {
        const sf::Vector2f wh = squareToCentre(ws.first);
        const sf::Vector2f wt = squareToCentre(ws.second);
        drawSnakeFull(window, wh, wt, warnPal, warnPulse, 0.f);
    }
}
