#pragma once

#include <string>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include "pieceType.h"
#include "color.h"
#include "castlingType.h"

namespace coredump
{
    struct Move
    {
        int fromSquare;  // Source square (0-63)
        int toSquare;    // Destination square (0-63)
        bool isCapture;  // True if this move captures a piece
        bool isCastling; // True if this is a castling move

        PieceType pieceType;       // Type of piece being moved
        Color color;               // Color of the piece being moved
        CastlingType castlingType; // Type of castling move

        bool isPromotion;         // True if this is a pawn promotion
        PieceType promotionPiece; // Piece type to promote to

        // Movement undoing fields
        uint64_t capturedPieceBitboard; // Bitboard of captured piece (if any)
        PieceType capturedPieceType;    // Type of captured piece (if any)

        uint64_t prevWhitePieces, prevBlackPieces; // Board states before move
        uint64_t prevOccupied;                     // Previous occupied squares

        int prevEnPassantSquare;    // En passant square before the move (-1 if none)
        int prevKingSquare;         // Stores king's position before the move
        uint8_t prevCastlingRights; // Bitmask of castling rights before move

        // Constructor
        Move(int from, int to, bool capture, PieceType type, Color col, bool castling,
             CastlingType castlingType = CastlingType::NONE, bool promotion = false,
             PieceType promoPiece = PieceType::NONE, uint64_t capturedBitboard = 0,
             PieceType capturedType = PieceType::NONE, uint64_t prevWhite = 0,
             uint64_t prevBlack = 0, uint64_t prevOccupiedSquares = 0,
             int enPassantSquare = -1, uint8_t castlingRights = 0, int kingSquare = -1);

        Move(std::string moveStr, Color col);

        // Default constructor
        Move();

        std::string getPgn();
        bool operator==(const Move &other) const;

        static std::string toAlgebraic(int square);
        static int fromAlgebraic(char file, char rank);
    };
}
