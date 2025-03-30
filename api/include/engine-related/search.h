#pragma once

#include <stdint.h>
#include <chrono>
#include <atomic>
#include <stdint.h>
#include <climits>
#include "board/position.h"
#include "move/movegen.h"
#include "engine-related/evaluation.h"
#include "engine-related/prioritization.h"
#include "extraHeuristics/killerMoves.h"

// extern std::atomic<uint64_t> nodeCount;
// extern std::atomic<uint64_t> leafNodeCount;
namespace coredump
{
    int negamax(const Position &pos, int depth, int alpha, int beta, Color color, int ply,
                std::chrono::high_resolution_clock::time_point startTime, double timeLimit, std::atomic<uint64_t> &nodeCount, std::atomic<uint64_t> &leafNodeCount);
    int quiescenceSearch(const Position &pos, int alpha, int beta, Color color, int ply, std::atomic<uint64_t> &leafNodeCount);
}
