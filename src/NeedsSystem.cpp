#include "NeedsSystem.h"
#include <cmath>

void InitNeeds(Needs& needs) {
    needs.hunger = 100;
    needs.happiness = 100;
}

void UpdateNeeds(Needs& needs) {
    needs.hunger -= 1;
    if (needs.hunger < 0) needs.hunger = 0;

    if (needs.hunger <= 20) {
        needs.happiness -= 1;
        if (needs.happiness < 0) needs.happiness = 0;
    }
}




void DrawNeeds(const Needs& needs) {
    DrawText("Hunger:", 50, 50, 20, BLACK);

    int totalCircles = 5;
    int filledCircles = (int)ceil((needs.hunger / 100.0f) * totalCircles);

    int circleRadius = 15;
    int spacing = 10;
    int startX = 150;
    int y = 60;

    for (int i = 0; i < totalCircles; i++) {
        int x = startX + i * (circleRadius * 2 + spacing);
        Color circleColor = (i < filledCircles) ? DARKGREEN : LIGHTGRAY;
        DrawCircle(x, y, circleRadius, circleColor);
    }
}
