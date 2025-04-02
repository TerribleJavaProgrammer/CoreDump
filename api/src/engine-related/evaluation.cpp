#include "engine-related/evaluation.h"
namespace coredump
{
    int evaluatePosition(const Position &pos, Color color)
    {
        int score = 0;

        uint64_t pieces;
        int square;
        if (color == Color::WHITE)
        {
            pieces = pos.whitePawns;
            while (pieces)
            {
                square = popLSB(pieces);
                score += PAWN_VALUE + PAWN_PST[square];
            }
            pieces = pos.whiteKnights;
            while (pieces)
            {
                square = popLSB(pieces);
                score += KNIGHT_VALUE + KNIGHT_PST[square];
            }
            pieces = pos.whiteBishops;
            while (pieces)
            {
                square = popLSB(pieces);
                score += BISHOP_VALUE + BISHOP_PST[square];
            }
            pieces = pos.whiteRooks;
            while (pieces)
            {
                square = popLSB(pieces);
                score += ROOK_VALUE + ROOK_PST[square];
            }
            pieces = pos.whiteQueens;
            while (pieces)
            {
                square = popLSB(pieces);
                score += QUEEN_VALUE + QUEEN_PST[square];
            }
            pieces = pos.whiteKing;
            while (pieces)
            {
                square = popLSB(pieces);
                score += KING_PST[square];
            }

            pieces = pos.blackPawns;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= PAWN_VALUE + PAWN_PST[mirror(square)];
            }
            pieces = pos.blackKnights;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= KNIGHT_VALUE + KNIGHT_PST[mirror(square)];
            }
            pieces = pos.blackBishops;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= BISHOP_VALUE + BISHOP_PST[mirror(square)];
            }
            pieces = pos.blackRooks;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= ROOK_VALUE + ROOK_PST[mirror(square)];
            }
            pieces = pos.blackQueens;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= QUEEN_VALUE + QUEEN_PST[mirror(square)];
            }
            pieces = pos.blackKing;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= KING_PST[mirror(square)];
            }
        } else {
            pieces = pos.blackPawns;
            while (pieces)
            {
                square = popLSB(pieces);
                score += PAWN_VALUE + PAWN_PST[square];
            }
            pieces = pos.blackKnights;
            while (pieces)
            {
                square = popLSB(pieces);
                score += KNIGHT_VALUE + KNIGHT_PST[square];
            }
            pieces = pos.blackBishops;
            while (pieces)
            {
                square = popLSB(pieces);
                score += BISHOP_VALUE + BISHOP_PST[square];
            }
            pieces = pos.blackRooks;
            while (pieces)
            {
                square = popLSB(pieces);
                score += ROOK_VALUE + ROOK_PST[square];
            }
            pieces = pos.blackQueens;
            while (pieces)
            {
                square = popLSB(pieces);
                score += QUEEN_VALUE + QUEEN_PST[square];
            }
            pieces = pos.blackKing;
            while (pieces)
            {
                square = popLSB(pieces);
                score += KING_PST[square];
            }

            pieces = pos.whitePawns;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= PAWN_VALUE + PAWN_PST[mirror(square)];
            }
            pieces = pos.whiteKnights;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= KNIGHT_VALUE + KNIGHT_PST[mirror(square)];
            }
            pieces = pos.whiteBishops;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= BISHOP_VALUE + BISHOP_PST[mirror(square)];
            }
            pieces = pos.whiteRooks;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= ROOK_VALUE + ROOK_PST[mirror(square)];
            }
            pieces = pos.whiteQueens;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= QUEEN_VALUE + QUEEN_PST[mirror(square)];
            }
            pieces = pos.whiteKing;
            while (pieces)
            {
                square = popLSB(pieces);
                score -= KING_PST[mirror(square)];
            }
        }

        return score;
    }

    constexpr int mirror(int square)
    {
        return square ^ 56;
    }
}