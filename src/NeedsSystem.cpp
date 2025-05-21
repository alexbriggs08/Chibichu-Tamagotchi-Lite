#include "NeedsSystem.h"
#include <cmath>

void InitNeeds(Needs& needs) {
    needs.hunger = 70;
    needs.happiness = 50;
    needs.cleanliness = 60;
    needs.energy = 100;
    needs.isSleeping = false;
    needs.moodState = 1; // Start with neutral mood
    needs.playLevel = 0;
    needs.lastPlayTime = 0;
}

void UpdateNeeds(Needs& needs, float deltaTime) {
    // Only decrease needs if pet is awake
    if (!needs.isSleeping) {
        // Decrease hunger gradually
        needs.hunger -= 0.3 * deltaTime;
        if (needs.hunger < 0) needs.hunger = 0;

        // Decrease cleanliness gradually
        needs.cleanliness -= 0.25 * deltaTime;
        if (needs.cleanliness < 0) needs.cleanliness = 0;

        // Decrease energy when awake
        needs.energy -= 0.2 * deltaTime;
        if (needs.energy < 0) needs.energy = 0;

        // Happiness decreases faster when other needs are low
        float happinessDecay = 0.1;
        if (needs.hunger <= NEEDS_LOW) happinessDecay += 0.2;
        if (needs.cleanliness <= NEEDS_LOW) happinessDecay += 0.15;
        if (needs.energy <= NEEDS_LOW) happinessDecay += 0.25;

        needs.happiness -= happinessDecay * deltaTime;
        if (needs.happiness < 0) needs.happiness = 0;
    }
    else {
        // When sleeping, energy recovers
        needs.energy += 0.4 * deltaTime;
        if (needs.energy > 100) needs.energy = 100;

        // Happiness slightly increases during sleep if energy was low
        if (needs.energy < NEEDS_MEDIUM) {
            needs.happiness += 0.05 * deltaTime;
            if (needs.happiness > 100) needs.happiness = 100;
        }
    }

    // Play level decays over time
    if (needs.playLevel > 0) {
        needs.playLevel -= 0.15 * deltaTime;
        if (needs.playLevel < 0) needs.playLevel = 0;
    }

    // Update the mood state based on overall needs
    UpdateMoodState(needs);
}

void UpdateMoodState(Needs& needs) {
    // Calculate average well-being
    float wellbeing = (needs.hunger + needs.happiness + needs.cleanliness + needs.energy) / 4.0f;

    // Set mood state based on wellbeing
    if (wellbeing < NEEDS_LOW) {
        needs.moodState = 0; // Sad
    }
    else if (wellbeing < NEEDS_HIGH) {
        needs.moodState = 1; // Neutral
    }
    else {
        needs.moodState = 2; // Happy
    }
}

const char* GetMoodText(const Needs& needs) {
    if (needs.isSleeping) return "Sleeping";

    switch (needs.moodState) {
    case 0: return "Sad";
    case 1: return "Content";
    case 2: return "Happy";
    default: return "Unknown";
    }
}

Color GetMoodColor(const Needs& needs) {
    if (needs.isSleeping) return DARKBLUE;

    switch (needs.moodState) {
    case 0: return RED;
    case 1: return ORANGE;
    case 2: return GREEN;
    default: return GRAY;
    }
}

void DrawNeeds(const Needs& needs) {
    int baseY = 50;
    int spacing = 40;
    int totalCircles = 5;
    int circleRadius = 8;
    int circlePadding = 5;

    // Draw Hunger
    DrawText("Hunger:", 50, baseY, 20, BLACK);
    int filledCircles = (int)ceil((needs.hunger / 100.0f) * totalCircles);
    int startX = 150;
    int y = baseY + 10;

    for (int i = 0; i < totalCircles; i++) {
        int x = startX + i * (circleRadius * 2 + circlePadding);
        Color circleColor = (i < filledCircles) ? DARKGREEN : LIGHTGRAY;
        DrawCircle(x, y, circleRadius, circleColor);
    }

    // Draw Happiness
    baseY += spacing;
    DrawText("Happiness:", 50, baseY, 20, BLACK);
    filledCircles = (int)ceil((needs.happiness / 100.0f) * totalCircles);
    y = baseY + 10;

    for (int i = 0; i < totalCircles; i++) {
        int x = startX + i * (circleRadius * 2 + circlePadding);
        Color circleColor = (i < filledCircles) ? PINK : LIGHTGRAY;
        DrawCircle(x, y, circleRadius, circleColor);
    }

    // Draw Cleanliness
    baseY += spacing;
    DrawText("Cleanliness:", 50, baseY, 20, BLACK);
    filledCircles = (int)ceil((needs.cleanliness / 100.0f) * totalCircles);
    y = baseY + 10;

    for (int i = 0; i < totalCircles; i++) {
        int x = startX + i * (circleRadius * 2 + circlePadding);
        Color circleColor = (i < filledCircles) ? SKYBLUE : LIGHTGRAY;
        DrawCircle(x, y, circleRadius, circleColor);
    }

    // Draw Energy
    baseY += spacing;
    DrawText("Energy:", 50, baseY, 20, BLACK);
    filledCircles = (int)ceil((needs.energy / 100.0f) * totalCircles);
    y = baseY + 10;

    for (int i = 0; i < totalCircles; i++) {
        int x = startX + i * (circleRadius * 2 + circlePadding);
        Color circleColor = (i < filledCircles) ? GOLD : LIGHTGRAY;
        DrawCircle(x, y, circleRadius, circleColor);
    }

    // Draw mood indicator
    baseY += spacing;
    DrawText("Mood:", 50, baseY, 20, BLACK);
    DrawText(GetMoodText(needs), 150, baseY, 20, GetMoodColor(needs));
}