#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>
#include <string>
#include <iostream>
#include "move/move.h"
#include "board/bitboard.h"
#include "extraHeuristics/zobrist.h"

struct Position {
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    uint64_t whitePieces, blackPieces, occupiedSquares, emptySquares;
    /*
    bit 0: White kingside  (1 if available, 0 if not)
    bit 1: White queenside
    bit 2: Black kingside
    bit 3: Black queenside
    bits 4-7: (Unused for now, but can be used for expansion)
    */
    uint8_t castlingRights;

    int enPassantSquare;

    uint64_t zobristKey = 0; // Stores the hash of the position

    uint64_t computeHash() const;  // Function prototype
    void initializePosition(); // Function prototype
    Position getPosition(); // Function prototype
    std::string displayPosition(); // Function prototype
    char getSquareChar(int square); // function prototype
    std::string getFen(bool whiteToMove, int halfmoveClock, int fullmoveNumber, std::string castlingRights, std::string enPassantTarget); // Function prototype 
};

#endif // POSITION_H

