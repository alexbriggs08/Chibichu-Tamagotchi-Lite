// Animations.h

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "raylib.h"
enum PetState {
    IDLE,
    EATING,
    CLEANING,
    SLEEPING,
    PLAYING,
    HAPPY,
    SAD
};

// Returns the correct frame based on pet state and animation frame index
const char* getPetFrame(PetState state, int frame);

// Get background color based on time of day
Color getBackgroundColor(int hour);

// Get a description for the current time of day
const char* getTimeDescription(int hour);

#endif // ANIMATIONS_H