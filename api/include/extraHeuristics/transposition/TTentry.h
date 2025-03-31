#pragma once

#include "move/move.h"
namespace coredump
{
    struct TTEntry
    {
        uint64_t zobristKey; // Unique position hash
        int depth;           // Depth of search
        int score;           // Stored evaluation score
        Move bestMove;       // Best move found
        int flag;            // Exact, Upper bound, Lower bound
    };
}
