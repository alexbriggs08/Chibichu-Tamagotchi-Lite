#include "Game.h"
#include <iostream>
#include <iomanip>
#include <limits>

// Displays the current status of the pet in a formatted box.
void Game::displayStatus() const {
    if (pet.isDead) {
        // If the pet is dead, show a Game Over message
        std::cout << "\n+---------------------+\n";
        std::cout << "|      PET DIED       |\n";
        std::cout << "|                     |\n";
        std::cout << "| Your pet has passed |\n";
        std::cout << "| away. Game Over.    |\n";
        std::cout << "+---------------------+\n\n";
        return;
    }

    // Display the pet's stats
    std::cout << "\n+---------------------+\n";
    std::cout << "|     Pet Status      |\n";
    std::cout << "+---------------------+\n";
    std::cout << "| Hunger:    " << std::setw(3) << pet.hunger << "          |\n";
    std::cout << "| Happiness: " << std::setw(3) << pet.happiness << "          |\n";
    std::cout << "| Age:       " << std::setw(3) << pet.age << "          |\n";
    std::cout << "| Asleep:    " << (pet.asleep ? "Yes" : "No ") << "          |\n";

    // Display pet's health status as a simple descriptor
    if (pet.isDying()) {
        std::cout << "| Status:    CRITICAL     |\n";
    }
    else if (pet.health < 30) {
        std::cout << "| Status:    UNWELL       |\n";
    }
    else if (pet.health < 60) {
        std::cout << "| Status:    TIRED        |\n";
    }
    else {
        std::cout << "| Status:    GOOD         |\n";
    }
    std::cout << "+---------------------+\n\n";
}

// Displays the main menu or the restart/quit menu depending on pet state
void Game::displayMenu() const {
    if (pet.isDead) {
        std::cout << "Game Over! Your pet has died.\n";
        std::cout << "1. Restart\n2. Quit\n";
        return;
    }

    std::cout << "What would you like to do?\n";
    std::cout << "1. Feed Pet\n";
    std::cout << "2. Play with Pet\n";
    std::cout << "3. Let Pet Sleep\n";
    std::cout << "4. Check Pet Health\n";
    std::cout << "5. Quit\n";
}

// Prompts and validates user input for a menu choice
int Game::getUserChoice(int min, int max) const {
    int choice{};
    while (true) {
        std::cout << "Enter choice (" << min << "-" << max << "): ";
        if (std::cin >> choice && choice >= min && choice <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        // Handle invalid input
        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Determines if pet has died based on health or hunger
void Game::checkForDeath() {
    if (pet.health <= 0 || pet.hunger <= 0) {

        pet.isDead = true;
    }
}

// Advances game time, modifying pet stats accordingly
void Game::passTime() {
    if (pet.isDead) return;

    // Degrade stats over time based on sleep state
    if (pet.asleep) {
        pet.hunger = std::min(100, pet.hunger - 2);
        pet.happiness = std::max(0, pet.happiness - 1);
        pet.health = std::max(0, pet.health - 1);
    }
    else {
        pet.hunger = std::min(100, pet.hunger - 5);
        pet.happiness = std::max(0, pet.happiness - 2);
        pet.health = std::max(0, pet.health - 1);
    }

    // Apply penalties if hunger or happiness reaches poor thresholds
    if (pet.hunger >= 90 || pet.happiness <= 10) {
        pet.health = std::max(0, pet.health - 5);
    }
    else if (pet.hunger >= 70 || pet.happiness <= 30) {
        pet.health = std::max(0, pet.health - 2);
    }

    pet.age++;  // Increment pet age every cycle
    checkForDeath();  // Check if conditions have killed the pet
}

// Constructor initializes game state
Game::Game() : isRunning(false) {}

// Resets pet to initial conditions
void Game::restart() {
    pet.hunger = 50;
    pet.happiness = 50;
    pet.health = 100;
    pet.age = 0;
    pet.asleep = false;
    pet.isDead = false;
    std::cout << "A new pet has been born! Take good care of them!\n";
}

// Starts the main game loop
void Game::start() {
    std::cout << "Welcome to Virtual Pet Game!\n";
    isRunning = true;

    while (isRunning) {
        displayStatus();

        if (pet.isDead) {
            displayMenu();
            int choice = getUserChoice(1, 2);
            if (choice == 1) {
                restart();
            }
            else {
                isRunning = false;
                std::cout << "Thanks for playing!\n";
            }
        }
        else {
            displayMenu();
            int choice = getUserChoice(1, 5);

            // Execute action based on user choice
            switch (choice) {
            case 1: feedPet(); break;
            case 2: playWithPet(); break;
            case 3: letPetSleep(); break;
            case 4: checkPetHealth(); break;
            case 5:
                isRunning = false;
                std::cout << "Thanks for playing! Your pet will miss you!\n";
                break;
            default:
                std::cout << "Unexpected error occurred.\n";
                break;
            }

            // Let time pass after user action
            if (isRunning && !pet.isDead) {
                passTime();
            }
        }

        // Wait for user before clearing screen for next loop
        if (isRunning) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

// Feeds the pet and adjusts relevant stats
void Game::feedPet() {
    if (pet.asleep) {
        std::cout << "Your pet is sleeping! You can't feed them right now.\n";
        return;
    }
    if (pet.hunger <= 10) {
        std::cout << "Your pet is already full.\n";
        return;
    }

    // Adjust stats positively after feeding
    pet.hunger = std::max(0, pet.hunger + 30);
    pet.happiness = std::min(100, pet.happiness + 15);
    pet.health = std::min(100, pet.health + 5);

    if (pet.hunger == 100) {
        std::cout << "You fed your pet! They are fully satisfied now.\n";
    }
    else {
        std::cout << "You fed your pet! They seem happier and less hungry.\n";
    }
}

// Lets the user play with the pet if conditions allow
void Game::playWithPet() {
    if (pet.asleep) {
        std::cout << "Your pet is sleeping.\n";
        return;
    }
    if (pet.health <= 20) {
        std::cout << "Your pet is too sick to play. They need rest.\n";
        return;
    }
    if (pet.hunger >= 10) {
        std::cout << "Your pet is too hungry to play. Feed them first.\n";
        return;
    }

    // Playing increases happiness but drains hunger and health
    pet.happiness = std::min(100, pet.happiness + 25);
    pet.hunger = std::min(100, pet.hunger + 10);
    pet.health = std::max(0, pet.health - 3);

    std::cout << "You played with your pet! They had fun.\n";
}

// Lets the pet sleep and recovers some health/happiness
void Game::letPetSleep() {
    if (pet.asleep) {
        std::cout << "Your pet is already sleeping.\n";
        return;
    }

    pet.asleep = true;
    pet.health = std::min(100, pet.health + 20);
    pet.happiness = std::min(100, pet.happiness + 10);

    std::cout << "Your pet is now sleeping.\n";

    // Sleep simulation is instantaneous for now
    pet.asleep = false;
    std::cout << "Your pet woke up feeling refreshed.\n";
}

// Gives more detailed info about pet's health
void Game::checkPetHealth() {
    std::cout << "Checking your pet's health...\n";

    // Health status report
    if (pet.health >= 80) {
        std::cout << "Your pet is in excellent health.\n";
    }
    else if (pet.health >= 60) {
        std::cout << "Your pet is doing well but could use some rest.\n";
    }
    else if (pet.health >= 40) {
        std::cout << "Your pet looks a bit tired.\n";
    }
    else if (pet.health >= 20) {
        std::cout << "Your pet doesn't look well and needs care.\n";
    }
    else if (pet.health > 0) {
        std::cout << "Your pet is very sick and needs immediate attention.\n";
    }

    // Hunger insights
    if (pet.hunger >= 90) {
        std::cout << "Your pet looks extremely hungry.\n";
    }
    else if (pet.hunger >= 70) {
        std::cout << "Your pet seems quite hungry.\n";
    }
    else if (pet.hunger <= 20) {
        std::cout << "Your pet's belly looks full.\n";
    }

    // Happiness summary
    if (pet.happiness >= 80) {
        std::cout << "Your pet looks very happy.\n";
    }
    else if (pet.happiness <= 30) {
        std::cout << "Your pet looks sad and needs attention.\n";
    }
}
