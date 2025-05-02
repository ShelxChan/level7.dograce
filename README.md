# level7.dograce
This is a C++ program where symbols are used to represent dogs in a race. Players need to keep pressing the space bar to make the dogs”@“ move and compete for the first place.

Dog Race Game README
1. Game Overview
Dog Race is a console-based game written in C++. In this game, symbols are used to represent dogs participating in a race. Players need to continuously press the space bar to move their dog, represented by the symbol @, forward and compete for the first place against two computer-controlled dogs.
2. Game Rules
The player's dog is represented by @, while the computer-controlled dogs are represented by % and # respectively.
The player must press the space bar repeatedly to move their dog forward.
The computer-controlled dogs move randomly 1 - 2 steps forward every 0.5 seconds.
The race ends when any dog reaches or exceeds the track length of 100 units.
If the player's dog finishes first, a victory screen will be displayed; otherwise, a defeat screen will be shown.
3. Installation and Execution
3.1 Environment Requirements
A compiler that supports C++11 or higher (e.g., g++).
Windows or Linux operating system.
3.2 Compilation
Save the code as level7.dograce.cpp, and then use the following command in the terminal to compile:
sh
g++ level7.dograce.cpp -o dograce
3.3 Execution
After successful compilation, run the game using the following command:
sh
./dograce
4. Key Features
4.1 Platform Compatibility
The game supports both Windows and Linux platforms. Different methods are used for mutex locking and screen clearing on different platforms to ensure smooth operation.
4.2 Input Handling
The game uses a non-blocking keyboard input function kbhit() to detect whether the player presses the space bar, allowing for real-time interaction.
4.3 Random Movement
The movement of both the player's dog and the computer-controlled dogs involves randomness. The player's dog may move different steps forward based on its position relative to the computer-controlled dogs, adding an element of strategy to the game.
4.4 Visual Display
The game uses ANSI escape sequences to set text colors and cursor positions, providing a colorful and intuitive visual display. It also includes a clear and detailed track rendering function to show the positions and rankings of all dogs.
5. Class Structure
5.1 Dog Class
Represents a dog participating in the race.
Contains attributes such as the dog's symbol, position, whether it is player-controlled, and its name.
Provides methods for getting the dog's symbol, position, name, and checking if it is player-controlled, as well as a method for moving the dog.
5.2 Track Class
Represents the race track.
Contains the total length of the track and a list of all participating dogs.
Provides methods for adding dogs to the track, checking if the race is finished, getting the winning dog, getting all dogs, rendering the track state, and getting the current ranking of dogs.
5.3 Game Class
Manages the overall game process.
Contains a track object and three dog objects (one player-controlled dog and two computer-controlled dogs).
Provides methods for initializing the game, handling player input, updating the positions of computer-controlled dogs, checking if the game is over, getting the winner, and running the game main loop.
6. Notes
The game uses a simple mutex mechanism to ensure that only one instance of the game can run at the same time. If you try to run multiple instances, an error message will be displayed.
In the Windows environment, the game sets the console to UTF-8 encoding to ensure correct display of special characters.
7. Acknowledgments
This game was developed by Chan Pak Tung (3036328173).
