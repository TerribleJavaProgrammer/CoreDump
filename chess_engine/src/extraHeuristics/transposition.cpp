#include <stdint.h>
#include "extraHeuristics/transposition/transposition.h"
#include "extraHeuristics/transposition/TTflag.h"
#include "move/move.h"

std::unordered_map<uint64_t, TTEntry> transpositionTable;

void storeTT(uint64_t hash, int depth, int score, Move bestMove, TTFlag flag) {
    auto it = transpositionTable.find(hash);

    // Replace only if the new depth is greater or equal (prevents shallow overwrites)
    if (it == transpositionTable.end() || it->second.depth <= depth) {
        transpositionTable[hash] = {hash, depth, score, bestMove, flag};
    }
}

TTEntry* probeTT(uint64_t zobristKey, int depth, int alpha, int beta) {
    auto entry = transpositionTable.find(zobristKey);
    if (entry == transpositionTable.end()) return nullptr; // No entry found

    TTEntry* tt = &entry->second;
    if (tt->depth >= depth) {
        if (tt->flag == EXACT) return tt;
        if (tt->flag == LOWERBOUND && tt->score >= beta) return tt;
        if (tt->flag == UPPERBOUND && tt->score <= alpha) return tt;
    }

    return nullptr; // No usable data
}