#ifndef GAME_H
#define GAME_H

#include "Pet.h"  // So the Game class can use Pet

class Game {
public:
    Game();         // Constructor
    void start();   // Starts the game loop

private:
    Pet pet{};      // The pet instance
    bool isRunning{ false };  // Game loop flag
};

#endif // GAME_H
