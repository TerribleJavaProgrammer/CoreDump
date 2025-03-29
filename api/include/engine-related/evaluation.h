#ifndef EVALUATION_H
#define EVALUATION_H

#include <stdint.h>
#include <array>
#include "board/position.h"
#include "board/bitboard.h"
#include "move/move.h"
#include "color.h"
#include "pieceType.h"
namespace coredump
{
    // Material values for each piece type (in centipawns)
    constexpr int PAWN_VALUE = 100;   // Base value for pawns
    constexpr int KNIGHT_VALUE = 300; // Base value for knights
    constexpr int BISHOP_VALUE = 330; // Base value for bishops
    constexpr int ROOK_VALUE = 500;   // Base value for rooks
    constexpr int QUEEN_VALUE = 900;  // Base value for queens
    constexpr int KING_VALUE = 20000; // Base value for king (very high to prioritize king safety)

    // Piece-Square Tables (PST) for positional evaluation
    // Higher values indicate better squares for that piece
    // Tables are oriented from White's perspective (a1 is bottom-left)

    // Pawn PST - Encourages pawns to:
    // - Advance towards promotion
    // - Control the center
    // - Maintain pawn structure
    constexpr std::array<int, 64> PAWN_PST = {
        0, 0, 0, 0, 0, 0, 0, 0,         // 8th rank
        50, 50, 50, 50, 50, 50, 50, 50, // 7th rank (promotion potential)
        10, 10, 20, 30, 30, 20, 10, 10, // 6th rank
        5, 5, 10, 25, 25, 10, 5, 5,     // 5th rank
        0, 0, 0, 20, 20, 0, 0, 0,       // 4th rank
        5, -5, -10, 0, 0, -10, -5, 5,   // 3rd rank
        5, 10, 10, -20, -20, 10, 10, 5, // 2nd rank
        0, 0, 0, 0, 0, 0, 0, 0          // 1st rank
    };

    // Knight PST - Encourages knights to:
    // - Occupy central squares
    // - Avoid edge squares
    constexpr std::array<int, 64> KNIGHT_PST = {
        -50, -40, -30, -30, -30, -30, -40, -50, // Edge penalties
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30, // Center bonuses
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50};

    // Bishop PST - Encourages bishops to:
    // - Control long diagonals
    // - Support center control
    // - Stay active
    constexpr std::array<int, 64> BISHOP_PST = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20};

    // Rook PST - Encourages rooks to:
    // - Control open files
    // - Move to 7th rank
    // - Support from behind pawns
    constexpr std::array<int, 64> ROOK_PST = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5, // 7th rank bonus
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0 // Slight bonus for central files
    };

    // Queen PST - Encourages queens to:
    // - Stay protected early game
    // - Control center when active
    // - Avoid early development
    constexpr std::array<int, 64> QUEEN_PST = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20};

    // King PST - Encourages kings to:
    // - Stay protected behind pawn shield
    // - Castle early
    // - Avoid center in middlegame
    constexpr std::array<int, 64> KING_PST = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,  // Castled position bonus
        20, 30, 10, 0, 0, 10, 30, 20 // Corner protection bonus
    };

    // Evaluates position
    int evaluatePosition(const Position &pos, Color color);

    // Mirroring function for black's perspective (flips board vertically)
    constexpr int mirror(int square);

    inline int getPieceValue(PieceType piece)
    {
        switch (piece)
        {
        case PieceType::PAWN:
            return PAWN_VALUE;
        case PieceType::KNIGHT:
            return KNIGHT_VALUE;
        case PieceType::BISHOP:
            return BISHOP_VALUE;
        case PieceType::ROOK:
            return ROOK_VALUE;
        case PieceType::QUEEN:
            return QUEEN_VALUE;
        case PieceType::KING:
            return KING_VALUE;
        default:
            return 0;
        }
    }

    // Only allow positive trade captures
    // TODO this restricts the bots abilities for gambits
    inline bool SEE(Move move)
    {
        int capturedValue = getPieceValue(move.capturedPieceType);
        int attackerValue = getPieceValue(move.pieceType);
        return attackerValue <= capturedValue; // Allow only good captures
    }
}
#endif // EVALUATION_H