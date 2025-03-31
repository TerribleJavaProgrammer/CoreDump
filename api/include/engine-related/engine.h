#pragma once

#include <stdlib.h>
#include <iostream>
#include <sstream>
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
#include "engine-related/search.h"
#include "board/threadSafePosition.h"

namespace coredump
{
    Move findBestMove(const Position &position, Color color, int maxDepth, double timeLimitSeconds, bool debug, std::ostringstream &debugStream);
    Move findRandomMove(const Position &position, Color color);
}
