#include <iostream>     // For input/output operations
#include <string>      // For string handling
#include <thread>      // For multi-threading support
#include <atomic>      // For thread-safe atomic operations
#include <chrono>      // For time-related functions
#include <vector>      // For dynamic arrays
#include <mutex>       // For mutual exclusion (thread synchronization)
#include <condition_variable> // For thread synchronization
#include <unistd.h>    // POSIX API for system calls
#include <termios.h>   // Terminal I/O interfaces
#include <fcntl.h>     // File control options

// Function declarations
void clearConsole();       // Function to clear the terminal screen
void setNonBlocking(bool enable); // Function to set terminal input to non-blocking mode
void inputThread();        // Function to handle user input in a separate thread

// Global variables for input handling
std::atomic<bool> running(true);   // Controls the main game loop
std::string inputBuffer;           // Stores user input
std::mutex inputMutex;             // Mutex for thread-safe access to inputBuffer
std::condition_variable inputCV;   // Condition variable for signaling between threads

/**
 * Pet class representing the virtual pet with its attributes and behaviors
 */
class Pet {
public:
    std::string petName;  // Name of the pet
    int hunger = 100;     // Hunger level (0-100, higher is better/more full)
    int happiness = 100;  // Happiness level (0-100, higher is better)
    int energy = 100;     // Energy level (0-100, higher is better)

    /**
     * Feed the pet to increase hunger, happiness, and energy
     */
    void feed() {
        hunger = std::min(hunger + 10, 100);     // Increase hunger, max 100
        happiness = std::min(happiness + 5, 100); // Slight happiness boost
        energy = std::min(energy + 5, 100);       // Slight energy boost
    }

    /**
     * Play with the pet to increase happiness but decrease hunger and energy
     */
    void play() {
        hunger = std::max(hunger - 10, 0);       // Decrease hunger
        happiness = std::min(happiness + 10, 100); // Increase happiness
        energy = std::max(energy - 10, 0);       // Decrease energy
    }

    /**
     * Let the pet sleep to restore energy but slightly decrease hunger and happiness
     */
    void sleep() {
        hunger = std::max(hunger - 5, 0);        // Slight hunger decrease
        happiness = std::max(happiness - 5, 0);   // Slight happiness decrease
        energy = std::min(energy + 10, 100);      // Increase energy
    }

    /**
     * Update pet stats over time (natural decay)
     * Called periodically to simulate the pet's needs changing
     */
    void update() {
        hunger = std::max(hunger - 1, 0);        // Gradually get hungry
        happiness = std::max(happiness - 1, 0);   // Gradually get bored
        energy = std::max(energy - 1, 0);         // Gradually get tired
    }
};

int main() {
    Pet myPet;  // Create a pet instance

    // Get the pet's name from the user
    std::cout << "Name your Chibichu: ";
    std::getline(std::cin, myPet.petName);

    // Define ASCII art frames for the pet's animation
    // Different frames represent different states/moods
    std::vector<std::string> frames = {
        R"(
 (\__/)  
 (•ᴗ•)  
 / づ  
)",     // Happy face
        R"(
 (\__/)  
 (• - •)  
 / づ  
)",     // Neutral face
        R"(
 (\__/)  
 (×_×)  
 / づ  
)"      // Tired/unhappy face
    };

    int frameIndex = 0;  // Current frame to display

    // Start the input handling thread
    std::thread t(inputThread);

    // Track time for updating pet stats
    auto lastUpdate = std::chrono::steady_clock::now();

    const int decayIntervalSeconds = 5;  // Stats decay every 5 seconds

    // Main game loop
    while (running) {
        clearConsole();  // Clear screen for fresh display

        // Display the current animation frame
        std::cout << frames[frameIndex] << "\n";

        // Display pet stats
        std::cout << "--- " << myPet.petName << "'s Stats ---\n";
        std::cout << "Hunger: " << myPet.hunger << "\n";
        std::cout << "Happiness: " << myPet.happiness << "\n";
        std::cout << "Energy: " << myPet.energy << "\n";

        // Display available commands
        std::cout << "\nCommands: feed | play | sleep | exit\n";

        // Display the current input buffer (what the user is typing)
        {
            std::lock_guard<std::mutex> lock(inputMutex);  // Thread-safe access to shared buffer
            std::cout << "Command input: " << inputBuffer << std::flush;
        }

        // Check if it's time to update pet stats
        auto now = std::chrono::steady_clock::now();
        if (now - lastUpdate >= std::chrono::seconds(decayIntervalSeconds)) {
            myPet.update();  // Update pet stats
            frameIndex = (frameIndex + 1) % frames.size();  // Cycle through animation frames
            lastUpdate = now;  // Reset timer
        }

        // Check for completed commands (when user presses Enter)
        {
            std::unique_lock<std::mutex> lock(inputMutex);
            if (!inputBuffer.empty() && inputBuffer.back() == '\n') {
                inputBuffer.pop_back();  // Remove the newline character
                std::string command = inputBuffer;
                inputBuffer.clear();  // Clear the buffer for new input

                // Process the command
                if (command == "feed") myPet.feed();
                else if (command == "play") myPet.play();
                else if (command == "sleep") myPet.sleep();
                else if (command == "exit") running = false;
                else {
                    // Handle invalid commands
                    std::cout << "\nInvalid command: " << command << "\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }

        // Short delay to prevent CPU overuse
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait for the input thread to finish
    t.join();

    // Farewell message
    std::cout << "\nThanks for playing with " << myPet.petName << "!\n";
    return 0;
}

/**
 * Clears the console using ANSI escape codes
 * \033[2J clears the screen
 * \033[1;1H moves the cursor to the top-left corner
 */
void clearConsole() {
    std::cout << "\033[2J\033[1;1H";
}

/**
 * Sets terminal input to non-blocking mode
 * In non-blocking mode, input can be read character by character without waiting for Enter
 *
 * @param enable True to enable non-blocking mode, false to restore normal mode
 */
void setNonBlocking(bool enable) {
    static struct termios oldt;  // Stores original terminal settings
    static bool saved = false;   // Flag to track if original settings were saved

    if (enable) {
        // Save original terminal settings if not already saved
        if (!saved) {
            tcgetattr(STDIN_FILENO, &oldt);
            saved = true;
        }

        // Create new terminal settings based on old ones
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new settings

        // Also set non-blocking flag on stdin
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
    else {
        // Restore original terminal settings if they were saved
        if (saved) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
        }
    }
}

/**
 * Thread function for handling user input
 * Runs in a separate thread to continuously read input without blocking the main game loop
 */
void inputThread() {
    setNonBlocking(true);  // Enable non-blocking input

    // Input handling loop
    while (running) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);  // Try to read one character

        if (n > 0) {  // If a character was read
            std::lock_guard<std::mutex> lock(inputMutex);  // Thread-safe access to shared buffer

            if (c == '\n') {  // Enter key
                inputBuffer.push_back(c);
                inputCV.notify_one();  // Notify waiting threads
            }
            else if (c == 127 || c == 8) {  // Backspace or Delete key
                if (!inputBuffer.empty()) inputBuffer.pop_back();
            }
            else {  // Any other character
                inputBuffer.push_back(c);
            }
        }
        else {
            // No character was read, sleep briefly to avoid CPU hogging
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    // Restore terminal settings when thread exits
    setNonBlocking(false);
}