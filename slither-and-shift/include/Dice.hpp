#pragma once

#include <random>

// ============================================================
//  Dice.hpp
//  Self-contained dice class that manages roll animation state
//  and produces random results using a Mersenne-Twister RNG.
//
//  Call start() to begin a roll, update() each frame, and read
//  finalTotal once rolling == false.
// ============================================================

class Dice {
public:
    Dice();

    // ---- Roll lifecycle ------------------------------------

    /**
     * @brief Starts a new roll animation.
     *
     * @param doubleRoll  When true, two dice are rolled and their
     *                    values summed (Chaos-mode DOUBLE_ROLL power-up).
     */
    void start(bool doubleRoll);

    /**
     * @brief Advances the roll animation by dt seconds.
     *
     * During the animation the displayed face values shuffle rapidly.
     * Once elapsed >= duration the final random value is committed and
     * rolling is set to false.
     *
     * @param dt          Frame delta-time in seconds.
     * @param doubleRoll  Must match the value passed to start().
     */
    void update(float dt, bool doubleRoll);

    // ---- State ---------------------------------------------

    bool  rolling    = false;  ///< True while the roll animation is playing
    float elapsed    = 0.f;   ///< Seconds elapsed since start()
    float duration   = 0.95f; ///< Total roll animation length in seconds
    float wobble     = 0.f;   ///< Horizontal wobble offset for drawing

    /// Face values to display this frame (may still be animating)
    int faceA = 1;
    int faceB = 1;

    /// Committed total (faceA [+ faceB] after rolling finishes; 0 while animating)
    int finalTotal = 0;

private:
    std::mt19937 m_rng;
};
