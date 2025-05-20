#include "raylib.h"
#include "NeedsSystem.h"
#include "Animations.h"
#include <cstdio>
#include <cmath>

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
    Rectangle cleanButton = { 410, 520, 150, 40 };

    // intilizes the pets needs
    Needs needs;
    InitNeeds(needs);

    // creates the timer for the stats so they go down on time and not fps
    float needsTimer = 0.0f;
    const double updateInterval = 3.5f;
    // stats the time of day
    float totalElapsedTime = 0.0f;
    const float fullDayDuration = 120.0f; // sets the full day duration (in game) to 2 minutes

    // for animating pet actions
    PetState currentState = IDLE;
    float stateTimer = 0.0f;
    const float actionDuration = 1.5f;

    // Animation frame control
    float frameTimer = 0.0f;
    const float frameInterval = 0.5f; // 2 frames per second
    int currentFrame = 0;

    // Game Loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        needsTimer += deltaTime;
        totalElapsedTime += deltaTime;

        // Handle animation frame updates
        frameTimer += deltaTime;
        if (frameTimer >= frameInterval) {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 2; // Toggle between 0 and 1
        }

        // Handle action state duration
        if (currentState == EATING || currentState == CLEANING) {
            stateTimer += deltaTime;
            if (stateTimer >= actionDuration) {
                // Return to IDLE when action is complete
                currentState = IDLE;
                stateTimer = 0.0f;
                printf("Action complete, returning to IDLE\n");
            }
        }

        // time of day logic
        int currentDay = static_cast<int>(totalElapsedTime / fullDayDuration) + 1;
        float timeOfDay = fmod(totalElapsedTime, fullDayDuration);
        int currentHour = getGameHour(timeOfDay, fullDayDuration);

        // Handle sleeping state
        bool shouldBeSleeping = (currentHour >= 22 || currentHour < 6);
        needs.isSleeping = shouldBeSleeping;

        // Only change to SLEEPING if we're not in the middle of an action
        if (shouldBeSleeping && currentState != EATING && currentState != CLEANING) {
            if (currentState != SLEEPING) {
                printf("Going to sleep\n");
                currentState = SLEEPING;
            }
        }
        else if (!shouldBeSleeping && currentState == SLEEPING) {
            // Wake up if it's daytime and we were sleeping
            printf("Waking up\n");
            currentState = IDLE;
        }

        // Get the correct animation frame
        const char* frameStr = getPetFrame(currentState, currentFrame);

        // Update needs only if game started and interval passed
        if (gameStarted && needsTimer >= updateInterval) {
            UpdateNeeds(needs);
            needsTimer = 0.0f;
            printf("Hunger: %d, Cleanliness: %d\n", needs.hunger, needs.cleanliness);
        }

        // Input handling
        if (!gameStarted) {
            // Start game button
            if (CheckCollisionPointRec(GetMousePosition(), startButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                gameStarted = true;
                printf("Game started!\n");
            }

            // Stop game button
            if (CheckCollisionPointRec(GetMousePosition(), stopButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                CloseWindow();
                break;
            }
        }
        else {
            // Only handle feeding/cleaning if pet is awake and not already in an action
            if (!needs.isSleeping) {
                // Feed button
                if (CheckCollisionPointRec(GetMousePosition(), feedButton) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (currentState != EATING && currentState != CLEANING) {
                        needs.hunger += 10;
                        if (needs.hunger > 100) needs.hunger = 100;
                        currentState = EATING;
                        stateTimer = 0.0f;
                        printf("Feeding pet, hunger: %d\n", needs.hunger);
                    }
                }

                // Clean button
                if (CheckCollisionPointRec(GetMousePosition(), cleanButton) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (currentState != EATING && currentState != CLEANING) {
                        needs.cleanliness += 10;
                        if (needs.cleanliness > 100) needs.cleanliness = 100;
                        currentState = CLEANING;
                        stateTimer = 0.0f;
                        printf("Cleaning pet, cleanliness: %d\n", needs.cleanliness);
                    }
                }
            }
        }

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameStarted) {
            // Draw start/stop buttons
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawText("START", startButton.x + 60, startButton.y + 15, 20, BLACK);

            DrawRectangleRec(stopButton, LIGHTGRAY);
            DrawText("STOP", stopButton.x + 60, stopButton.y + 15, 20, BLACK);
        }
        else {
            // Draw pet animation
            DrawText(frameStr, 350, 200, 20, BLACK);  // Centered the pet display

            // Draw UI elements
            DrawNeeds(needs);
            DrawText(needs.isSleeping ? "Zzz..." : "Awake", 50, 140, 20, needs.isSleeping ? DARKBLUE : DARKGREEN);
            DrawText(TextFormat("Day %d", currentDay), screenWidth - 100, 20, 20, DARKGRAY);

            // Debug information
            DrawText(TextFormat("State: %d (0=IDLE, 1=EATING, 2=CLEANING, 3=SLEEPING)", currentState), 50, 170, 16, DARKGRAY);
            DrawText(TextFormat("Frame: %d", currentFrame), 50, 190, 16, DARKGRAY);
            DrawText(TextFormat("Hour: %d", currentHour), 50, 210, 16, DARKGRAY);

            // Draw cleanliness stat
            DrawText(TextFormat("Cleanliness: %d", needs.cleanliness), 50, 100, 20, DARKGRAY);

            // Draw buttons
            Color feedButtonColor = (needs.isSleeping || currentState == EATING || currentState == CLEANING) ? GRAY : LIGHTGRAY;
            Color cleanButtonColor = (needs.isSleeping || currentState == EATING || currentState == CLEANING) ? GRAY : LIGHTGRAY;

            DrawRectangleRec(feedButton, feedButtonColor);
            DrawText("Feed", feedButton.x + 50, feedButton.y + 10, 20, BLACK);

            DrawRectangleRec(cleanButton, cleanButtonColor);
            DrawText("Clean", cleanButton.x + 50, cleanButton.y + 10, 20, BLACK);
        }

        EndDrawing();
    }

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