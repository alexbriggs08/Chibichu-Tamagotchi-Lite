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

int getUserChoice(int min, int max);

Game::Game() {
    // ...
}

void Game::start() {
    isRunning = true;
    while (isRunning) {
        displayStatus();

        std::cout << "What would you like to do?\n";
        std::cout << "1. Feed Pet\n";
        std::cout << "2. Quit\n";

        int choice = getUserChoice(1, 2);

        switch (choice) {
        case 1:
            feedPet();
            break;
        case 2:
            isRunning = false;
            std::cout << "Thanks for playing!\n";
            break;
        }
    }
}



int getUserChoice(int min, int max) {
    int choice;
    while (true) {
        std::cout << "Enter choice (" << min << "-" << max << "): ";
        if (std::cin >> choice) {
            if (choice >= min && choice <= max) {
                return choice;
            }
            else {
                std::cout << "Please enter a number between " << min << " and " << max << ".\n";
            }
        }
        else {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        }
    }
}
