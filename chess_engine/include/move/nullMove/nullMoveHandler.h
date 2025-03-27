#ifndef NULLMOVEHANDLER_H
#define NULLMOVEHANDLER_H

#include <stdint.h>
#include <vector>
#include "move/nullMove/nullMoveState.h"
#include "board/position.h"

extern std::vector<NullMoveState> nullMoveStack;
const uint64_t ZOBRIST_TURN_KEY = 0x9D39247E33776D41ULL; // Random constant for toggling turn


void makeNullMove(Position& pos);
void undoNullMove(Position& pos);

#endif // NULLMOVEHANDLER_H