#ifndef GAME_H
#define GAME_H

#include "Pet.h"

class Game {
private:
    Pet pet;
    bool isRunning;

    // Helper methods
    void displayStatus() const;
    void displayMenu() const;
    int getUserChoice(int min, int max) const;

    // Game actions
    void feedPet();
    void playWithPet();
    void letPetSleep();
    void checkPetHealth();

public:
    Game();
    void start();
};

#endif // GAME_H