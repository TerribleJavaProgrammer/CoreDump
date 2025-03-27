#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include "move/move.h"

struct SearchResult
{
    int score;
    Move bestMove;
    SearchResult() : score(0), bestMove(Move()) {}
    SearchResult(int s, Move m) : score(s), bestMove(m) {}
};

#endif // SEARCHRESULT_H