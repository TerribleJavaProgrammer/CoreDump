#pragma once

#include <stdint.h>
#include <array>
#include <random>

namespace coredump
{
    // Zobrist hashing for position hashing
    extern std::array<std::array<uint64_t, 64>, 12> zobristTable;
    extern uint64_t zobristEnPassant[8];
    extern uint64_t zobristCastling[16]; // 16 combinations of castling rights
    extern uint64_t zobristTurn;

    // Initializes Zobrist hashing table
    void initZobrist();
}
