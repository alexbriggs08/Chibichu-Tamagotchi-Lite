#include "raylib.h"
#include "NeedsSystem.h"
#include "Animations.h"
#include <cstdio>
#include <cmath>
#include <string>

// Function declarations
const char* getFormattedTime(float timeOfDay, float fullDayDuration);
int getGameHour(float timeOfDay, float fullDayDuration);
void DrawPetEnvironment(int hour, const Needs& needs);

int main() {
    // Sets the screen dimensions to 800x600
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Creates the window and sets the fps to 60
    InitWindow(screenWidth, screenHeight, "Chibichu - Advanced Tamagotchi");
    SetTargetFPS(60);

    // Create all the buttons
    bool gameStarted = false;
    bool isPaused = false;
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 25, 200, 50 };
    Rectangle stopButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };
    Rectangle feedButton = { 120, 520, 120, 40 };
    Rectangle cleanButton = { 260, 520, 120, 40 };
    Rectangle playButton = { 400, 520, 120, 40 };
    Rectangle sleepButton = { 540, 520, 120, 40 };
    Rectangle pauseButton = { screenWidth - 50, 20, 30, 30 };

    // Initialize the pet's needs
    Needs needs;
    InitNeeds(needs);

    // Create timers for game mechanics
    float needsTimer = 0.0f;
    const float needsUpdateInterval = 0.5f; // Update needs more frequently for smoother gameplay

    // Game time tracking
    float totalElapsedTime = 0.0f;
    const float fullDayDuration = 240.0f; // Extend day to 4 minutes for more immersion

    // For animating pet actions
    PetState currentState = IDLE;
    PetState previousState = IDLE;
    float stateTimer = 0.0f;
    const float actionDuration = 2.0f;

    // For minigame (simple ball catching)
    bool miniGameActive = false;
    Vector2 ballPosition = { -100, -100 };
    Vector2 ballSpeed = { 0, 0 };
    Rectangle petHitbox = { 0, 0, 60, 60 };
    int gameScore = 0;
    float miniGameTimer = 0;
    const float miniGameDuration = 15.0f;

    // Weather effects
    bool isRaining = false;
    float rainTimer = 0;
    float rainChance = 0.001f; // Chance per frame for rain to start
    float rainDuration = 0;

    // Pet states
    std::string petName = "Chibichu";
    int petAge = 1; // Age in days

    // Game Loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Don't update timers if paused
        if (!isPaused && gameStarted) {
            needsTimer += deltaTime;
            totalElapsedTime += deltaTime;
            stateTimer += deltaTime;

            // Update rain timer
            if (isRaining) {
                rainTimer += deltaTime;
                if (rainTimer >= rainDuration) {
                    isRaining = false;
                    rainTimer = 0;
                }
            }
            else {
                // Small chance of rain starting
                if (GetRandomValue(0, 1000) < rainChance * 1000) {
                    isRaining = true;
                    rainDuration = GetRandomValue(10, 30);
                    rainTimer = 0;
                }
            }
        }

        int frame = static_cast<int>(totalElapsedTime * 2) % 2; // 2 frames per second animation

        // Time of day logic
        int currentDay = static_cast<int>(totalElapsedTime / fullDayDuration) + 1;
        float timeOfDay = fmod(totalElapsedTime, fullDayDuration);
        int currentHour = getGameHour(timeOfDay, fullDayDuration);

        // Update pet age
        petAge = currentDay;

        // Update sleeping state based on time of day
        bool wasSleeping = needs.isSleeping;
        needs.isSleeping = (currentHour >= 22 || currentHour < 6);

        // Update pet state based on needs
        previousState = currentState;

        if (needs.isSleeping) {
            currentState = SLEEPING;
        }
        else if (currentState != IDLE && stateTimer >= actionDuration) {
            // Return to mood-based state after action
            if (needs.moodState == 0) {
                currentState = SAD;
            }
            else if (needs.moodState == 2) {
                currentState = HAPPY;
            }
            else {
                currentState = IDLE;
            }
            stateTimer = 0.0f;
        }

        // Handle minigame logic
        if (miniGameActive) {
            miniGameTimer += deltaTime;

            // Move the ball
            ballPosition.x += ballSpeed.x * deltaTime;
            ballPosition.y += ballSpeed.y * deltaTime;

            // Bounce off walls
            if (ballPosition.x < 0 || ballPosition.x > screenWidth) {
                ballSpeed.x *= -1;
            }
            if (ballPosition.y < 0 || ballPosition.y > screenHeight) {
                ballSpeed.y *= -1;
            }

            // Set pet hitbox position
            petHitbox.x = 330;
            petHitbox.y = 300;

            // Check for collision with pet
            if (CheckCollisionCircleRec(ballPosition, 15, petHitbox)) {
                gameScore++;

                // Reposition ball
                ballPosition.x = GetRandomValue(50, screenWidth - 50);
                ballPosition.y = GetRandomValue(50, 200);
                ballSpeed.x = GetRandomValue(-200, 200);
                ballSpeed.y = GetRandomValue(100, 300);

                // Increase happiness
                needs.happiness += 5;
                if (needs.happiness > 100) needs.happiness = 100;
            }

            // End minigame after time expires
            if (miniGameTimer >= miniGameDuration) {
                miniGameActive = false;
                currentState = HAPPY;
                stateTimer = 0.0f;

                // Update happiness based on score
                needs.happiness += gameScore * 2;
                if (needs.happiness > 100) needs.happiness = 100;
            }
        }

        // Update needs if game started and interval passed
        if (gameStarted && !isPaused && needsTimer >= needsUpdateInterval) {
            UpdateNeeds(needs, needsUpdateInterval);
            needsTimer = 0.0f;
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

        // Input handling: pause game
        if (gameStarted &&
            CheckCollisionPointRec(GetMousePosition(), pauseButton) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isPaused = !isPaused;
        }

        // Draw everything
        BeginDrawing();

        // Set background color based on time of day
        ClearBackground(getBackgroundColor(currentHour));

        // Draw start screen if not started
        if (!gameStarted) {
            // Draw title
            DrawText("Chibichu - Advanced Tamagotchi", screenWidth / 2 - 200, screenHeight / 2 - 100, 24, WHITE);

            // Draw start and stop buttons
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawText("START", startButton.x + 60, startButton.y + 15, 20, BLACK);

            DrawRectangleRec(stopButton, LIGHTGRAY);
            DrawText("STOP", stopButton.x + 60, stopButton.y + 15, 20, BLACK);

            // Draw sample pet
            DrawText(getPetFrame(IDLE, frame), 340, 150, 20, BLACK);
        }
        else {
            // Draw environment based on time of day
            DrawPetEnvironment(currentHour, needs);

            // Draw rain if it's raining
            if (isRaining) {
                for (int i = 0; i < 100; i++) {
                    DrawLine(
                        GetRandomValue(0, screenWidth),
                        GetRandomValue(0, screenHeight / 2),
                        GetRandomValue(0, screenWidth),
                        GetRandomValue(screenHeight / 2, screenHeight),
                        (Color) {
                        200, 200, 255, 100
                    }
                    );
                }
            }

            // Draw pet based on current state
            DrawText(getPetFrame(currentState, frame), 320, 250, 20, BLACK);

            // Draw UI elements
            DrawRectangle(0, 0, screenWidth, 40, (Color) { 0, 0, 0, 150 });
            DrawText(TextFormat("Day %d", currentDay), 20, 10, 20, WHITE);
            DrawText(TextFormat("Time: %s (%s)", getFormattedTime(timeOfDay, fullDayDuration), getTimeDescription(currentHour)),
                screenWidth / 2 - 100, 10, 20, WHITE);
            DrawText(TextFormat("%s (Age: %d)", petName.c_str(), petAge), screenWidth - 200, 10, 20, WHITE);

            // Draw pause button
            DrawRectangleRec(pauseButton, isPaused ? RED : DARKGRAY);
            DrawRectangle(pauseButton.x + 7, pauseButton.y + 7, 6, 16, WHITE);
            DrawRectangle(pauseButton.x + 17, pauseButton.y + 7, 6, 16, WHITE);

            // Draw needs meters
            DrawNeeds(needs);

            // Draw status indicator
            DrawText("Status:", 50, 220, 20, BLACK);
            DrawText(GetMoodText(needs), 150, 220, 20, GetMoodColor(needs));

            // If minigame is active, draw the ball and score
            if (miniGameActive) {
                DrawCircle(ballPosition.x, ballPosition.y, 15, RED);
                DrawText(TextFormat("Score: %d", gameScore), screenWidth / 2 - 50, 50, 30, BLACK);
                DrawText(TextFormat("Time: %d", (int)(miniGameDuration - miniGameTimer)), screenWidth / 2 - 50, 80, 20, BLACK);
                DrawText("Catch the ball!", screenWidth / 2 - 80, 110, 20, DARKGRAY);
            }

            // Pause overlay
            if (isPaused) {
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color) { 0, 0, 0, 150 });
                DrawText("PAUSED", screenWidth / 2 - 70, screenHeight / 2 - 15, 40, WHITE);
                DrawText("Click pause button to resume", screenWidth / 2 - 150, screenHeight / 2 + 40, 20, WHITE);
            }

            // Draw buttons
            Color buttonColor = LIGHTGRAY;
            Color buttonColorDisabled = (Color){ 180, 180, 180, 100 };

            // Feed button
            DrawRectangleRec(feedButton, !needs.isSleeping ? buttonColor : buttonColorDisabled);
            DrawText("Feed", feedButton.x + 40, feedButton.y + 10, 20, BLACK);

            // Clean button
            DrawRectangleRec(cleanButton, !needs.isSleeping ? buttonColor : buttonColorDisabled);
            DrawText("Clean", cleanButton.x + 32, cleanButton.y + 10, 20, BLACK);

            // Play button
            DrawRectangleRec(playButton, !needs.isSleeping ? buttonColor : buttonColorDisabled);
            DrawText("Play", playButton.x + 40, playButton.y + 10, 20, BLACK);

            // Sleep button
            DrawRectangleRec(sleepButton, !needs.isSleeping ? buttonColor : buttonColorDisabled);
            DrawText(needs.isSleeping ? "Wake" : "Sleep", sleepButton.x + 34, sleepButton.y + 10, 20, BLACK);

            // Handle button clicks when game is active and not paused
            if (gameStarted && !isPaused && !needs.isSleeping) {
                // Feed button
                if (CheckCollisionPointRec(GetMousePosition(), feedButton) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    needs.hunger += 20;
                    if (needs.hunger > 100) needs.hunger = 100;

                    needs.energy -= 5; // Eating consumes energy
                    if (needs.energy < 0) needs.energy = 0;

                    currentState = EATING;
                    stateTimer = 0.0f;
                }

                // Clean button
                if (CheckCollisionPointRec(GetMousePosition(), cleanButton) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    needs.cleanliness += 20;
                    if (needs.cleanliness > 100) needs.cleanliness = 100;

                    needs.energy -= 5; // Cleaning consumes energy
                    if (needs.energy < 0) needs.energy = 0;

                    currentState = CLEANING;
                    stateTimer = 0.0f;
                }

                // Play button (start minigame)
                if (CheckCollisionPointRec(GetMousePosition(), playButton) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                    !miniGameActive) {

                    miniGameActive = true;
                    miniGameTimer = 0;
                    gameScore = 0;

                    // Initialize ball position and speed
                    ballPosition.x = GetRandomValue(50, screenWidth - 50);
                    ballPosition.y = GetRandomValue(50, 200);
                    ballSpeed.x = GetRandomValue(-200, 200);
                    ballSpeed.y = GetRandomValue(100, 300);

                    currentState = PLAYING;
                    stateTimer = 0.0f;

                    needs.energy -= 10; // Playing consumes more energy
                    if (needs.energy < 0) needs.energy = 0;
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Get formatted time string (HH:MM)
const char* getFormattedTime(float timeOfDay, float fullDayDuration) {
    static char timeStr[6]; // "HH:MM\0"
    int hour = getGameHour(timeOfDay, fullDayDuration);
    int minute = static_cast<int>((timeOfDay / fullDayDuration) * 24 * 60) % 60;
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hour, minute);
    return timeStr;
}

// Get current game hour (0-23)
int getGameHour(float timeOfDay, float fullDayDuration) {
    return static_cast<int>((timeOfDay / fullDayDuration) * 24);
}

// Draw the pet's environment based on time of day
void DrawPetEnvironment(int hour, const Needs& needs) {
    // Draw ground
    DrawRectangle(0, 400, 800, 200, DARKGREEN);

    // Draw sun/moon based on time
    if (hour >= 6 && hour < 18) {
        // Sun
        DrawCircle(400, 100, 50, YELLOW);
        // Sun rays
        for (int i = 0; i < 8; i++) {
            float angle = i * 45 * DEG2RAD;
            DrawLine(
                400 + cos(angle) * 50,
                100 + sin(angle) * 50,
                400 + cos(angle) * 70,
                100 + sin(angle) * 70,
                YELLOW
            );
        }
    }
    else {
        // Moon
        DrawCircle(400, 100, 40, LIGHTGRAY);
        // Moon craters
        DrawCircle(380, 90, 8, DARKGRAY);
        DrawCircle(420, 110, 6, DARKGRAY);
        DrawCircle(400, 120, 5, DARKGRAY);
    }

    // Draw stars at night
    if (hour >= 22 || hour < 5) {
        for (int i = 0; i < 50; i++) {
            int x = GetRandomValue(0, 800);
            int y = GetRandomValue(0, 200);
            DrawCircle(x, y, 1, WHITE);
        }
    }

    // Draw clouds
    DrawCircle(200, 150, 30, WHITE);
    DrawCircle(230, 150, 25, WHITE);
    DrawCircle(260, 150, 30, WHITE);

    DrawCircle(600, 200, 30, WHITE);
    DrawCircle(630, 200, 25, WHITE);
    DrawCircle(660, 200, 30, WHITE);
}