#pragma once

#include "move/move.h"

namespace coredump
{
    // Stores history heuristic
    extern int historyHeuristic[2][64][64];

    inline void storeHistoryHeuristic(Move move, int depth, Color color)
    {
        historyHeuristic[(color == Color::WHITE) ? 0 : 1][move.fromSquare][move.toSquare] += depth * depth;
    }
}
