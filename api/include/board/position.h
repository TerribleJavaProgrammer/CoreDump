#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "board/bitboard.h"
#include "extraHeuristics/zobrist.h"

namespace coredump
{
    inline void updateBitboard(uint64_t &bitboard, uint64_t &fromBB, uint64_t &toBB)
    {
        bitboard &= ~fromBB; // Remove piece from its original square
        bitboard |= toBB;    // Place piece in the new square
    }

    inline void handleCapture(uint64_t &bitboard, uint64_t &captureBB)
    {
        bitboard &= ~captureBB; // Remove captured piece from its square
    }

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

        // Starting board state constructor
        Position();
        // Copy constructor
        Position(const Position &other);
        // Construct from position + move
        Position(const Position &other, const Move &move);

        // Getters for on the fly composite bitboards
        uint64_t getWhitePieces() const;
        uint64_t getBlackPieces() const;
        uint64_t getOccupiedSquares() const;
        uint64_t getEmptySquares() const;

        // Function prototypes
        uint64_t computeHash() const;
        std::string displayPosition();
        std::string getFen(Color toMove, int halfmoveClock, int fullmoveNumber, std::string castlingRights, std::string enPassantTarget);

        // Move modifiers
        void makeMove(const Move &move);
        void undoMove(const Move &move);
    };
}
