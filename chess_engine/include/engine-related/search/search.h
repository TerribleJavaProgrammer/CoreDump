#ifndef SEARCH_H
#define SEARCH_H

#include <stdint.h>
#include "board/position.h"
#include "extraHeuristics/transposition/transposition.h"
#include "extraHeuristics/transposition/TTentry.h"
#include "extraHeuristics/transposition/TTflag.h"
#include "move/movegen.h"
#include "move/move.h"
#include "engine-related/evaluation.h"
#include "engine-related/prioritization.h"
#include "global_constants.h"
#include "extraHeuristics/historyHeuristic.h"
#include "extraHeuristics/killerMoves.h"
#include "move/nullMove/nullMoveHandler.h"

int negamax(Position& pos, int depth, int alpha, int beta, Move::Color color, int ply);
int quiescenceSearch(Position& pos, int alpha, int beta, Move::Color color, int ply);

#endif // SEARCH_H