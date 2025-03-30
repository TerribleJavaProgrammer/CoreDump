#pragma once

#include <stdint.h>
#include <unordered_map>
#include "extraHeuristics/transposition/TTentry.h"
#include "extraHeuristics/transposition/TTflag.h"

namespace coredump
{
    extern std::unordered_map<uint64_t, TTEntry> transpositionTable;

    void storeTT(uint64_t hash, int depth, int score, Move bestMove, TTFlag flag);

    TTEntry *probeTT(uint64_t zobristKey, int depth, int alpha, int beta);
}
