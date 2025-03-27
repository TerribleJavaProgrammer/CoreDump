#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "board/position.h"
#include "move/move.h"

extern Position currPosition;

// **BOARD STATE MANAGEMENT**
void makeMove(Position& pos, const Move& move);   // Makes a move on the board
void undoMove(Position&pos, const Move& move);    // Undoes a move on the board
void initializeBoard();                           // Initializes the board
Position getBoard();

#endif // BOARD_H