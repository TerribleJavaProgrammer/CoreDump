#include "board/magic/magicbitboard.h"

namespace coredump
{
    // Initialize magic lookup tables for efficient move generation
    std::array<MagicEntry, 64> rookTable = {};   // Stores pre-calculated rook move patterns
    std::array<MagicEntry, 64> bishopTable = {}; // Stores pre-calculated bishop move patterns

    void initializeMagicBitboards()
    {
        // Initialize rook tables
        for (int square = 0; square < 64; square++)
        {
            rookTable[square].mask = generateRookMask(square);
            rookTable[square].magic = ROOK_MAGICS[square];
            rookTable[square].shift = 64 - __builtin_popcountll(rookTable[square].mask);

            uint64_t mask = rookTable[square].mask;
            int variations = 1 << __builtin_popcountll(mask);

            // Pre-allocate with proper size
            rookTable[square].attacks.clear();
            rookTable[square].attacks.resize(variations);

            for (int i = 0; i < variations; i++)
            {
                uint64_t blockers = 0ULL;
                int bit = 0;
                for (int j = 0; j < 64; j++)
                {
                    if (mask & (1ULL << j))
                    {
                        if (i & (1 << bit))
                        {
                            blockers |= (1ULL << j);
                        }
                        bit++;
                    }
                }

                uint64_t index = ((blockers * rookTable[square].magic) >> rookTable[square].shift);
                index &= (variations - 1); // Ensure index is within bounds
                rookTable[square].attacks[index] = generateRookAttacks(square, blockers);
            }
        }

        // Similar changes for bishop table initialization
        for (int square = 0; square < 64; square++)
        {
            bishopTable[square].mask = generateBishopMask(square);
            bishopTable[square].magic = BISHOP_MAGICS[square];
            bishopTable[square].shift = 64 - __builtin_popcountll(bishopTable[square].mask);

            uint64_t mask = bishopTable[square].mask;
            int variations = 1 << __builtin_popcountll(mask);

            bishopTable[square].attacks.clear();
            bishopTable[square].attacks.resize(variations);

            for (int i = 0; i < variations; i++)
            {
                uint64_t blockers = 0ULL;
                int bit = 0;
                for (int j = 0; j < 64; j++)
                {
                    if (mask & (1ULL << j))
                    {
                        if (i & (1 << bit))
                        {
                            blockers |= (1ULL << j);
                        }
                        bit++;
                    }
                }

                uint64_t index = ((blockers * bishopTable[square].magic) >> bishopTable[square].shift);
                index &= (variations - 1); // Ensure index is within bounds
                bishopTable[square].attacks[index] = generateBishopAttacks(square, blockers);
            }
        }
    }

    // Generates a mask of potential blocking squares for a rook on a given square
    // Excludes edge squares since they don't affect sliding piece movement calculations
    uint64_t generateRookMask(int square)
    {
        uint64_t mask = 0ULL;
        int rank = square / 8; // Get rank (0-7) from square number
        int file = square % 8; // Get file (0-7) from square number

        // Generate rays in all 4 directions (excluding edges)
        // North ray
        for (int r = rank + 1; r < 7; r++)
            mask |= (1ULL << (r * 8 + file));
        // South ray
        for (int r = rank - 1; r > 0; r--)
            mask |= (1ULL << (r * 8 + file));
        // East ray
        for (int f = file + 1; f < 7; f++)
            mask |= (1ULL << (rank * 8 + f));
        // West ray
        for (int f = file - 1; f > 0; f--)
            mask |= (1ULL << (rank * 8 + f));

        return mask;
    }

    // Generates a mask of potential blocking squares for a bishop on a given square
    // Similar to rook mask but for diagonal movements
    uint64_t generateBishopMask(int square)
    {
        uint64_t mask = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // Generate rays in all 4 diagonal directions (excluding edges)
        // Northeast ray
        for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++)
            mask |= (1ULL << (r * 8 + f));
        // Southeast ray
        for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--)
            mask |= (1ULL << (r * 8 + f));
        // Northwest ray
        for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++)
            mask |= (1ULL << (r * 8 + f));
        // Southwest ray
        for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--)
            mask |= (1ULL << (r * 8 + f));

        return mask;
    }

    // Generates all possible rook attacks for a given square and blocker configuration
    uint64_t generateRookAttacks(int square, uint64_t blockers)
    {
        uint64_t attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // Generate attacks in all 4 directions (stopping at blockers)
        // North ray
        for (int r = rank + 1; r < 8; r++)
        {
            attacks |= (1ULL << (r * 8 + file));
            if (blockers & (1ULL << (r * 8 + file)))
                break;
        }
        // South ray
        for (int r = rank - 1; r >= 0; r--)
        {
            attacks |= (1ULL << (r * 8 + file));
            if (blockers & (1ULL << (r * 8 + file)))
                break;
        }
        // East ray
        for (int f = file + 1; f < 8; f++)
        {
            attacks |= (1ULL << (rank * 8 + f));
            if (blockers & (1ULL << (rank * 8 + f)))
                break;
        }
        // West ray
        for (int f = file - 1; f >= 0; f--)
        {
            attacks |= (1ULL << (rank * 8 + f));
            if (blockers & (1ULL << (rank * 8 + f)))
                break;
        }

        return attacks;
    }

    // Generates all possible bishop attacks from a given square with blocker configuration
    // Parameters:
    //   square: The square the bishop is on (0-63)
    //   blockers: Bitboard representing all blocking pieces
    // Returns: Bitboard with all possible bishop moves/attacks
    uint64_t generateBishopAttacks(int square, uint64_t blockers)
    {
        uint64_t attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // Generate attacks in all 4 diagonal directions (stopping at blockers)
        // Northeast ray
        for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++)
        {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f)))
                break; // Stop if blocker found
        }
        // Southeast ray
        for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--)
        {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f)))
                break; // Stop if blocker found
        }
        // Northwest ray
        for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++)
        {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f)))
                break; // Stop if blocker found
        }
        // Southwest ray
        for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
        {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f)))
                break; // Stop if blocker found
        }

        return attacks;
    }
}