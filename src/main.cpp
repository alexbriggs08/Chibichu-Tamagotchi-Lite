#include "raylib.h"
#include "NeedsSystem.h"
#include <cstdio>

// function calling
const char* getFormattedTime(float timeOfDay, float fullDayDuration);
int getGameHour(float timeOfDay, float fullDayDuration);

int main() {
    // sets the screen dimensions to 800x600
    const int screenWidth = 800;
    const int screenHeight = 600;

    // creates the window and sets the fps to 60
    InitWindow(screenWidth, screenHeight, "Chibichu - Tamagotchi Lite");
    SetTargetFPS(60);

    // creates all the buttons
    bool gameStarted = false;
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 25, 200, 50 };
    Rectangle stopButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };
    Rectangle feedButton = { 240, 520, 150, 40 }; // 50 = x, 200 = y, 150 = width, 40 = height
    Rectangle  cleanButton = { 410, 520, 150, 40 };

    // intilizes the pets needs
    Needs needs;
    InitNeeds(needs);

    // creates the timer for the stats so they go down on time and not fps
    float needsTimer = 0.0f;
    const double updateInterval = 3.5f;
    // stats the time of day
    float totalElapsedTime = 0.0f;
    const float fullDayDuration = 120.0f; // sets the full day duration (in game) to 2 minutes

    // Game Loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        needsTimer += deltaTime;

        // creates the time of day
        totalElapsedTime += deltaTime;


        int currentDay = static_cast<int>(totalElapsedTime / fullDayDuration) + 1;

        float timeOfDay = 0.0f;

        timeOfDay += deltaTime;
        if (timeOfDay > fullDayDuration) timeOfDay = 0.0f;

        int currentHour = getGameHour(timeOfDay, fullDayDuration);

        needs.isSleeping = (currentHour >= 22 || currentHour < 6);



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

        // will draw the window with a BG color of white
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // will draw the start and stop button when the game hasn't started 
        if (!gameStarted) {
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawText("START", startButton.x + 60, startButton.y + 15, 20, BLACK);

            DrawRectangleRec(stopButton, LIGHTGRAY);
            DrawText("STOP", stopButton.x + 60, stopButton.y + 15, 20, BLACK);
        }
        else {
            // Information: 
            DrawNeeds(needs); // will draw the pets needs
            DrawText(needs.isSleeping ? "Zzz..." : "Awake", 50, 140, 20, needs.isSleeping ? DARKBLUE : DARKGREEN); // will draw the pets sleeping stat
            DrawText(TextFormat("Day %d", currentDay), screenWidth - 100, 20, 20, DARKGRAY); // will draw the days 
            DrawText("Game Started!", screenWidth / 2 - 80, screenHeight / 2 - 10, 20, DARKGRAY); // placeholder text until we get the pet and its animations
            DrawRectangleRec(feedButton, LIGHTGRAY); // will draw the feed button < v
            DrawText("Feed", feedButton.x + 50, feedButton.y + 10, 20, BLACK);
            DrawRectangleRec(cleanButton, LIGHTGRAY); // will draw the clean button < v
            DrawText("Clean", cleanButton.x + 50, cleanButton.y + 10, 20, BLACK);
            DrawText(TextFormat("Cleanliness: %d", needs.cleanliness), 50, 100, 20, DARKGRAY); // displays the clenliness stat


            // Buttons:
            if (CheckCollisionPointRec(GetMousePosition(), feedButton) && // handles the feed button input
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                needs.hunger += 10; // increases the hunger stat
                if (needs.hunger > 100) needs.hunger = 100; // clamp to max
            }
            if (CheckCollisionPointRec(GetMousePosition(), cleanButton) && // handles the clean button input
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                needs.cleanliness += 10; // Increase cleanliness
                if (needs.cleanliness > 100) needs.cleanliness = 100; // Clamp to max
            }

        }
        // ends the drawing of the screen
        EndDrawing();
    }
    // closes the window
    CloseWindow();

    return 0;
}


// Gets the current in game hour from 0 - 23 
int getGameHour(float timeOfDay, float fullDayDuration) {
    int totalMinutes = static_cast<int>((timeOfDay / fullDayDuration) * 24 * 60);
    return totalMinutes / 60;
}

// converts in-game time to a formated string of HH::MM
const char* getFormattedTime(float timeOfDay, float fullDayDuration) {
    int totalMinutes = static_cast<int>((timeOfDay / fullDayDuration) * 24 * 60);
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    static char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return buffer;
}

