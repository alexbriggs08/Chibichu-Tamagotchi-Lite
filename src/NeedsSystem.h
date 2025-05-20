#ifndef NEEDS_SYSTEM_H
#define NEEDS_SYSTEM_H

#include "raylib.h"

struct Needs {
    int hunger;
    int happiness;
    int cleanliness;
    bool isSleeping;
};

// Functions declarations...
void InitNeeds(Needs& needs);
void UpdateNeeds(Needs& needs);
void DrawNeeds(const Needs& needs);

#endif // NEEDS_SYSTEM_H
