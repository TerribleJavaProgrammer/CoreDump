#include "extraHeuristics/killerMoves.h"

namespace coredump
{
    // Initialize the mutex
    std::mutex killerMovesMutex;
    
    Move killerMoves[100][2] = {};
}
