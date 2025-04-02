#include "extraHeuristics/historyHeuristic.h"

namespace coredump
{
    // Initialize the mutex
    std::mutex historyHeuristicMutex;

    int historyHeuristic[2][64][64] = {};
}
