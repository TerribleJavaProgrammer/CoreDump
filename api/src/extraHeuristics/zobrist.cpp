#include "extraHeuristics/zobrist.h"

std::array<std::array<uint64_t, 64>, 12> zobristTable;
uint64_t zobristEnPassant[8];
uint64_t zobristCastling[16]; // 16 combinations of castling rights
uint64_t zobristTurn;

// Initializes Zobrist hashing table
void initZobrist()
{
    std::mt19937_64 rng(123456789); // Fixed seed for reproducibility
    std::uniform_int_distribution<uint64_t> dist;

    for (int piece = 0; piece < 12; ++piece)
        for (int square = 0; square < 64; ++square)
            zobristTable[piece][square] = dist(rng);

    for (int i = 0; i < 8; ++i)
        zobristEnPassant[i] = dist(rng);
    for (int i = 0; i < 16; ++i)
        zobristCastling[i] = dist(rng);
    zobristTurn = dist(rng);
}