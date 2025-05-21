#ifndef NEEDS_SYSTEM_H
#define NEEDS_SYSTEM_H

#include "raylib.h"

// Expanded needs system with more properties
struct Needs {
    int hunger;
    int happiness;
    int cleanliness;
    int energy;
    bool isSleeping;

    // Track mood state based on overall needs
    int moodState;  // 0=sad, 1=neutral, 2=happy

    // Track play stats
    int playLevel;
    float lastPlayTime;
};

// Constants for needs thresholds
const int NEEDS_LOW = 25;
const int NEEDS_MEDIUM = 50;
const int NEEDS_HIGH = 75;

// Functions declarations
void InitNeeds(Needs& needs);
void UpdateNeeds(Needs& needs, float deltaTime);
void DrawNeeds(const Needs& needs);
void UpdateMoodState(Needs& needs);
const char* GetMoodText(const Needs& needs);
Color GetMoodColor(const Needs& needs);

#endif // NEEDS_SYSTEM_H