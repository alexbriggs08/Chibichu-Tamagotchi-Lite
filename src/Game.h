#ifndef GAME_H
#define GAME_H

#include "Pet.h"

#include <iostream>

class Game {
public:
    Game();
    void start();
    void displayStatus();
    void feedPet() {
        pet.hunger -= 10;
        if (pet.hunger < 0) {
            pet.hunger = 0;
        }

        pet.happiness += 5;
        if (pet.happiness > 100) {
            pet.happiness = 100;
        }

        std::cout << "You fed your pet. Hunger decreased, happiness increased!\n";
    };
    void playWithPet();
    void putToSleep();


private:
    Pet pet{};
    bool isRunning{ false };
};

#endif
