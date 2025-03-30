#pragma once

#include <vector>
#include <algorithm>
#include <climits>
#include "extraHeuristics/transposition/transposition.h"
#include "engine-related/evaluation.h"
#include "extraHeuristics/historyHeuristic.h"
#include "extraHeuristics/killerMoves.h"

namespace coredump
{
    int sortMoves(std::vector<Move> &moves, const Position &pos, int ply, Color color);
}
