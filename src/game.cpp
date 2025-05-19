#include "Game.h"
#include <iostream>

void Game::displayStatus() {
    std::cout << "---- Pet Status ----\n";
    std::cout << "Hunger: " << pet.hunger << "\n";
    std::cout << "Happiness: " << pet.happiness << "\n";
    std::cout << "Health: " << pet.health << "\n";
    std::cout << "Age: " << pet.age << "\n";
    std::cout << "Asleep: " << (pet.asleep ? "Yes" : "No") << "\n";
    std::cout << "---------------------\n";
}

Game::Game() {
    // ...
}

void Game::start() {
    isRunning = true;
    while (isRunning) {
        displayStatus(); // OK now because it's defined above
    }
}
