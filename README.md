# Dog Race Game  

## Game Overview  
A text-based racing game where players control their dog (represented by `@`) in a race against two CPU-controlled dogs (`%` and `#`). The goal is to reach the finish line first on a 100-unit track by pressing the spacebar to move forward.  


## Game Rules  
- **Player Control**: Press the **spacebar** to move your dog (`@`) forward by a random 1–3 steps.  
- **CPU Dogs**: Two computer-controlled dogs (`%` and `#`) move 1–2 steps every 0.5 seconds automatically.  
- **Track Length**: The race track is 100 character units long.  
- **Win Condition**: The first dog to reach or exceed the track length (100 units) wins the race.  


## Controls  
- **Spacebar**: Move the player’s dog forward.  
- **Any Key**: Start the game from the welcome screen or exit after the race ends.  


## Compilation and Running  

### Recommended: Using Makefile  
The `Makefile` supports cross-platform compilation and execution:  

```bash  
# Compile for any platform (Linux/Windows)  
make  

# Run the game (auto-detects your OS)  
make run  

# Run specifically on Windows  
make run-win  

# Run specifically on Linux/Unix  
make run-unix  
