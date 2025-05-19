#include "NeedsSystem.h"

void InitNeeds(Needs& needs) {
    needs.hunger = 0;
    needs.happiness = 0;
}

void UpdateNeeds(Needs& needs) {
    needs.hunger += 1;
    if (needs.hunger > 100) needs.hunger = 100;

    if (needs.hunger <= 20) {
        needs.happiness -= 1;
        if (needs.happiness < 0) needs.happiness = 0;
    }
}


void DrawNeeds(const Needs& needs) {
    DrawText("Hunger:", 50, 50, 20, BLACK);
    DrawRectangle(150, 50, 200, 25, LIGHTGRAY);
    DrawRectangle(150, 50, 2 * (100 - needs.hunger), 25, RED);

    DrawText("Happiness:", 50, 100, 20, BLACK);
    DrawRectangle(200, 100, 200, 25, LIGHTGRAY);
    DrawRectangle(200, 100, 2 * needs.happiness, 25, GREEN);
}
    