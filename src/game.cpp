#include "Game.h"
#include <iostream>
#include <iomanip>
#include <limits>

void Game::displayStatus() const {
    std::cout << "\n┌─────────────────────┐\n";
    std::cout << "│     Pet Status      │\n";
    std::cout << "├─────────────────────┤\n";
    std::cout << "│ Hunger:    " << std::setw(8) << pet.hunger << " │\n";
    std::cout << "│ Happiness: " << std::setw(8) << pet.happiness << " │\n";
    std::cout << "│ Health:    " << std::setw(8) << pet.health << " │\n";
    std::cout << "│ Age:       " << std::setw(8) << pet.age << " │\n";
    std::cout << "│ Asleep:    " << std::setw(8) << (pet.asleep ? "Yes" : "No") << " │\n";
    std::cout << "└─────────────────────┘\n\n";
}

void Game::displayMenu() const {
    std::cout << "What would you like to do?\n";
    std::cout << "1. Feed Pet\n";
    std::cout << "2. Play with Pet\n";
    std::cout << "3. Let Pet Sleep\n";
    std::cout << "4. Check Pet Health\n";
    std::cout << "5. Quit\n";
}

int Game::getUserChoice(int min, int max) const {
    int choice{};
    while (true) {
        std::cout << "Enter choice (" << min << "-" << max << "): ";

        if (std::cin >> choice) {
            if (choice >= min && choice <= max) {
                // Clear any remaining input
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            }
            std::cout << "Please enter a number between " << min << " and " << max << ".\n";
        }
        else {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        }
    }
}

Game::Game() : isRunning{ false } {
    // pet will be initialized with default values automatically
}

void Game::start() {
    std::cout << "Welcome to Virtual Pet Game!\n";
    isRunning = true;

    while (isRunning) {
        displayStatus();
        displayMenu();

        int choice{ getUserChoice(1, 5) };

        switch (choice) {
        case 1:
            feedPet();
            break;
        case 2:
            playWithPet();
            break;
        case 3:
            letPetSleep();
            break;
        case 4:
            checkPetHealth();
            break;
        case 5:
            isRunning = false;
            std::cout << "\nThanks for playing! Your pet will miss you!\n";
            break;
        default:
            // This should never happen due to input validation
            std::cout << "Unexpected error occurred.\n";
            break;
        }

        // Add a small pause for better user experience
        if (isRunning) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void Game::feedPet() {
    if (pet.asleep) {
        std::cout << "Your pet is sleeping! You can't feed them right now.\n";
        return;
    }

    if (pet.hunger <= 0) {
        std::cout << "Your pet isn't hungry right now!\n";
        return;
    }

    pet.hunger = std::max(0, pet.hunger - 20);
    pet.happiness = std::min(100, pet.happiness + 10);
    std::cout << "You fed your pet! They look happier now.\n";
}

void Game::playWithPet() {
    if (pet.asleep) {
        std::cout << "Shh! Your pet is sleeping peacefully.\n";
        return;
    }

    if (pet.health <= 20) {
        std::cout << "Your pet is too sick to play. Take care of their health first!\n";
        return;
    }

    pet.happiness = std::min(100, pet.happiness + 25);
    pet.hunger = std::min(100, pet.hunger + 10);
    pet.health = std::max(0, pet.health - 5);
    std::cout << "You played with your pet! They had so much fun!\n";
}

void Game::letPetSleep() {
    if (pet.asleep) {
        std::cout << "Your pet is already sleeping soundly.\n";
        return;
    }

    pet.asleep = true;
    pet.health = std::min(100, pet.health + 15);
    pet.happiness = std::min(100, pet.happiness + 5);
    std::cout << "Your pet curls up and falls asleep. Sweet dreams!\n";

    // Wake up after some time (simulate)
    std::cout << "After a good rest, your pet wakes up refreshed!\n";
    pet.asleep = false;
}

void Game::checkPetHealth() {
    std::cout << "You examine your pet carefully...\n";

    if (pet.health >= 80) {
        std::cout << "Your pet is in excellent health!\n";
    }
    else if (pet.health >= 60) {
        std::cout << "Your pet is doing well, but could use some rest.\n";
    }
    else if (pet.health >= 40) {
        std::cout << "Your pet looks a bit tired. Consider letting them sleep.\n";
    }
    else if (pet.health >= 20) {
        std::cout << "Your pet doesn't look well. They need rest and care!\n";
    }
    else {
        std::cout << "Your pet is very sick! They need immediate attention!\n";
    }
}