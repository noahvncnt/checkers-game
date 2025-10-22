# Bitboard Checkers

A simple terminal-based Checkers game implemented in C using **64-bit bitboards** for piece representation.

## Features
- Bitboard-based move and capture logic  
- King promotion and forced captures  
- Simple text UI
- Save/load game state  

## Build and Run


### Using GCC
Download all files.
Run this prompt with a command prompt in the project folder
```bash
gcc -std=c11 -Wall -Wextra -O2 -o checkers main.c bitops.c utils.c game.c ui.c save.c
```
This will create the file checkers.exe silently

run checkers.exe
