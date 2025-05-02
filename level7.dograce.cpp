// Game Name: Dog race
// Description: This is a C++ program where symbols are used to represent dogs in a race. 
// Players need to keep pressing the space bar to make the dogs” @ “ move and compete for the first place.
// Made by: Chan Pak Tung 3036328173
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
// Add platform detection headers
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// Simple program mutex mechanism
bool acquireLock() {
    #ifdef _WIN32
    // Windows platform uses named mutex
    HANDLE hMutex = CreateMutex(NULL, TRUE, "DogRaceGameMutex");
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) {
            CloseHandle(hMutex);
        }
        return false;
    }
    return true;
    #else
    // Linux/Unix platform uses file lock
    int fd = open("/tmp/dograce.lock", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        return false;
    }
    if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
        close(fd);
        return false;
    }
    // Don't close fd to maintain the lock, OS will automatically close it when program exits
    return true;
    #endif
}

// Non-blocking keyboard input function
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Clear the screen - Using more reliable multi-platform screen clearing method
void clearScreen() {
    // Combine multiple screen clearing methods to ensure maximum effectiveness
    
    // Method 1: ANSI escape sequence for screen clearing (works in most terminals)
    std::cout << "\033[2J"; // Clear entire screen
    std::cout << "\033[3J"; // Clear scrollback buffer (in supported terminals)
    std::cout << "\033[1;1H"; // Move cursor to top-left corner
    std::cout.flush(); // Flush output immediately
    
    // Method 2: Output multiple newlines to scroll the screen (universal method)
    for (int i = 0; i < 5; i++) {
        std::cout << std::endl;
    }
    
    // Method 3: Set cursor position again to ensure subsequent output starts from the top
    std::cout << "\033[1;1H" << std::flush;
    
    // Short delay to ensure above methods take effect
    usleep(10000); // 10 milliseconds
}

// Set cursor position
void setCursorPosition(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

// Set text color
void setConsoleColor(int colorCode) {
    std::cout << "\033[" << colorCode << "m";
}

// Hide cursor
void hideCursor() {
    std::cout << "\033[?25l";
}

// Show cursor
void showCursor() {
    std::cout << "\033[?25h";
}

// Dog class definition
class Dog {
private:
    char symbol;         // Dog's symbol, such as @, %, #
    int position;        // Position on the track
    bool isPlayer;       // Whether it's player-controlled
    std::string name;    // Dog's name

public:
    Dog(char symbol, int initialPosition, bool isPlayer, const std::string& name) 
        : symbol(symbol), position(initialPosition), isPlayer(isPlayer), name(name) {
    }
    
    // Get the dog's symbol
    char getSymbol() const {
        return symbol;
    }
    
    // Get the dog's position
    int getPosition() const {
        return position;
    }
    
    // Get the dog's name
    std::string getName() const {
        return name;
    }
    
    // Check if it's player-controlled
    bool isPlayerControlled() const {
        return isPlayer;
    }
    
    // Move the dog
    void move(int steps) {
        position += steps;
    }
};

// Track class definition
class Track {
private:
    const int length;            // Total track length
    std::vector<Dog*> dogs;      // All participating dogs
    
public:
    // Constructor, sets the track length
    Track(int length) : length(length) {
    }
    
    // Destructor doesn't need to delete Dog pointers in dogs, as they'll be managed in the Game class
    ~Track() {
        // No need to delete pointers in dogs, they're managed by the Game class
    }
    
    // Add a dog to the track
    void addDog(Dog* dog) {
        dogs.push_back(dog);
    }
    
    // Get the track length
    int getLength() const {
        return length;
    }
    
    // Check if any dog has finished the race
    bool isRaceFinished() const {
        for (const auto& dog : dogs) {
            if (dog->getPosition() >= length) {
                return true;
            }
        }
        return false;
    }
    
    // Get the winning dog (if any)
    Dog* getWinner() const {
        for (const auto& dog : dogs) {
            if (dog->getPosition() >= length) {
                return dog;
            }
        }
        return nullptr;
    }
    
    // Get all dogs
    const std::vector<Dog*>& getDogs() const {
        return dogs;
    }
    
    // Render the track state
    void render() const {
        // Only move cursor to the top-left corner, do not clear screen content
        // Since Game::run() already has clearScreen() operation, no need to repeat screen clearing here
        // Removed screen clearing command: std::cout << "\033[2J";
        std::cout << "\033[1;1H" << std::flush; // Move cursor to top-left corner
        
        // Display game title
        std::cout << "\033[33m"; // Yellow
        std::cout << "╔═══════════════════════ DOG RACE ════════════════════════╗" << std::endl;
        std::cout << "\033[37m"; // White
        
        // Get ranking information
        auto ranking = getRanking();
        
        // Display positions and rankings of all dogs
        std::cout << "\033[36m"; // Cyan
        std::cout << "║  ";
        
        // Display player dog information
        for (const auto& dog : dogs) {
            if (dog->isPlayerControlled()) {
                std::cout << "You(@): " << std::setw(3) << dog->getPosition() << "/" << length;
                
                // Display player ranking
                for (size_t i = 0; i < ranking.size(); ++i) {
                    if (ranking[i]->isPlayerControlled()) {
                        std::cout << " [" << (i + 1) << "st]";
                        break;
                    }
                }
                break;
            }
        }
        
        // Fill with spaces for layout alignment
        std::cout << std::string(26, ' ') << "║" << std::endl;
        
        // Display CPU dog information
        std::cout << "║  ";
        bool firstCpu = true;
        for (const auto& dog : dogs) {
            if (!dog->isPlayerControlled()) {
                if (!firstCpu) {
                    std::cout << " | ";
                }
                std::cout << dog->getName() << "(" << dog->getSymbol() << "): " << std::setw(3) << dog->getPosition();
                firstCpu = false;
            }
        }
        
        // Fill with spaces for layout alignment
        std::cout << std::string(26, ' ') << "║" << std::endl;
        
        std::cout << "\033[37m"; // Reset to white
        std::cout << "╠════════════════════════════════════════════════════════╣" << std::endl;
        
        // Draw track area
        for (const auto& dog : dogs) {
            int pos = dog->getPosition();
            std::string trackBody(length, ' '); // Track body
            
            // Draw track background
            for (int i = 0; i < length; i += 4) {
                trackBody[i] = '.';
            }
            
            // Place dog symbol on the track
            if (pos >= 0 && pos < length) {
                trackBody[pos] = dog->getSymbol();
            }
            
            // Dog color
            std::cout << "║ ";
            if (dog->getSymbol() == '@') {
                std::cout << "\033[32m"; // Green for player dog
            } else if (dog->getSymbol() == '%') {
                std::cout << "\033[31m"; // Red for CPU1 dog
            } else {
                std::cout << "\033[34m"; // Blue for CPU2 dog
            }
            
            // Print track
            std::cout << trackBody;
            
            // Finish line
            std::cout << "\033[37m"; // Bright white
            std::cout << "║";
            std::cout << "\033[33m"; // Yellow
            std::cout << "▌▌";
            std::cout << "\033[37m"; // Reset to white
            std::cout << " ║" << std::endl;
        }
        
        // Draw bottom border
        std::cout << "\033[33m"; // Yellow
        std::cout << "╚════════════════════════════════════════════════════════╝" << std::endl;
        
        // Prompt information
        std::cout << "\033[37m"; // Bright white
        std::cout << "  Press SPACE to make your dog (@) move forward!" << std::endl;
        std::cout << "\033[0m"; // Reset all attributes
    }
    
    // Get the current ranking of dogs
    std::vector<Dog*> getRanking() const {
        std::vector<Dog*> ranking = dogs;
        std::sort(ranking.begin(), ranking.end(), [](const Dog* a, const Dog* b) {
            return a->getPosition() > b->getPosition();
        });
        return ranking;
    }
};

// Game class definition
class Game {
private:
    Track track;                 // Track
    Dog playerDog;               // Player's dog
    Dog cpuDog1;                 // CPU's dog 1
    Dog cpuDog2;                 // CPU's dog 2
    bool gameOver;               // Whether the game is over
    
    std::mt19937 rng;            // Random number generator
    
    // Last time the CPU dogs moved
    std::chrono::time_point<std::chrono::steady_clock> lastCpuMoveTime;
    
    // Get random movement steps
    int getRandomSteps(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
    
    // Handle player input
    void handleInput() {
        if (kbhit()) {
            char key = getchar();
            if (key == ' ') {
                movePlayer();
            }
        }
    }
    
    // Update CPU dog positions
    void updateCpuDogs() {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastCpuMoveTime).count();
        
        // CPU dogs move every 500 milliseconds (0.5 seconds)
        if (elapsedTime >= 500) {
            // Use the same random step count for both CPU dogs to ensure consistent movement speed
            int sharedSteps = getRandomSteps(1, 2);
            
            // Both CPU dogs advance by the same number of steps
            cpuDog1.move(sharedSteps);
            cpuDog2.move(sharedSteps);
            
            // Update last move time
            lastCpuMoveTime = currentTime;
        }
    }
    
public:
    // Constructor
    Game() 
        : track(100),                // Initialize track length to 100
          playerDog('@', 0, true, "Player"),  // Initialize player's dog
          cpuDog1('%', -5, false, "CPU1"),  // Initialize CPU's dog 1, position at -5
          cpuDog2('#', -5, false, "CPU2"), // Initialize CPU's dog 2, position set to same as CPU1
          gameOver(false) {
        
        // Initialize random number generator
        std::random_device rd;
        rng = std::mt19937(rd());
        
        // Initialize last CPU dog move time
        lastCpuMoveTime = std::chrono::steady_clock::now();
    }
    
    // Destructor
    ~Game() {
        // In the destructor, ensure terminal settings are restored
        showCursor();
    }
    
    // Initialize the game
    void initialize() {
        // Add debug information to help identify if the program is executed multiple times
        #ifndef NDEBUG
        std::cerr << "DEBUG: Game initialization started..." << std::endl;
        #endif
        
        // Add all dogs to the track
        track.addDog(&playerDog);
        track.addDog(&cpuDog1);
        track.addDog(&cpuDog2);
        
        // Thoroughly clear the screen, ensuring no previous content remains
        std::cout << "\033[2J\033[1;1H\033[3J" << std::flush; // Add \033[3J to clear scrollback buffer
        std::cout.flush();
        system("clear"); // Use system command to clear screen more thoroughly
        
        // Set up terminal
        clearScreen();
        hideCursor();
        
        // Only show the initial animation once to reduce repeated displays
        clearScreen();
        setConsoleColor(33); // Yellow
        
        std::cout << R"(
            .--.--.
           /  ()  \
          |   ^^   |
          \`----'/ 
           `------'  
)" << std::endl;
        
        setConsoleColor(37); // Bright white
        std::cout << R"(
        ╔═══════════════════════════════════╗
        ║          DOG RACE                 ║
        ╚═══════════════════════════════════╝
)" << std::endl;
        
        // Add game instructions
        setConsoleColor(36); // Cyan
        // Display prompt information with alternating colors
        setConsoleColor(32); // Green
        std::cout << R"(
        >>> Press SPACE to start! <<<
)" << std::endl;
        
        setConsoleColor(0); // Restore default color
        
        // Wait for spacebar, but no longer use looping animation
        setConsoleColor(37); // Bright white
        std::cout << "\n     Waiting for SPACE key..." << std::endl;
        setConsoleColor(0); // Restore default color
        
        // Simplified key waiting logic
        while (true) {
            if (kbhit()) {
                char key = getchar();
                if (key == ' ') break;
            }
            usleep(50000); // 50ms
        }
        
        // Clear the screen again to ensure a clean interface before the game starts
        clearScreen();
    }
    
    // Player moves on space press
    void movePlayer() {
        // Calculate the position difference between player and CPU dogs
        int diffWithCpu1 = playerDog.getPosition() - cpuDog1.getPosition();
        int diffWithCpu2 = playerDog.getPosition() - cpuDog2.getPosition();
        
        // Adjust player movement steps based on position difference
        if (diffWithCpu1 > 15 && diffWithCpu2 > 15) {
            // If player is too far ahead, slightly reduce movement steps
            playerDog.move(getRandomSteps(1, 2));
        } else if ((diffWithCpu1 < -15 || diffWithCpu2 < -15) && 
                  (playerDog.getPosition() < track.getLength() * 0.7)) {
            // If player is too far behind and not near the finish line, slightly increase movement steps
            // But only provide this "catch-up" mechanism in the first 70% of the race
            playerDog.move(getRandomSteps(2, 4));
        } else {
            // Normal case
            playerDog.move(getRandomSteps(1, 3));
        }
    }
    
    // Check if the game is over
    bool isGameOver() const {
        return gameOver;
    }
    
    // Get the winner
    Dog* getWinner() {
        return track.getWinner();
    }
    
    // Game main loop
    void run() {
        // Thoroughly clear the screen again before starting the main game loop
        std::cout << "\033[2J\033[1;1H\033[3J" << std::flush; // Clear screen and scrollback buffer
        std::cout.flush();
        #ifndef _WIN32
        system("clear"); // Use system clear command in non-Windows environments
        #endif
        clearScreen(); // Use our own clear screen function
        
        // Prepare the game main loop
        auto startTime = std::chrono::steady_clock::now();
        
        while (!gameOver) {
            // Clear screen at the start of each loop to ensure a clean interface
            // clearScreen(); // Remove this line because Track::render() already includes screen clearing
            
            handleInput();
            updateCpuDogs();
            track.render();
            
            if (track.isRaceFinished()) {
                gameOver = true;
                Dog* winner = getWinner();
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                double seconds = duration.count() / 1000.0;
                
                // Wait a short time to let the player see the final track state
                usleep(1000000); // 1 second
                
                // Display the ending screen
                clearScreen();
                showCursor(); // Restore cursor at the end
                
                if (winner && winner->isPlayerControlled()) {
                    // Victory screen
                    setConsoleColor(32); // Green
                    std::cout << R"(
        ╔═══════════════════════════════════════════════════╗
        ║                                                   ║
        ║                                                   ║
                       ___________ 
                      '._==_==_=_.'
                      .-\:      /-.
                     | (|:.     |) |
                      '-|:.     |-'
                        \::.    /
                         '::. .'
                           ) (
                         _.' '._
                        `-------`
        ║                                                   ║
        ║                                                   ║
        ╚═══════════════════════════════════════════════════╝
)" << std::endl;

                    setConsoleColor(33); // Yellow
                    std::cout << R"(
             ✨ CONGRATULATIONS! ✨
)" << std::endl;
                    
                    setConsoleColor(37); // Bright white
                    std::cout << "    Your dog finished in 1st place!\n" << std::endl;
                    std::cout << "    Time: " << std::fixed << std::setprecision(2) << seconds << " seconds\n" << std::endl;
                } else {
                    // Defeat screen
                    setConsoleColor(31); // Red
                    std::cout << R"(
        ╔═══════════════════════════════════════════════════╗
        ║                                                   ║
        ║                                                   ║
                        ^--^
                       |o_o |
                       |:_/ |
                      //   \ \
                     (|     | )
                    /'\_   _/`\
                    \___)=(___/
        ║                                                   ║
        ║                                                   ║
        ╚═══════════════════════════════════════════════════╝
)" << std::endl;

                    setConsoleColor(33); // Yellow
                    std::cout << R"(
             😢 Better luck next time! 😢
)" << std::endl;
                    
                    setConsoleColor(37); // Bright white
                    std::cout << "    You finished in 3rd place...\n" << std::endl;
                }
                
                setConsoleColor(0); // Restore default color
                std::cout << "    Press any key to exit..." << std::endl;
                getchar();
            }
            
            // Short delay between loops to control game speed
            // Approximately 30ms delay, corresponding to about 33FPS
            // usleep(30000);
            // Increase delay to 100ms (10FPS) to reduce refresh frequency and improve flickering issues
            usleep(100000);
        }
    }
};

int main() {
    // Ensure only one game instance is running
    if (!acquireLock()) {
        std::cerr << "Error: Game is already running!" << std::endl;
        return 1;
    }
    
    // Set up UTF-8 display (Windows environment only)
    #ifdef _WIN32
    system("chcp 65001");
    #endif
    
    // Initialize random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create game instance
    Game dogRace;
    
    // Initialize game
    dogRace.initialize();
    
    // Run game
    dogRace.run();
    
    return 0;
}