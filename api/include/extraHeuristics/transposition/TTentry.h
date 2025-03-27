#ifndef TTENTRY_H
#define TTENTRY_H

#include "move/move.h"

struct TTEntry
{
    uint64_t zobristKey; // Unique position hash
    int depth;           // Depth of search
    int score;           // Stored evaluation score
    Move bestMove;       // Best move found
    int flag;            // Exact, Upper bound, Lower bound
};

#endif // TTETNTRY_H