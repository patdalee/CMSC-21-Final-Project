#include "Dice.hpp"
#include <cmath>
#include <algorithm>
#include <random>

// ============================================================
//  Dice.cpp
//  Implements the Dice roll-animation lifecycle.
// ============================================================

Dice::Dice()
    : m_rng(std::random_device{}())
{}

void Dice::start(bool doubleRoll)
{
    rolling    = true;
    elapsed    = 0.f;
    duration   = doubleRoll ? 1.1f : 0.9f;  // double-roll lingers slightly longer
    faceA      = 1;
    faceB      = 1;
    finalTotal = 0;
    wobble     = 0.f;
}

void Dice::update(float dt, bool doubleRoll)
{
    if (!rolling) return;

    elapsed += dt;

    // Sinusoidal wobble decays to zero as the roll approaches its end.
    wobble = std::sin(elapsed * 26.f) * 4.f * (1.f - (elapsed / duration));

    // Shuffle the displayed face value every other frame until near the end.
    std::uniform_int_distribution<int> d6(1, 6);
    const bool shuffling = (static_cast<int>(elapsed * 18.f) % 2 == 0)
                           && (elapsed < duration - 0.18f);
    if (shuffling) {
        faceA = d6(m_rng);
        if (doubleRoll) faceB = d6(m_rng);
    }

    // Commit the final value when the animation duration has elapsed.
    if (elapsed >= duration) {
        faceA      = d6(m_rng);
        faceB      = doubleRoll ? d6(m_rng) : 0;
        finalTotal = faceA + (doubleRoll ? faceB : 0);
        rolling    = false;
        wobble     = 0.f;
    }
}
