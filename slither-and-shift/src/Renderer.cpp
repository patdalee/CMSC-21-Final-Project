#include "Renderer.hpp"
#include "MathUtils.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

// ============================================================
//  Renderer.cpp
//  All low-level SFML drawing routines.
//
//  Functions are intentionally free (not class members) so they
//  can be shared by Board, UI, and Game without coupling to any
//  single object's state.
// ============================================================

// ---- Primitive helpers -------------------------------------

void drawThickSeg(sf::RenderWindow& window,
                  sf::Vector2f a, sf::Vector2f b,
                  sf::Color col, float hw)
{
    const sf::Vector2f dir  = vnorm(b - a);
    const sf::Vector2f perp = { -dir.y, dir.x };

    sf::ConvexShape quad(4);
    quad.setPoint(0, a + perp * hw);
    quad.setPoint(1, a - perp * hw);
    quad.setPoint(2, b - perp * hw);
    quad.setPoint(3, b + perp * hw);
    quad.setFillColor(col);
    window.draw(quad);
}

// ---- Token face shared helpers -----------------------------

void drawBlushShine(sf::RenderWindow& window, sf::Vector2f pos, float sz)
{
    // Two blush circles on either cheek
    sf::CircleShape bl(sz * 0.15f), br(sz * 0.15f);
    bl.setFillColor({ 255, 115, 140, 165 });
    br.setFillColor({ 255, 115, 140, 165 });
    bl.setPosition({pos.x - sz * 0.62f, pos.y + sz * 0.08f});
    br.setPosition({pos.x + sz * 0.32f, pos.y + sz * 0.08f});
    window.draw(bl);
    window.draw(br);

    // Specular shine in the upper-left quadrant of the face
    sf::CircleShape shine(sz * 0.17f);
    shine.setPosition({pos.x - sz * 0.52f, pos.y - sz * 0.62f});
    shine.setFillColor({ 255, 255, 255, 115 });
    window.draw(shine);
}

void drawEyes(sf::RenderWindow& window, sf::Vector2f pos, float sz,
              sf::Color iris)
{
    const float er = sz * 0.18f;  // eye radius

    for (int side = -1; side <= 1; side += 2) {
        const sf::Vector2f ep(pos.x + side * sz * 0.3f, pos.y - sz * 0.12f);

        // White sclera
        sf::CircleShape eye(er);
        eye.setOrigin({er, er});
        eye.setPosition(ep);
        eye.setFillColor({ 255, 255, 255 });
        window.draw(eye);

        // Iris + pupil
        const float pr = er * 0.55f;
        sf::CircleShape pupil(pr);
        pupil.setOrigin({pr, pr});
        pupil.setPosition(ep);
        pupil.setFillColor({ 15, 15, 15 });
        window.draw(pupil);

        // Specular glint
        const float gr = pr * 0.32f;
        sf::CircleShape glint(gr);
        glint.setOrigin({gr, gr});
        glint.setPosition(ep + sf::Vector2f(-pr * 0.3f, -pr * 0.3f));
        glint.setFillColor({ 255, 255, 255, 210 });
        window.draw(glint);

        (void)iris; // iris colour can be used for a coloured overlay if desired
    }
}

void drawSmile(sf::RenderWindow& window, sf::Vector2f pos, float sz)
{
    // Approximate a smile arc with 8 short thick segments.
    constexpr int   SEGS = 8;
    const float mw = sz * 0.32f;  // half-width of smile
    const float mh = sz * 0.12f;  // vertical amplitude

    for (int i = 0; i < SEGS; ++i) {
        const float t0 = static_cast<float>(i)     / SEGS;
        const float t1 = static_cast<float>(i + 1) / SEGS;
        const float a0 = t0 * 3.14159f;
        const float a1 = t1 * 3.14159f;

        const sf::Vector2f p0(pos.x - mw + 2.f * mw * t0,
                              pos.y + sz * 0.25f + std::sin(a0) * mh);
        const sf::Vector2f p1(pos.x - mw + 2.f * mw * t1,
                              pos.y + sz * 0.25f + std::sin(a1) * mh);
        drawThickSeg(window, p0, p1, { 20, 15, 15, 200 }, 1.6f);
    }
}

// ---- Animal token drawing ----------------------------------

void drawCat(sf::RenderWindow& window, sf::Vector2f pos,
             sf::Color color, float sz)
{
    // Outer ear triangles
    sf::ConvexShape eL(3), eR(3);
    eL.setPoint(0, { pos.x - sz * 0.46f, pos.y - sz * 0.88f });
    eL.setPoint(1, { pos.x - sz * 0.90f, pos.y - sz * 0.20f });
    eL.setPoint(2, { pos.x - sz * 0.10f, pos.y - sz * 0.40f });
    eL.setFillColor(color);
    eR.setPoint(0, { pos.x + sz * 0.46f, pos.y - sz * 0.88f });
    eR.setPoint(1, { pos.x + sz * 0.10f, pos.y - sz * 0.40f });
    eR.setPoint(2, { pos.x + sz * 0.90f, pos.y - sz * 0.20f });
    eR.setFillColor(color);
    window.draw(eL); window.draw(eR);

    // Inner ear (lighter colour)
    const sf::Color inner(
        std::min(255, color.r + 75),
        std::min(255, color.g + 25),
        std::min(255, color.b + 55));
    sf::ConvexShape ieL(3), ieR(3);
    ieL.setPoint(0, { pos.x - sz * 0.46f, pos.y - sz * 0.80f });
    ieL.setPoint(1, { pos.x - sz * 0.76f, pos.y - sz * 0.28f });
    ieL.setPoint(2, { pos.x - sz * 0.16f, pos.y - sz * 0.43f });
    ieL.setFillColor(inner);
    ieR.setPoint(0, { pos.x + sz * 0.46f, pos.y - sz * 0.80f });
    ieR.setPoint(1, { pos.x + sz * 0.16f, pos.y - sz * 0.43f });
    ieR.setPoint(2, { pos.x + sz * 0.76f, pos.y - sz * 0.28f });
    ieR.setFillColor(inner);
    window.draw(ieL); window.draw(ieR);

    // Circular face body
    sf::CircleShape body(sz);
    body.setOrigin({sz, sz});
    body.setPosition(pos);
    body.setFillColor(color);
    body.setOutlineColor({ 255, 255, 255, 110 });
    body.setOutlineThickness(2.f);
    window.draw(body);

    drawEyes(window, pos, sz, { 55, 165, 55 });

    // Nose
    sf::CircleShape nose(sz * 0.09f);
    nose.setOrigin({sz * 0.09f, sz * 0.09f});
    nose.setPosition({pos.x, pos.y + sz * 0.14f});
    nose.setFillColor({ 255, 165, 175 });
    window.draw(nose);

    // Whiskers (two rows per side)
    for (int side = -1; side <= 1; side += 2) {
        for (int row = 0; row < 2; ++row) {
            sf::RectangleShape wh({ sz * 0.72f, 1.3f });
            wh.setOrigin({0.f, 0.65f});
            const float wx = (side == -1)
                ? pos.x - sz * 0.82f
                : pos.x + side * sz * 0.10f;
            wh.setPosition({wx, pos.y + sz * 0.11f + row * sz * 0.1f});
            wh.setRotation(sf::degrees(static_cast<float>(side)) * (row == 0 ? -8.f : 8.f));
            wh.setFillColor({ 255, 255, 255, 150 });
            window.draw(wh);
        }
    }

    drawSmile(window, pos, sz);
    drawBlushShine(window, pos, sz);
}

void drawDog(sf::RenderWindow& window, sf::Vector2f pos,
             sf::Color color, float sz)
{
    // Floppy ears (darker shade of token colour)
    const sf::Color earCol(
        std::max(0, color.r - 45),
        std::max(0, color.g - 35),
        std::max(0, color.b - 25));

    sf::CircleShape earL(sz * 0.44f), earR(sz * 0.44f);
    earL.setOrigin({sz * 0.44f, 0.f});
    earL.setPosition({pos.x - sz * 0.82f, pos.y - sz * 0.55f});
    earR.setOrigin({0.f, 0.f});
    earR.setPosition({pos.x + sz * 0.38f, pos.y - sz * 0.55f});
    earL.setFillColor(earCol); earR.setFillColor(earCol);
    window.draw(earL); window.draw(earR);

    // Circular face body
    sf::CircleShape body(sz);
    body.setOrigin({sz, sz});
    body.setPosition(pos);
    body.setFillColor(color);
    body.setOutlineColor({ 255, 255, 255, 110 });
    body.setOutlineThickness(2.f);
    window.draw(body);

    // Muzzle patch (lighter oval)
    sf::CircleShape muzzle(sz * 0.42f, 20);
    muzzle.setOrigin({sz * 0.42f, sz * 0.32f});
    muzzle.setScale({1.f, 0.75f});
    muzzle.setPosition({pos.x, pos.y + sz * 0.15f});
    const sf::Color mCol(
        std::min(255, color.r + 55),
        std::min(255, color.g + 45),
        std::min(255, color.b + 38));
    muzzle.setFillColor(mCol);
    window.draw(muzzle);

    drawEyes(window, pos, sz, { 110, 72, 25 });

    // Nose (dark oval with highlight)
    sf::CircleShape nose(sz * 0.16f);
    nose.setOrigin({sz * 0.16f, sz * 0.16f});
    nose.setPosition({pos.x, pos.y + sz * 0.18f});
    nose.setFillColor({ 40, 25, 20 });
    window.draw(nose);

    sf::CircleShape noseHL(sz * 0.06f);
    noseHL.setOrigin({sz * 0.06f, sz * 0.06f});
    noseHL.setPosition({pos.x - sz * 0.07f, pos.y + sz * 0.12f});
    noseHL.setFillColor({ 90, 60, 50, 180 });
    window.draw(noseHL);

    drawSmile(window, pos, sz);
    drawBlushShine(window, pos, sz);
}

void drawRabbit(sf::RenderWindow& window, sf::Vector2f pos,
                sf::Color color, float sz)
{
    // Long upright ears (ellipses, each with a pink inner)
    const sf::Color earInner{ 255, 195, 205 };
    for (int side = -1; side <= 1; side += 2) {
        sf::CircleShape earBase(sz * 0.22f, 20);
        earBase.setOrigin({sz * 0.22f, sz * 0.22f});
        earBase.setScale({1.f, 2.4f});
        earBase.setPosition({pos.x + side * sz * 0.38f, pos.y - sz * 1.2f});
        earBase.setFillColor(color);
        window.draw(earBase);

        sf::CircleShape earInn(sz * 0.13f, 20);
        earInn.setOrigin({sz * 0.13f, sz * 0.13f});
        earInn.setScale({1.f, 2.1f});
        earInn.setPosition({pos.x + side * sz * 0.38f, pos.y - sz * 1.15f});
        earInn.setFillColor(earInner);
        window.draw(earInn);
    }

    // Circular face body
    sf::CircleShape body(sz);
    body.setOrigin({sz, sz});
    body.setPosition(pos);
    body.setFillColor(color);
    body.setOutlineColor({ 255, 255, 255, 110 });
    body.setOutlineThickness(2.f);
    window.draw(body);

    drawEyes(window, pos, sz, { 200, 70, 70 });

    // Small pink oval nose
    sf::CircleShape nose(sz * 0.10f, 20);
    nose.setOrigin({sz * 0.10f, sz * 0.08f});
    nose.setScale({1.f, 0.8f});
    nose.setPosition({pos.x, pos.y + sz * 0.12f});
    nose.setFillColor({ 255, 150, 160 });
    window.draw(nose);

    drawSmile(window, pos, sz);
    drawBlushShine(window, pos, sz);
}

void drawFox(sf::RenderWindow& window, sf::Vector2f pos,
             sf::Color /*color*/, float sz)
{
    // Fox uses fixed theme colours rather than the player-assigned colour.
    const sf::Color foxOrange{ 238, 108,  38 };
    const sf::Color foxDark  { 175,  68,  18 };
    const sf::Color foxCream { 255, 242, 230 };

    // Pointed outer ears
    sf::ConvexShape eL(3), eR(3);
    eL.setPoint(0, { pos.x - sz * 0.44f, pos.y - sz * 0.92f });
    eL.setPoint(1, { pos.x - sz * 0.88f, pos.y - sz * 0.18f });
    eL.setPoint(2, { pos.x - sz * 0.08f, pos.y - sz * 0.40f });
    eL.setFillColor(foxOrange);
    eR.setPoint(0, { pos.x + sz * 0.44f, pos.y - sz * 0.92f });
    eR.setPoint(1, { pos.x + sz * 0.08f, pos.y - sz * 0.40f });
    eR.setPoint(2, { pos.x + sz * 0.88f, pos.y - sz * 0.18f });
    eR.setFillColor(foxOrange);
    window.draw(eL); window.draw(eR);

    // Dark inner ear triangles
    sf::ConvexShape ieL(3), ieR(3);
    ieL.setPoint(0, { pos.x - sz * 0.44f, pos.y - sz * 0.84f });
    ieL.setPoint(1, { pos.x - sz * 0.74f, pos.y - sz * 0.25f });
    ieL.setPoint(2, { pos.x - sz * 0.14f, pos.y - sz * 0.43f });
    ieL.setFillColor(foxDark);
    ieR.setPoint(0, { pos.x + sz * 0.44f, pos.y - sz * 0.84f });
    ieR.setPoint(1, { pos.x + sz * 0.14f, pos.y - sz * 0.43f });
    ieR.setPoint(2, { pos.x + sz * 0.74f, pos.y - sz * 0.25f });
    ieR.setFillColor(foxDark);
    window.draw(ieL); window.draw(ieR);

    // Main face circle (orange)
    sf::CircleShape body(sz);
    body.setOrigin({sz, sz});
    body.setPosition(pos);
    body.setFillColor(foxOrange);
    body.setOutlineColor({ 255, 255, 255, 110 });
    body.setOutlineThickness(2.f);
    window.draw(body);

    // Cream-coloured muzzle oval
    sf::CircleShape muzz(sz * 0.52f, 20);
    muzz.setOrigin({sz * 0.52f, sz * 0.38f});
    muzz.setScale({1.f, 0.75f});
    muzz.setPosition({pos.x, pos.y + sz * 0.16f});
    muzz.setFillColor(foxCream);
    window.draw(muzz);

    drawEyes(window, pos, sz, { 195, 125, 25 });

    // Tiny dark nose
    sf::CircleShape nose(sz * 0.10f);
    nose.setOrigin({sz * 0.10f, sz * 0.10f});
    nose.setPosition({pos.x, pos.y + sz * 0.16f});
    nose.setFillColor({ 25, 18, 18 });
    window.draw(nose);

    drawSmile(window, pos, sz);
    drawBlushShine(window, pos, sz);
}

void drawAnimal(sf::RenderWindow& window,
                sf::Vector2f pos, sf::Color color,
                AnimalType animal, float sz)
{
    switch (animal) {
    case AnimalType::CAT:    drawCat   (window, pos, color, sz); break;
    case AnimalType::DOG:    drawDog   (window, pos, color, sz); break;
    case AnimalType::RABBIT: drawRabbit(window, pos, color, sz); break;
    case AnimalType::FOX:    drawFox   (window, pos, color, sz); break;
    }
}

// ---- Snake rendering ---------------------------------------

void drawSnakeFull(sf::RenderWindow& window,
                   sf::Vector2f head, sf::Vector2f tail,
                   const SnakePal& pal,
                   float alpha,
                   float slitherPhase)
{
    if (alpha <= 0.f) return;

    // Build Bezier control points for the snake's S-curve.
    sf::Vector2f cp1, cp2;
    snakeBezCP(head, tail, cp1, cp2);

    // Sample the Bezier curve into a polyline.
    constexpr int STEPS = 72;
    std::vector<sf::Vector2f> pts;
    pts.reserve(STEPS + 1);

    for (int i = 0; i <= STEPS; ++i) {
        const float        t = static_cast<float>(i) / STEPS;
        sf::Vector2f       p = cubicBez(head, cp1, cp2, tail, t);

        // Add a sinusoidal lateral wave during board-reshuffle animation.
        // slitherPhase > 0 drives the body into an S-wriggle.
        if (slitherPhase > 0.f) {
            const sf::Vector2f dt_ = (i < STEPS)
                ? vnorm(cubicBez(head, cp1, cp2, tail, t + 0.01f) - p)
                : vnorm(p - cubicBez(head, cp1, cp2, tail, t - 0.01f));
            const sf::Vector2f perp(-dt_.y, dt_.x);
            const float wave = std::sin((t * 6.28318f * 2.f) - slitherPhase * 8.f)
                             * slitherPhase * 12.f;
            p = p + perp * wave;
        }
        pts.push_back(p);
    }

    // Pass 1: drop shadow (offset, semi-transparent black)
    for (int i = 0; i < static_cast<int>(pts.size()) - 1; ++i) {
        drawThickSeg(window,
                     pts[i]     + sf::Vector2f(3.f, 4.f),
                     pts[i + 1] + sf::Vector2f(3.f, 4.f),
                     withAlpha({ 0, 0, 0 }, alpha * 0.20f), 12.f);
    }

    // Pass 2: body fill
    for (int i = 0; i < static_cast<int>(pts.size()) - 1; ++i) {
        drawThickSeg(window, pts[i], pts[i + 1], withAlpha(pal.body, alpha), 10.f);
    }

    // Pass 3: belly stripe (every other segment, narrower)
    for (int i = 0; i < static_cast<int>(pts.size()) - 1; i += 2) {
        drawThickSeg(window, pts[i], pts[i + 1], withAlpha(pal.belly, alpha), 4.5f);
    }

    // Tail tip cap
    sf::CircleShape tip(5.5f);
    tip.setOrigin({5.5f, 5.5f});
    tip.setPosition(pts.back());
    tip.setFillColor(withAlpha(pal.body, alpha));
    window.draw(tip);

    // Head circle
    const sf::Vector2f hDir  = vnorm(cp1 - head);
    const sf::Vector2f hPerp = { -hDir.y, hDir.x };
    sf::CircleShape hd(16.f);
    hd.setOrigin({16.f, 16.f});
    hd.setPosition(head);
    hd.setFillColor(withAlpha(pal.head, alpha));
    window.draw(hd);

    // Skip face details at low alpha (e.g. fading in during morph)
    if (alpha < 0.35f) return;

    // Eyes on the head
    const float er = 5.2f;
    for (int side = -1; side <= 1; side += 2) {
        const sf::Vector2f ep = head + hPerp * static_cast<float>(side) * 8.f;
        sf::CircleShape e(er);
        e.setOrigin({er, er});
        e.setPosition(ep);
        e.setFillColor(withAlpha({ 255, 255, 255 }, alpha));
        window.draw(e);
        sf::CircleShape pu(er * 0.58f);
        pu.setOrigin({er * 0.58f, er * 0.58f});
        pu.setPosition(ep);
        pu.setFillColor(withAlpha({ 10, 10, 10 }, alpha));
        window.draw(pu);
    }

    // Forked tongue
    const sf::Vector2f tb = head + hDir * 15.f;
    const sf::Vector2f tt = tb   + hDir * 9.f;
    drawThickSeg(window, tb, tt, withAlpha({ 220, 28, 28 }, alpha), 1.8f);
    drawThickSeg(window, tt, tt + hDir * 5.f + hPerp * 4.f,
                 withAlpha({ 220, 28, 28 }, alpha), 1.2f);
    drawThickSeg(window, tt, tt + hDir * 5.f - hPerp * 4.f,
                 withAlpha({ 220, 28, 28 }, alpha), 1.2f);
}

// ---- Ladder rendering ---------------------------------------

void drawLadderFull(sf::RenderWindow& window,
                    sf::Vector2f base, sf::Vector2f top,
                    const LadderPal& pal, float alpha)
{
    if (alpha <= 0.f) return;

    const sf::Vector2f d    = top - base;
    const float        dist = vlen(d);
    const sf::Vector2f fwd  = vnorm(d);
    const sf::Vector2f perp = { -fwd.y, fwd.x };
    const float        rg   = 13.f;  // half-gap between rails

    const sf::Vector2f r1a = base + perp * rg;
    const sf::Vector2f r1b = top  + perp * rg;
    const sf::Vector2f r2a = base - perp * rg;
    const sf::Vector2f r2b = top  - perp * rg;

    // Drop shadows for each rail
    drawThickSeg(window, r1a + sf::Vector2f(3,4), r1b + sf::Vector2f(3,4),
                 withAlpha({ 0,0,0 }, alpha * 0.20f), 6.f);
    drawThickSeg(window, r2a + sf::Vector2f(3,4), r2b + sf::Vector2f(3,4),
                 withAlpha({ 0,0,0 }, alpha * 0.20f), 6.f);

    // Rails
    drawThickSeg(window, r1a, r1b, withAlpha(pal.rail, alpha), 6.f);
    drawThickSeg(window, r2a, r2b, withAlpha(pal.rail, alpha), 6.f);

    // Specular highlight stripe along each rail
    const sf::Color hl{
        static_cast<std::uint8_t>(std::min(255, pal.rail.r + 60)),
        static_cast<std::uint8_t>(std::min(255, pal.rail.g + 45)),
        static_cast<std::uint8_t>(std::min(255, pal.rail.b + 35))
    };
    drawThickSeg(window, r1a, r1b, withAlpha(hl, alpha), 2.f);
    drawThickSeg(window, r2a, r2b, withAlpha(hl, alpha), 2.f);

    // Evenly-spaced rungs
    const int numRungs = std::max(4, static_cast<int>(dist / 34.f));
    for (int i = 0; i <= numRungs; ++i) {
        const float        t   = static_cast<float>(i) / numRungs;
        const sf::Vector2f mid = base + d * t;
        const sf::Vector2f ra  = mid + perp * (rg + 2.f);
        const sf::Vector2f rb  = mid - perp * (rg + 2.f);

        // Shadow under each rung
        drawThickSeg(window,
                     ra + sf::Vector2f(1.5f, 2.f),
                     rb + sf::Vector2f(1.5f, 2.f),
                     withAlpha({ 0, 0, 0, 55 }, alpha), 4.f);
        drawThickSeg(window, ra, rb, withAlpha(pal.rung, alpha), 4.f);
    }
}

// ---- Dice face rendering -----------------------------------

void drawDiceFace(sf::RenderWindow& window,
                  sf::Vector2f center, float size,
                  int face, float wobble)
{
    const float        half = size / 2.f;
    const sf::Vector2f pos(center.x - half + wobble, center.y - half);

    // Drop shadow
    sf::RectangleShape shadow({ size, size });
    shadow.setPosition({pos.x + 4.f, pos.y + 5.f});
    shadow.setFillColor({ 0, 0, 0, 55 });
    window.draw(shadow);

    // Die body
    sf::RectangleShape body({ size, size });
    body.setPosition(pos);
    body.setFillColor({ 248, 248, 252 });
    body.setOutlineColor({ 145, 145, 165 });
    body.setOutlineThickness(2.f);
    window.draw(body);

    // Top edge highlight
    sf::RectangleShape topHL({ size, size * 0.17f });
    topHL.setPosition(pos);
    topHL.setFillColor({ 255, 255, 255, 85 });
    window.draw(topHL);

    // Right-edge shadow band
    sf::RectangleShape rightSh({ size * 0.10f, size });
    rightSh.setPosition({pos.x + size - size * 0.10f, pos.y});
    rightSh.setFillColor({ 0, 0, 0, 22 });
    window.draw(rightSh);

    // Draw pips using a lambda
    const float dotR   = size * 0.095f;
    const float spread = size * 0.28f;
    const sf::Color dotCol{ 22, 22, 40 };

    auto dot = [&](float nx, float ny) {
        sf::CircleShape d(dotR);
        d.setOrigin({dotR, dotR});
        d.setPosition({center.x + wobble + nx * spread, center.y + ny * spread});
        d.setFillColor(dotCol);
        window.draw(d);

        // Subtle glint on each pip
        const float gr = dotR * 0.32f;
        sf::CircleShape glint(gr);
        glint.setOrigin({gr, gr});
        glint.setPosition({center.x + wobble + nx * spread - dotR * 0.28f, center.y + ny * spread          - dotR * 0.28f});
        glint.setFillColor({ 55, 55, 75, 150 });
        window.draw(glint);
    };

    switch (face) {
    case 1: dot( 0,  0); break;
    case 2: dot(-1, -1); dot( 1,  1); break;
    case 3: dot(-1, -1); dot( 0,  0); dot( 1,  1); break;
    case 4: dot(-1, -1); dot( 1, -1); dot(-1,  1); dot( 1,  1); break;
    case 5: dot(-1, -1); dot( 1, -1); dot( 0,  0); dot(-1,  1); dot( 1,  1); break;
    case 6:
        for (int r = -1; r <= 1; ++r)
            for (int c = -1; c <= 1; c += 2)
                dot(static_cast<float>(c), static_cast<float>(r));
        break;
    default: break;
    }
}
