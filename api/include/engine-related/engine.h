#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <array>
#include <random>
#include <bitset>
#include <unordered_map>
#include "move/move.h"
#include "board/position.h"
#include "move/movegen.h"
#include "engine-related/search.h"
#include "engine-related/prioritization.h"
#include "engine-related/evaluation.h"
#include "board/threadSafePosition.h"
#include "color.h"

Move findBestMove(Position position, Color color, int maxDepth, double timeLimitSeconds, bool debug);

#endif // ENGINE_H