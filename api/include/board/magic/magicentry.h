#pragma once

#include <stdint.h>
#include <vector>
namespace coredump
{
    struct MagicEntry
    {
        uint64_t mask;                 // Mask for relevant occupancy bits
        uint64_t magic;                // Magic number for this square
        int shift;                     // Right shift amount
        std::vector<uint64_t> attacks; // Pre-calculated attack patterns
    };
}
