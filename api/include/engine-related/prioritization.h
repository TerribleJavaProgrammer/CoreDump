#ifndef PRIORITIZATION_H
#define PRIORITIZATION_H

#include <vector>
#include <algorithm>
#include "move/move.h"
#include "board/position.h"
#include "extraHeuristics/transposition/transposition.h"
#include "global_constants.h"
#include "engine-related/evaluation.h"
#include "extraHeuristics/historyHeuristic.h"
#include "extraHeuristics/killerMoves.h"

int sortMoves(std::vector<Move> &moves, const Position &pos, int ply, Color::color);

#endif // PRIORITIZATION_H