#ifndef KILLERMOVES_H
#define KILLERMOVES_H

#include "move/move.h"

// Killer move history
extern Move killerMoves[100][2];

inline void storeKillerMove(Move move, int ply) {
    if (0 <= ply && ply < 100) {
        if (!(killerMoves[ply][0] == move)) {
            killerMoves[ply][1] = killerMoves[ply][0];  // Shift second-best
            killerMoves[ply][0] = move;  // Store best killer move
        }
    }
}

#endif // KILLERMOVES_H