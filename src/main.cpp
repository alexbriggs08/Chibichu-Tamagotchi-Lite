#include "raylib.h"
#include "NeedsSystem.h"
#include <cstdio>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Chibichu - Tamagotchi Lite");
    SetTargetFPS(60);

    // creates all the buttons
    bool gameStarted = false;
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 25, 200, 50 };
    Rectangle stopButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };
    Rectangle feedButton = { 50, 200, 150, 40 }; // 50 = x, 200 = y, 150 = width, 40 = height
    Rectangle  cleanButton = { 50, 260, 150, 40 };

    Needs needs;
    InitNeeds(needs);

    float needsTimer = 0.0f;
    const double updateInterval = 3.5f;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        needsTimer += deltaTime;

        // Update needs only if game started and interval passed
        if (gameStarted && needsTimer >= updateInterval) {
            UpdateNeeds(needs);
            needsTimer = 0.0f;
            printf("Hunger: %d\n", needs.hunger);
        }

        // Input handling: start game
        if (!gameStarted &&
            CheckCollisionPointRec(GetMousePosition(), startButton) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            gameStarted = true;
        }

        // Input handling: stop game (close window)
        if (!gameStarted &&
            CheckCollisionPointRec(GetMousePosition(), stopButton) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CloseWindow();
            break;
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
            DrawNeeds(needs);
            DrawText("Game Started!", screenWidth / 2 - 80, screenHeight / 2 - 10, 20, DARKGRAY);
            DrawRectangleRec(feedButton, LIGHTGRAY);
            DrawText("Feed", feedButton.x + 50, feedButton.y + 10, 20, BLACK);
            DrawRectangleRec(cleanButton, LIGHTGRAY);
            DrawText("Clean", cleanButton.x + 50, cleanButton.y + 10, 20, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), feedButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                needs.hunger += 10; // increases the hunger stat
                if (needs.hunger > 100) needs.hunger = 100; // clamp to max
            }
            if (CheckCollisionPointRec(GetMousePosition(), cleanButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                needs.cleanliness += 10; // Increase cleanliness
                if (needs.cleanliness > 100) needs.cleanliness = 100; // Clamp to max
            }
            DrawText(TextFormat("Cleanliness: %d", needs.cleanliness), 50, 100, 20, DARKGRAY);

        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
