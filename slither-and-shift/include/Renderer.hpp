#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Types.hpp"
#include "MathUtils.hpp"

// ============================================================
//  Renderer.hpp
//  Free functions for all low-level SFML drawing primitives.
//
//  Splitting these out of the main game class makes each drawing
//  routine independently testable and reusable by UI, Board, and
//  player-token code without pulling in unrelated state.
// ============================================================

// ---- Primitive helpers -------------------------------------

/**
 * @brief Draws a thick line segment as a filled quad.
 *
 * Used heavily by snake, ladder, and smile rendering where variable-
 * width strokes are needed without SFML's built-in line thickness.
 *
 * @param window  Render target.
 * @param a       Start point.
 * @param b       End point.
 * @param col     Fill colour.
 * @param hw      Half-width of the segment (total width = 2 * hw).
 */
void drawThickSeg(sf::RenderWindow& window,
                  sf::Vector2f a, sf::Vector2f b,
                  sf::Color col, float hw);

// ---- Token face parts --------------------------------------

/// Draws blush circles and a specular shine dot on an animal token.
void drawBlushShine(sf::RenderWindow& window, sf::Vector2f pos, float sz);

/// Draws two stylised eyes with iris, pupil, and specular glint.
void drawEyes(sf::RenderWindow& window, sf::Vector2f pos, float sz,
              sf::Color iris = {60, 170, 60});

/// Draws a curved smile composed of small thick segments.
void drawSmile(sf::RenderWindow& window, sf::Vector2f pos, float sz);

// ---- Animal token renderers --------------------------------

void drawCat   (sf::RenderWindow& window, sf::Vector2f pos, sf::Color color, float sz);
void drawDog   (sf::RenderWindow& window, sf::Vector2f pos, sf::Color color, float sz);
void drawRabbit(sf::RenderWindow& window, sf::Vector2f pos, sf::Color color, float sz);
void drawFox   (sf::RenderWindow& window, sf::Vector2f pos, sf::Color color, float sz);

/**
 * @brief Dispatches to the appropriate animal-specific draw function.
 *
 * @param window  Render target.
 * @param pos     Centre position.
 * @param color   Primary token colour (player-assigned).
 * @param animal  Animal type enum value.
 * @param sz      Radius override (defaults to TOKEN_SIZE).
 */
void drawAnimal(sf::RenderWindow& window,
                sf::Vector2f pos,
                sf::Color    color,
                AnimalType   animal,
                float        sz = TOKEN_SIZE);

// ---- Board entity renderers --------------------------------

/**
 * @brief Draws a complete snake from head to tail via a Bezier curve.
 *
 * The snake body is rendered in three passes:
 *   1. Drop shadow (offset, semi-transparent black)
 *   2. Body fill (full width)
 *   3. Belly stripe (narrower, lighter colour)
 * Head details (eyes, tongue) are added when alpha is high enough.
 *
 * @param window        Render target.
 * @param head          Head pixel position.
 * @param tail          Tail pixel position.
 * @param pal           Colour palette (body, belly, head).
 * @param alpha         Overall opacity ∈ [0, 1].
 * @param slitherPhase  When > 0, adds a sinusoidal lateral wave to the
 *                      curve — used during the board-reshuffle animation.
 */
void drawSnakeFull(sf::RenderWindow& window,
                   sf::Vector2f head, sf::Vector2f tail,
                   const SnakePal& pal,
                   float alpha,
                   float slitherPhase = 0.f);

/**
 * @brief Draws a complete ladder between two world positions.
 *
 * Renders two rails with evenly-spaced rungs plus a drop shadow and a
 * specular highlight stripe along each rail.
 *
 * @param window  Render target.
 * @param base    Bottom-rung pixel position.
 * @param top     Top-rung pixel position.
 * @param pal     Colour palette (rail, rung).
 * @param alpha   Overall opacity.
 */
void drawLadderFull(sf::RenderWindow& window,
                    sf::Vector2f base, sf::Vector2f top,
                    const LadderPal& pal,
                    float alpha);

// ---- Dice face ---------------------------------------------

/**
 * @brief Draws a single dice face (value 1-6) centred at the given position.
 *
 * Renders a rounded rectangle body with drop shadow, specular highlight,
 * and pip dots matching the face value.
 *
 * @param window  Render target.
 * @param center  Centre of the die face.
 * @param size    Side length in pixels.
 * @param face    Value to display (1–6).
 * @param wobble  Horizontal offset applied during a roll animation.
 */
void drawDiceFace(sf::RenderWindow& window,
                  sf::Vector2f center, float size,
                  int face, float wobble = 0.f);
