#include "raylib.h"
#include "NeedsSystem.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Chibichu - Tamagotchi Lite");
    SetTargetFPS(60);

    bool gameStarted = false;
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 25, 200, 50 };
    Rectangle stopButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };

    Needs needs;
    InitNeeds(needs);

    while (!WindowShouldClose()) {
        // Start button click
        if (!gameStarted &&
            CheckCollisionPointRec(GetMousePosition(), startButton) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameStarted = true;
        }

        // Stop button click (quit)
        if (!gameStarted &&
            CheckCollisionPointRec(GetMousePosition(), stopButton) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CloseWindow();
            break;
        }

        if (gameStarted) {
            UpdateNeeds(needs);
            DrawNeeds(needs);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameStarted) {
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawText("START", startButton.x + 60, startButton.y + 15, 20, BLACK);

            DrawRectangleRec(stopButton, LIGHTGRAY);
            DrawText("STOP", stopButton.x + 60, stopButton.y + 15, 20, BLACK);
        }
        else {
            DrawText("Game Started!", screenWidth / 2 - 80, screenHeight / 2 - 10, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
