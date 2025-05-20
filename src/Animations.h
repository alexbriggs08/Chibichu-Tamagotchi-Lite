// Animations.h

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

enum PetState {
    IDLE,
    EATING,
    CLEANING,
    SLEEPING
};

// Returns the correct frame based on pet state and animation frame index
const char* getPetFrame(PetState state, int frame);

#endif // ANIMATIONS_H
