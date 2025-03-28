#include "move/movegen.h"

std::vector<Move> generateMoves(Position &pos, Color color)
{
    std::vector<Move> moveList;

    uint64_t ourPieces = (color == Color::WHITE) ? pos.getWhitePieces() : pos.getBlackPieces();
    uint64_t enemyPieces = (color == Color::WHITE) ? pos.getBlackPieces() : pos.getWhitePieces();
    uint64_t ourPiecesCopy = ourPieces; // Preserve original value for iteration

    // Loop thru ourPiecesCopy, popping 
    while (ourPiecesCopy)
    {
        int square = popLSB(ourPiecesCopy); // Extract one piece at a time

        // Is current square a pawn?
        if ((pos.whitePawns | pos.blackPawns) & (1ULL << square))
        {
            uint64_t pawnMoves = getPawnMoves(square, color, pos.getOccupiedSquares(), pos);
            while (pawnMoves)
            {
                int targetSquare = popLSB(pawnMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, isCapture, PieceType::PAWN, color, false);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Is current square a knight?
        else if ((pos.whiteKnights | pos.blackKnights) & (1ULL << square))
        {
            uint64_t knightMoves = getKnightMoves(square);
            while (knightMoves)
            {
                int targetSquare = popLSB(knightMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, false, PieceType::KNIGHT, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Bishop moves
        else if ((pos.whiteBishops | pos.blackBishops) & (1ULL << square))
        {
            uint64_t bishopMoves = getBishopMoves(square, pos.getOccupiedSquares());
            while (bishopMoves)
            {
                int targetSquare = popLSB(bishopMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, false, PieceType::BISHOP, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Rook moves
        else if ((pos.whiteRooks | pos.blackRooks) & (1ULL << square))
        {
            uint64_t rookMoves = getRookMoves(square, pos.getOccupiedSquares());
            while (rookMoves)
            {
                int targetSquare = popLSB(rookMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, false, PieceType::ROOK, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Queen moves
        else if ((pos.whiteQueens | pos.blackQueens) & (1ULL << square))
        {
            uint64_t queenMoves = getQueenMoves(square, pos.getOccupiedSquares());
            while (queenMoves)
            {
                int targetSquare = popLSB(queenMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, false, PieceType::QUEEN, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // King moves
        else if ((pos.whiteKing | pos.blackKing) & (1ULL << square))
        {
            uint64_t kingMoves = getKingMoves(square);
            while (kingMoves)
            {
                int targetSquare = popLSB(kingMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare)))
                { // Valid capture or empty square
                    Move move(square, targetSquare, false, PieceType::KING, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move))
                    {
                        moveList.push_back(move);
                    }
                }
            }
        }
    }

    return moveList;
}

uint64_t getPawnMoves(int square, Color color, uint64_t occupied, Position &pos)
{
    uint64_t moves = 0ULL;
    uint64_t pawnBB = 1ULL << square;

    if (color == Color::WHITE)
    {
        uint64_t singlePush = (pawnBB << 8) & ~occupied;
        moves |= singlePush;

        if (pawnBB & RANK_2)
        {
            moves |= ((singlePush << 8) & ~occupied & RANK_4);
        }
        if ((square % 8) != 0)
        {
            moves |= ((pawnBB << 7) & pos.getBlackPieces() & ~FILE_H);
        }
        if ((square % 8) != 7)
        {
            moves |= ((pawnBB << 9) & pos.getBlackPieces() & ~FILE_A);
        }
        if (pos.enPassantSquare != -1 && square >= 24 && square < 32)
        {
            if ((square % 8) != 0 && pos.enPassantSquare == square + 7)
            {
                moves |= (1ULL << pos.enPassantSquare);
            }
            if ((square % 8) != 7 && pos.enPassantSquare == square + 9)
            {
                moves |= (1ULL << pos.enPassantSquare);
            }
        }
    }
    else
    {
        uint64_t singlePush = (pawnBB >> 8) & ~occupied;
        moves |= singlePush;

        if (pawnBB & RANK_7)
        {
            moves |= ((singlePush >> 8) & ~occupied & RANK_5);
        }
        if ((square % 8) != 0)
        {
            moves |= ((pawnBB >> 9) & pos.getWhitePieces() & ~FILE_H);
        }
        if ((square % 8) != 7)
        {
            moves |= ((pawnBB >> 7) & pos.getWhitePieces() & ~FILE_A);
        }
        if (pos.enPassantSquare != -1 && square >= 32 && square < 40)
        {
            if ((square % 8) != 0 && pos.enPassantSquare == square - 9)
            {
                moves |= (1ULL << pos.enPassantSquare);
            }
            if ((square % 8) != 7 && pos.enPassantSquare == square - 7)
            {
                moves |= (1ULL << pos.enPassantSquare);
            }
        }
    }

    return moves;
}

uint64_t getCastlingMoves(Color color, uint64_t occupied, const Position &pos)
{
    uint64_t moves = 0ULL;

    if (color == Color::WHITE)
    {
        if (pos.castlingRights & (1 << 0))
        {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 5) && !getBit(occupied, 6) &&
                !isSquareAttacked(4, Color::BLACK, pos) &&
                !isSquareAttacked(5, Color::BLACK, pos) &&
                !isSquareAttacked(6, Color::BLACK, pos))
            {
                moves |= (1ULL << 6); // g1
            }
        }
        if (pos.castlingRights & (1 << 1))
        {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 1) && !getBit(occupied, 2) && !getBit(occupied, 3) &&
                !isSquareAttacked(4, Color::BLACK, pos) &&
                !isSquareAttacked(3, Color::BLACK, pos) &&
                !isSquareAttacked(2, Color::BLACK, pos))
            {
                moves |= (1ULL << 2); // c1
            }
        }
    }
    else
    {
        if (pos.castlingRights & (1 << 2))
        {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 61) && !getBit(occupied, 62) &&
                !isSquareAttacked(60, Color::WHITE, pos) &&
                !isSquareAttacked(61, Color::WHITE, pos) &&
                !isSquareAttacked(62, Color::WHITE, pos))
            {
                moves |= (1ULL << 62); // g8
            }
        }
        if (pos.castlingRights & (1 << 3))
        {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 57) && !getBit(occupied, 58) && !getBit(occupied, 59) &&
                !isSquareAttacked(60, Color::WHITE, pos) &&
                !isSquareAttacked(59, Color::WHITE, pos) &&
                !isSquareAttacked(58, Color::WHITE, pos))
            {
                moves |= (1ULL << 58); // c8
            }
        }
    }

    return moves;
}

bool isSquareAttacked(int square, Color attackingColor, Position &pos)
{
    uint64_t occupied = pos.getWhitePieces() | pos.getBlackPieces();

    // Check for pawn attacks
    if (attackingColor == Color::WHITE)
    {
        if (((square > 7) && (((square % 8) > 0) && getBit(pos.whitePawns, square - 9))) ||
            ((square % 8) < 7 && getBit(pos.whitePawns, square - 7)))
        {
            return true;
        }
    }
    else
    {
        if (((square < 56 && ((square % 8) > 0) && getBit(pos.blackPawns, square + 7)) ||
             (((square % 8) < 7) && getBit(pos.blackPawns, square + 9))))
        {
            return true;
        }
    }

    // Check for knight attacks
    uint64_t knightAttacks = getKnightMoves(square);
    if (knightAttacks & (attackingColor == Color::WHITE ? pos.whiteKnights : pos.blackKnights))
        return true;

    // Check for bishop/queen attacks
    uint64_t bishopAttacks = getBishopMoves(square, occupied);
    if (bishopAttacks & (attackingColor == Color::WHITE ? (pos.whiteBishops | pos.whiteQueens) : (pos.blackBishops | pos.blackQueens)))
        return true;

    // Check for rook/queen attacks
    uint64_t rookAttacks = getRookMoves(square, occupied);
    if (rookAttacks & (attackingColor == Color::WHITE ? (pos.whiteRooks | pos.whiteQueens) : (pos.blackRooks | pos.blackQueens)))
        return true;

    // Check for king attacks
    uint64_t kingAttacks = getKingMoves(square);
    if (kingAttacks & (attackingColor == Color::WHITE ? pos.whiteKing : pos.blackKing))
        return true;

    return false;
}

std::vector<Move> generateCaptures(const Position &pos, Color color)
{
    std::vector<Move> allMoves = generateMoves(pos, color);
    std::vector<Move> captures;
    captures.reserve(20);

    std::copy_if(allMoves.begin(), allMoves.end(), std::back_inserter(captures),
                 [](const Move &move)
                 { return move.isCapture || move.isPromotion; });

    return captures;
}

bool isInCheck(const Position &pos, Color color)
{
    uint64_t kingBB = (color == Color::WHITE) ? pos.whiteKing : pos.blackKing;
    int kingSquare = __builtin_ctzll(kingBB);
    return isSquareAttacked(kingSquare, color == Color::WHITE ? Color::BLACK : Color::WHITE, pos);
}

bool wouldLeaveKingInCheck(const Position &pos, const Move &move)
{
    const Position tempPos(pos, move);
    return isInCheck(tempPos, move.color); // Check if the move leaves the king in check;
}