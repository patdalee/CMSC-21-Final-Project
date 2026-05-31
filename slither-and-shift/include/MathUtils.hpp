#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <cstdint>

// ============================================================
//  MathUtils.hpp
//  Inline geometry and colour helper functions used by rendering,
//  animation, and board-layout code.
//
//  All functions are defined inline here to avoid a separate .cpp
//  translation unit while keeping them out of consumer headers.
// ============================================================

// ---- Vector helpers ----------------------------------------

/// Returns the Euclidean length of a 2-D vector.
inline float vlen(sf::Vector2f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

/// Returns the unit vector in the direction of v (or zero if v is zero).
inline sf::Vector2f vnorm(sf::Vector2f v)
{
    const float l = vlen(v);
    return l > 0.f ? sf::Vector2f(v.x / l, v.y / l) : sf::Vector2f{0.f, 0.f};
}

/// Linear interpolation between two positions.
inline sf::Vector2f lerpV(sf::Vector2f a, sf::Vector2f b, float t)
{
    return a + (b - a) * t;
}

// ---- Cubic Bezier ------------------------------------------

/**
 * @brief Evaluates a cubic Bezier curve at parameter t ∈ [0, 1].
 *
 * p0 / p3 are the endpoints; p1 / p2 are the control points.
 * Used for smooth snake-slide and reshuffle animations.
 */
inline sf::Vector2f cubicBez(sf::Vector2f p0, sf::Vector2f p1,
                              sf::Vector2f p2, sf::Vector2f p3, float t)
{
    const float u = 1.f - t;
    return u*u*u*p0 + 3.f*u*u*t*p1 + 3.f*u*t*t*p2 + t*t*t*p3;
}

// ---- Easing functions --------------------------------------

/// Smooth-step (cubic) — accelerates then decelerates.
inline float easeInOut(float t)
{
    return t * t * (3.f - 2.f * t);
}

/**
 * @brief Ease-out-back — overshoots slightly before settling.
 *
 * Gives the board-reshuffle a satisfying "snap" at the end of the morph.
 */
inline float easeOutBack(float t)
{
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.f;
    return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
}

// ---- Colour helpers ----------------------------------------

/// Returns a copy of c with its alpha channel set from a float ∈ [0, 1].
inline sf::Color withAlpha(sf::Color c, float a)
{
    c.a = static_cast<std::uint8_t>(std::max(0.f, std::min(255.f, a * 255.f)));
    return c;
}

/// Per-component linear interpolation between two SFML colours.
inline sf::Color lerpColor(sf::Color a, sf::Color b, float t)
{
    return sf::Color(
        static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<std::uint8_t>(a.b + (b.b - a.b) * t),
        static_cast<std::uint8_t>(a.a + (b.a - a.a) * t));
}

// ---- Board layout ------------------------------------------

/**
 * @brief Converts a board square number (1–100) to its pixel centre.
 *
 * The board is a 10×10 grid numbered 1–100 in a boustrophedon
 * (snake) pattern: row 0 goes left-to-right (squares 1-10),
 * row 1 right-to-left (11-20), etc.
 */
inline sf::Vector2f squareToCentre(int sq)
{
    if (sq < 1)   sq = 1;
    if (sq > 100) sq = 100;

    const int idx = sq - 1;
    const int row = idx / 10;
    int       col = idx % 10;

    // Odd rows run right-to-left (boustrophedon numbering)
    if (row % 2 != 0)
        col = 9 - col;

    // Row 0 is at the bottom; flip so row 0 = bottom of screen
    const int displayRow = 9 - row;

    return {
        static_cast<float>(45  + col * 72 + 36),
        static_cast<float>(35  + displayRow * 72 + 36)
    };
}

// ---- Bezier control-points for snake curves ----------------

/**
 * @brief Computes two cubic Bezier control points for a snake path.
 *
 * Places cp1 and cp2 so the curve wiggles like a real snake body —
 * the perpendicular offset creates the characteristic S-bend.
 *
 * @param head  World position of the snake's head square.
 * @param tail  World position of the snake's tail square.
 * @param cp1   [out] First control point.
 * @param cp2   [out] Second control point.
 */
inline void snakeBezCP(sf::Vector2f head, sf::Vector2f tail,
                        sf::Vector2f& cp1, sf::Vector2f& cp2)
{
    const sf::Vector2f d    = tail - head;
    const float        dist = vlen(d);
    const sf::Vector2f perp(-d.y / (dist + 0.001f), d.x / (dist + 0.001f));
    const float        wave = std::min(dist * 0.42f, 70.f);

    cp1 = head + d * 0.3f + perp * wave;
    cp2 = head + d * 0.7f - perp * wave;
}
