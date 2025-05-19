#ifndef GAME_H
#define GAME_H

#include "Pet.h"

class Game {
public:
    Game();
    void start();
    void displayStatus();

private:
    Pet pet{};
    bool isRunning{ false };
};

#endif
