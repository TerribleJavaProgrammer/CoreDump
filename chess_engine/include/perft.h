#ifndef PERFT_H
#define PERFT_H

#include <stdint.h>
#include <iostream>
#include <thread>
#include <future>
#include "board/position.h"
#include "move/movegen.h"
#include "move/move.h"


// Perft function (single-threaded)
uint64_t perft(const Position& pos, int depth, Move::Color color);

// Perft function (multi-threaded)
uint64_t parallelPerft(const Position& pos, int depth, Move::Color color, int numThreads);

// Runs a single-threaded perft test
void runPerftTest(int depth);

// Runs a multi-threaded perft test
void runParallelPerft(int depth, int numThreads);

#endif // PERFT_H