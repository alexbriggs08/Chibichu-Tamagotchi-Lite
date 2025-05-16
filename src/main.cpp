/**
 * Clears the console using ANSI escape codes
 * \033[2J clears the screen
 * \033[1;1H moves the cursor to the top-left corner
 */
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
#include <fstream>     // For file I/O operations
#include <filesystem>  // For filesystem operations

 // Forward declarations for functions not requiring Pet class
void clearConsole();       // Function to clear the terminal screen
void setNonBlocking(bool enable); // Function to set terminal input to non-blocking mode
void inputThread();        // Function to handle user input in a separate thread

// Function to get life stage name based on stage index (forward declaration)
std::string getLifeStageName(int stage);

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
    int age = 0;          // Age of the pet (increases over time)

    /**
     * Default constructor
     */
    Pet() = default;

    /**
     * Constructor with all parameters
     */
    Pet(const std::string& name, int h, int happy, int e, int a) :
        petName(name), hunger(h), happiness(happy), energy(e), age(a) {
    }

    /**
     * Get the current life stage of the pet based on age
     * 0: Baby (0-4)
     * 1: Child (5-9)
     * 2: Teen (10-14)
     * 3: Adult (15+)
     */
    int getLifeStage() const {
        if (age < 5) return 0;      // Baby
        else if (age < 10) return 1; // Child
        else if (age < 15) return 2; // Teen
        else return 3;               // Adult
    }

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
        hunger = std::max(hunger - 20, 0);       // Decrease hunger
        happiness = std::min(happiness + 10, 100); // Increase happiness
        energy = std::max(energy - 15, 0);       // Decrease energy
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

    /**
     * Age the pet - called periodically to increase age
     */
    void ageUp() {
        age++;
    }
};

// Forward declarations for functions that require Pet class
bool savePet(const std::string& filename, const Pet& pet); // Function to save pet to a file
bool loadPet(const std::string& filename, Pet& pet);       // Function to load pet from a file

int main() {
    Pet myPet;  // Create a pet instance

    // Get the pet's name from the user
    std::cout << "Name your Chibichu: ";
    std::getline(std::cin, myPet.petName);

    // Define ASCII art frames for the pet's animation at different life stages
    // Each life stage has multiple frames for animation
    std::vector<std::vector<std::string>> lifeStageFrames = {
        // Baby frames (age 0-4)
        {
            R"(
 (\_/)  
 (^.^)  
 >O<   
)",     // Happy baby
            R"(
 (\_/)  
 (-.-)  
 >O<   
)",     // Neutral baby
            R"(
 (\_/)  
 (T.T)  
 >O<   
)"      // Sad baby
        },
        // Child frames (age 5-9)
        {
            R"(
 (\__/)  
 (•ᴗ•)  
 / づ  
)",     // Happy child
            R"(
 (\__/)  
 (• - •)  
 / づ  
)",     // Neutral child
            R"(
 (\__/)  
 (×_×)  
 / づ  
)"      // Sad child
        },
        // Teen frames (age 10-14)
        {
            R"(
 /\_/\  
 (^ω^)  
 |   |  
 U   U  
)",     // Happy teen
            R"(
 /\_/\  
 (· · )  
 |   |  
 U   U  
)",     // Neutral teen
            R"(
 /\_/\  
 (¬_¬)  
 |   |  
 U   U  
)"      // Sad teen
        },
        // Adult frames (age 15+)
        {
            R"(
   /\_/\  
  ( o.o )  
 >(=o=)<  
  /   \   
 (__)(__) 
)",     // Happy adult
            R"(
   /\_/\  
  ( - - )  
 >(=o=)<  
  /   \   
 (__)(__) 
)",     // Neutral adult
            R"(
   /\_/\  
  ( -.- )  
 >(=o=)<  
  /   \   
 (__)(__) 
)"      // Sad adult
        }
    };

    int frameIndex = 0;  // Current frame to display
    auto lastAgeUpdate = std::chrono::steady_clock::now();  // Track time for age updates

    // Start the input handling thread
    std::thread t(inputThread);

    // Track time for updating pet stats
    auto lastUpdate = std::chrono::steady_clock::now();

    const int decayIntervalSeconds = 5;  // Stats decay every 5 seconds
    const int ageIntervalSeconds = 20;   // Age increases every 20 seconds

    // Main game loop
    while (running) {
        clearConsole();  // Clear screen for fresh display

        // Get current life stage
        int lifeStage = myPet.getLifeStage();

        // Get mood index (0: happy, 1: neutral, 2: sad) based on overall stats
        int moodIndex = 0;
        int overallWellbeing = (myPet.hunger + myPet.happiness + myPet.energy) / 3;
        if (overallWellbeing < 33) {
            moodIndex = 2;  // Sad
        }
        else if (overallWellbeing < 66) {
            moodIndex = 1;  // Neutral
        }
        else {
            moodIndex = 0;  // Happy
        }

        // Display the current animation frame for the current life stage and mood
        std::cout << lifeStageFrames[lifeStage][moodIndex] << "\n";

        // Display pet stats and age
        std::cout << "--- " << myPet.petName << "'s Stats ---\n";
        std::cout << "Age: " << myPet.age << " (" << getLifeStageName(lifeStage) << ")\n";
        std::cout << "Hunger: " << myPet.hunger << "\n";
        std::cout << "Happiness: " << myPet.happiness << "\n";
        std::cout << "Energy: " << myPet.energy << "\n";

        // Display available commands
        std::cout << "\nCommands: feed | play | sleep | save | load | exit\n";

        // Display the current input buffer (what the user is typing)
        {
            std::lock_guard<std::mutex> lock(inputMutex);  // Thread-safe access to shared buffer
            std::cout << "Command input: " << inputBuffer << std::flush;
        }

        // Check if it's time to update pet stats
        auto now = std::chrono::steady_clock::now();
        if (now - lastUpdate >= std::chrono::seconds(decayIntervalSeconds)) {
            myPet.update();  // Update pet stats
            lastUpdate = now;  // Reset timer
        }

        // Check if it's time to age up the pet
        if (now - lastAgeUpdate >= std::chrono::seconds(ageIntervalSeconds)) {
            myPet.ageUp();  // Increase pet age
            lastAgeUpdate = now;  // Reset timer

            // Announce when pet grows to a new life stage
            int newLifeStage = myPet.getLifeStage();
            if (newLifeStage > lifeStage) {
                std::cout << "\n" << myPet.petName << " has grown up to "
                    << getLifeStageName(newLifeStage) << " stage!\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
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
                else if (command == "save") {
                    std::string filename = myPet.petName + ".sav";
                    if (savePet(filename, myPet)) {
                        std::cout << "\nSuccessfully saved " << myPet.petName << " to " << filename << "\n";
                    }
                    else {
                        std::cout << "\nFailed to save " << myPet.petName << "!\n";
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
                else if (command == "load") {
                    std::string filename = myPet.petName + ".sav";
                    if (loadPet(filename, myPet)) {
                        std::cout << "\nSuccessfully loaded " << myPet.petName << " from " << filename << "\n";
                    }
                    else {
                        std::cout << "\nFailed to load " << myPet.petName << "! File may not exist.\n";
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
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
 * Gets the name of the life stage based on the stage index
 *
 * @param stage Life stage index (0-3)
 * @return String name of the life stage
 */
std::string getLifeStageName(int stage) {
    switch (stage) {
    case 0: return "Baby";
    case 1: return "Child";
    case 2: return "Teen";
    case 3: return "Adult";
    default: return "Unknown";
    }
}

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

/**
 * Saves the pet's data to a file
 *
 * @param filename The name of the file to save to
 * @param pet The pet object to save
 * @return True if the save was successful, false otherwise
 */
bool savePet(const std::string& filename, const Pet& pet) {
    try {
        std::ofstream saveFile(filename, std::ios::out);
        if (!saveFile.is_open()) {
            return false;
        }

        // Write pet data to file
        saveFile << pet.petName << "\n";
        saveFile << pet.hunger << "\n";
        saveFile << pet.happiness << "\n";
        saveFile << pet.energy << "\n";
        saveFile << pet.age << "\n";  // Also save the pet's age

        saveFile.close();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

/**
 * Loads a pet's data from a file
 *
 * @param filename The name of the file to load from
 * @param pet The pet object to load data into
 * @return True if the load was successful, false otherwise
 */
bool loadPet(const std::string& filename, Pet& pet) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filename)) {
            return false;
        }

        std::ifstream loadFile(filename, std::ios::in);
        if (!loadFile.is_open()) {
            return false;
        }

        // Temporary variables to hold loaded data
        std::string name;
        int h, happy, e, a = 0;  // Initialize age to 0 as default

        // Read pet data from file
        std::getline(loadFile, name);
        loadFile >> h >> happy >> e;

        // Try to read age (might not exist in older save files)
        if (!loadFile.fail()) {
            loadFile >> a;
        }

        if (loadFile.fail() && !loadFile.eof()) {
            loadFile.close();
            return false;
        }

        // Update pet with loaded data
        pet.petName = name;
        pet.hunger = h;
        pet.happiness = happy;
        pet.energy = e;
        pet.age = a;  // Set the loaded age

        loadFile.close();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}