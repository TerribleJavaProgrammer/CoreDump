#pragma once

#include <mutex>
#include "move/move.h"

namespace coredump
{
    // Stores history heuristic
    extern int historyHeuristic[2][64][64];

    // Mutex to protect killerMoves array
    extern std::mutex historyHeuristicMutex;

    inline void storeHistoryHeuristic(Move move, int depth, Color color)
    {
        if (0 <= depth && depth < 100)
        {
            // Lock the mutex before accessing shared data
            std::lock_guard<std::mutex> lock(historyHeuristicMutex);

            historyHeuristic[(color == Color::WHITE) ? 0 : 1][move.fromSquare][move.toSquare] += depth * depth;
        }
    }
}
