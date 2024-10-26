# PostgreSQL Chess Extension

This repository contains a PostgreSQL extension developed as a course project for INFOH417 Database System Architecture (2023/24) at Université Libre de Bruxelles (ULB). This extension facilitates the storage and retrieval of chess games within PostgreSQL, supporting chess-specific data types, functions, and indices to enable efficient querying of chess game data.

## Overview

Chess games are recorded using various notation standards. This extension utilizes:
- **Standard Algebraic Notation (SAN)** for move sequences (commonly used in Portable Game Notation or PGN)
- **Forsyth–Edwards Notation (FEN)** for representing board positions at specific moves

For details on these notation standards, see:
- [Standard Algebraic Notation (SAN)](https://en.wikipedia.org/wiki/Algebraic_notation_(chess))
- [Forsyth–Edwards Notation (FEN)](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)

## Features

The extension introduces two custom data types and several functions tailored to chess data:

### Custom Data Types
- **`chessboard`**: Represents a specific board state in a chess game.
- **`chessgame`**: Represents a sequence of moves for an entire chess game.

### Functions

#### `getBoard(chessgame, integer) -> chessboard`
- Returns the board state at a specified half-move count (a full move includes one turn by each player).
- Setting the integer parameter to `0` returns the initial board state.

#### `getFirstMoves(chessgame, integer) -> chessgame`
- Truncates and returns the first *N* half-moves of a given game.

#### `hasOpening(chessgame, chessgame) -> bool`
- Checks if a chess game begins with a specific sequence of moves (e.g., opening moves).
- The second parameter should be a partial game sequence, containing only the opening moves to check for.

#### `hasBoard(chessgame, chessboard, integer) -> bool`
- Verifies if a specific board state appears within the first *N* half-moves of a game.

### Indices

The extension also provides specialized indices to optimize common queries:

- **`hasOpening_idx`**: A B-tree index on the `chessgame` type, supporting efficient lookup for games with specific opening sequences.
- **`hasBoard_idx`**: A GIN index that uses `getAllStates(chessgame)` to index individual board states across moves, supporting fast lookup for specific board configurations.

## Installation

### Prerequisites
- PostgreSQL on a Linux-based environment (e.g., Ubuntu, WSL).

### Steps
1. Clone this repository and navigate to the project directory.
2. Run:
   ```bash
   make && make install
