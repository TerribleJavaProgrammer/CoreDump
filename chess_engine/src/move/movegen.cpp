#include "move/movegen.h"

std::vector<Move> generateMoves(const Position& pos, Move::Color color) {
    std::vector<Move> moveList;

    uint64_t ourPieces = (color == Move::Color::WHITE) ? pos.whitePieces : pos.blackPieces;
    uint64_t enemyPieces = (color == Move::Color::WHITE) ? pos.blackPieces : pos.whitePieces;
    uint64_t ourPiecesCopy = ourPieces; // Preserve original value for iteration

    while (ourPiecesCopy) {
        int square = popLSB(ourPiecesCopy); // Extract one piece at a time

        // Pawn moves
        if ((pos.whitePawns | pos.blackPawns) & (1ULL << square)) {
            uint64_t pawnMoves = getPawnMoves(square, color, pos.occupiedSquares, pos);
            while (pawnMoves) {
                int targetSquare = popLSB(pawnMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::PAWN, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Knight moves
        else if ((pos.whiteKnights | pos.blackKnights) & (1ULL << square)) {
            uint64_t knightMoves = getKnightMoves(square);
            while (knightMoves) {
                int targetSquare = popLSB(knightMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::KNIGHT, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Bishop moves
        else if ((pos.whiteBishops | pos.blackBishops) & (1ULL << square)) {
            uint64_t bishopMoves = getBishopMoves(square, pos.occupiedSquares);
            while (bishopMoves) {
                int targetSquare = popLSB(bishopMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::BISHOP, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Rook moves
        else if ((pos.whiteRooks | pos.blackRooks) & (1ULL << square)) {
            uint64_t rookMoves = getRookMoves(square, pos.occupiedSquares);
            while (rookMoves) {
                int targetSquare = popLSB(rookMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::ROOK, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // Queen moves
        else if ((pos.whiteQueens | pos.blackQueens) & (1ULL << square)) {
            uint64_t queenMoves = getQueenMoves(square, pos.occupiedSquares);
            while (queenMoves) {
                int targetSquare = popLSB(queenMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::QUEEN, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
        // King moves
        else if ((pos.whiteKing | pos.blackKing) & (1ULL << square)) {
            uint64_t kingMoves = getKingMoves(square);
            while (kingMoves) {
                int targetSquare = popLSB(kingMoves);
                bool isCapture = (enemyPieces & (1ULL << targetSquare)) != 0; // Check for enemy piece
                if (isCapture || !(ourPieces & (1ULL << targetSquare))) { // Valid capture or empty square
                    Move move(square, targetSquare, false, Move::PieceType::KING, color, isCapture);
                    if (!wouldLeaveKingInCheck(pos, move)) {
                        moveList.push_back(move);
                    }
                }
            }
        }
    }

    return moveList;
}

uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied, const Position& pos) {
    uint64_t moves = 0ULL;
    uint64_t pawnBB = 1ULL << square;

    if (color == Move::Color::WHITE) {
        uint64_t singlePush = (pawnBB << 8) & ~occupied;
        moves |= singlePush;
        
        if (pawnBB & RANK_2) {
            moves |= ((singlePush << 8) & ~occupied & RANK_4);
        }
        if ((square % 8) != 0) {
            moves |= ((pawnBB << 7) & pos.blackPieces & ~FILE_H);
        }
        if ((square % 8) != 7) {
            moves |= ((pawnBB << 9) & pos.blackPieces & ~FILE_A);
        }
        if (pos.enPassantSquare != -1 && square >= 24 && square < 32) {
            if ((square % 8) != 0 && pos.enPassantSquare == square + 7) {
                moves |= (1ULL << pos.enPassantSquare);
            }
            if ((square % 8) != 7 && pos.enPassantSquare == square + 9) {
                moves |= (1ULL << pos.enPassantSquare);
            }
        }
    } else {
        uint64_t singlePush = (pawnBB >> 8) & ~occupied;
        moves |= singlePush;
        
        if (pawnBB & RANK_7) {
            moves |= ((singlePush >> 8) & ~occupied & RANK_5);
        }
        if ((square % 8) != 0) {
            moves |= ((pawnBB >> 9) & pos.whitePieces & ~FILE_H);
        }
        if ((square % 8) != 7) {
            moves |= ((pawnBB >> 7) & pos.whitePieces & ~FILE_A);
        }
        if (pos.enPassantSquare != -1 && square >= 32 && square < 40) {
            if ((square % 8) != 0 && pos.enPassantSquare == square - 9) {
                moves |= (1ULL << pos.enPassantSquare);
            }
            if ((square % 8) != 7 && pos.enPassantSquare == square - 7) {
                moves |= (1ULL << pos.enPassantSquare);
            }
        }
    }

    return moves;
}

uint64_t getCastlingMoves(Move::Color color, uint64_t occupied, const Position& pos) {
    uint64_t moves = 0ULL;
    
    if (color == Move::Color::WHITE) {
        if (pos.castlingRights & (1 << 0)) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 5) && !getBit(occupied, 6) &&
                !isSquareAttacked(4, Move::Color::BLACK, pos) &&
                !isSquareAttacked(5, Move::Color::BLACK, pos) &&
                !isSquareAttacked(6, Move::Color::BLACK, pos)) {
                moves |= (1ULL << 6);  // g1
            }
        }
        if (pos.castlingRights & (1 << 1)) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 1) && !getBit(occupied, 2) && !getBit(occupied, 3) &&
                !isSquareAttacked(4, Move::Color::BLACK, pos) &&
                !isSquareAttacked(3, Move::Color::BLACK, pos) &&
                !isSquareAttacked(2, Move::Color::BLACK, pos)) {
                moves |= (1ULL << 2);  // c1
            }
        }
    } else {
        if (pos.castlingRights & (1 << 2)) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 61) && !getBit(occupied, 62) &&
                !isSquareAttacked(60, Move::Color::WHITE, pos) &&
                !isSquareAttacked(61, Move::Color::WHITE, pos) &&
                !isSquareAttacked(62, Move::Color::WHITE, pos)) {
                moves |= (1ULL << 62);  // g8
            }
        }
        if (pos.castlingRights & (1 << 3)) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 57) && !getBit(occupied, 58) && !getBit(occupied, 59) &&
                !isSquareAttacked(60, Move::Color::WHITE, pos) &&
                !isSquareAttacked(59, Move::Color::WHITE, pos) &&
                !isSquareAttacked(58, Move::Color::WHITE, pos)) {
                moves |= (1ULL << 58);  // c8
            }
        }
    }
    
    return moves;
}

bool isSquareAttacked(int square, Move::Color attackingColor, const Position& pos) {
    uint64_t occupied = pos.whitePieces | pos.blackPieces;
    
    // Check for pawn attacks
    if (attackingColor == Move::Color::WHITE) {
        if (((square > 7) && (((square % 8) > 0) && getBit(pos.whitePawns, square - 9))) ||
            ((square % 8) < 7 && getBit(pos.whitePawns, square - 7))) {
            return true;
        }
    } else {
        if (((square < 56 && ((square % 8) > 0) && getBit(pos.blackPawns, square + 7)) ||
            (((square % 8) < 7) && getBit(pos.blackPawns, square + 9)))) {
            return true;
        }
    }

    // Check for knight attacks
    uint64_t knightAttacks = getKnightMoves(square);
    if (knightAttacks & (attackingColor == Move::Color::WHITE ? 
        pos.whiteKnights : pos.blackKnights))
        return true;

    // Check for bishop/queen attacks
    uint64_t bishopAttacks = getBishopMoves(square, occupied);
    if (bishopAttacks & (attackingColor == Move::Color::WHITE ? 
        (pos.whiteBishops | pos.whiteQueens) : (pos.blackBishops | pos.blackQueens)))
        return true;

    // Check for rook/queen attacks
    uint64_t rookAttacks = getRookMoves(square, occupied);
    if (rookAttacks & (attackingColor == Move::Color::WHITE ? 
        (pos.whiteRooks | pos.whiteQueens) : (pos.blackRooks | pos.blackQueens)))
        return true;

    // Check for king attacks
    uint64_t kingAttacks = getKingMoves(square);
    if (kingAttacks & (attackingColor == Move::Color::WHITE ? 
        pos.whiteKing : pos.blackKing))
        return true;

    return false;
}

std::vector<Move> generateCaptures(const Position& pos, Move::Color color) {
    std::vector<Move> allMoves = generateMoves(pos, color);
    std::vector<Move> captures;
    captures.reserve(20); 

    std::copy_if(allMoves.begin(), allMoves.end(), std::back_inserter(captures),
                 [](const Move& move) { return move.isCapture || move.isPromotion; });

    return captures;
}


bool isInCheck(const Position& pos, Move::Color color) {
    uint64_t kingBB = (color == Move::Color::WHITE) ? pos.whiteKing : pos.blackKing;
    int kingSquare = __builtin_ctzll(kingBB);
    return isSquareAttacked(kingSquare, color == Move::Color::WHITE ? 
        Move::Color::BLACK : Move::Color::WHITE, pos);
}

bool wouldLeaveKingInCheck(const Position& pos, const Move& move) {
    Position tempPos = pos;
    makeMove(tempPos, move);
    bool inCheck = isInCheck(tempPos, move.color);  // Check if the move leaves the king in check
    undoMove(tempPos, move);
    return inCheck;
}