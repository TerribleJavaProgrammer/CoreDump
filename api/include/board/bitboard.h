#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <stdint.h>
#include "move/move.h"
#include "pieceType.h"

// **BITBOARD MANIPULATION FUNCTIONS**
inline uint64_t setBit(uint64_t board, int square)
{
    return board | (1ULL << square); // Uses bitwise OR to set the bit
}

// Sets a bit to 0 at the specified square position
// Parameters:
//   board: The bitboard to modify
//   square: The square position (0-63) where to clear the bit
// Returns: Modified bitboard with bit set to 0 at specified position
inline uint64_t clearBit(uint64_t board, int square)
{
    return board & ~(1ULL << square); // Uses bitwise AND with inverted mask
}

// Gets the value of a bit at the specified square position
// Parameters:
//   board: The bitboard to check
//   square: The square position (0-63) to examine
// Returns: true if bit is 1, false if bit is 0
inline bool getBit(uint64_t board, int square)
{
    return (board >> square) & 1; // Shifts bit to LSB position and masks
}

inline int popLSB(uint64_t &bb)
{
    int square = __builtin_ctzll(bb); // Get index of least significant set bit
    bb &= bb - 1;                     // Remove that bit
    return square;
}

#endif // BITBOARDS_H