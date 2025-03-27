#ifndef HISTORYHEURISTIC_H
#define HISTORYHEURISTIC_H

#include "move/move.h"

// Stores history heuristic
extern int historyHeuristic[2][64][64];

inline void storeHistoryHeuristic(Move move, int depth, Move::Color color) {
    historyHeuristic[(color == Move::Color::WHITE) ? 0 : 1][move.fromSquare][move.toSquare] += depth * depth;
}

#endif // HISTORYHEURISTIC_H