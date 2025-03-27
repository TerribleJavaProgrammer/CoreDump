#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>
#include <string>
#include <iostream>
#include "move/move.h"
#include "board/bitboard.h"
#include "extraHeuristics/zobrist.h"
#include "color.h"

class Position
{
private:
    char getSquareChar(int square);

public:
    // Public bitboard fields
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    uint8_t castlingRights;
    int enPassantSquare;

    // Constructor
    Position();

    // Getters for on the fly composite bitboards
    uint64_t getWhitePieces() {}
    uint64_t getBlackPieces() {}
    uint64_t getOccupiedSquares() {}
    uint64_t getEmptySquares() {}

    // Function prototypes
    uint64_t computeHash() const;
    std::string displayPosition();
    std::string getFen(bool whiteToMove, int halfmoveClock, int fullmoveNumber, std::string castlingRights, std::string enPassantTarget);
};

#endif // POSITION_H
