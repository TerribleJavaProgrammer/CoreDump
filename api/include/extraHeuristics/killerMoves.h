#pragma once

#include <mutex>
#include "move/move.h"

namespace coredump
{
    // Killer move history
    extern Move killerMoves[100][2];
    
    // Mutex to protect killerMoves array
    extern std::mutex killerMovesMutex;

    inline void storeKillerMove(Move move, int ply)
    {
        if (0 <= ply && ply < 100)
        {
            // Lock the mutex before accessing shared data
            std::lock_guard<std::mutex> lock(killerMovesMutex);
            
            if (!(killerMoves[ply][0] == move))
            {
                killerMoves[ply][1] = killerMoves[ply][0]; // Shift second-best
                killerMoves[ply][0] = move;                // Store best killer move
            }
        }
    }
}
