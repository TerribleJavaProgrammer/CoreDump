#ifndef MAGICENTRY_H
#define MAGICENTRY_H

#include <stdint.h>
#include <vector>

struct MagicEntry
{
    uint64_t mask;                 // Mask for relevant occupancy bits
    uint64_t magic;                // Magic number for this square
    int shift;                     // Right shift amount
    std::vector<uint64_t> attacks; // Pre-calculated attack patterns
};

#endif // MAGICENTRY_H