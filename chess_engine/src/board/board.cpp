#include "board/board.h"

Position currPosition;

void initializeBoard() {
    currPosition.initializePosition();
}

Position getBoard() {
    return currPosition;
}

void makeMove(Position& pos, const Move& move) {
    uint64_t fromBB = 1ULL << move.fromSquare;
    uint64_t toBB = 1ULL << move.toSquare;
    bool isWhite = (move.color == Move::Color::WHITE);

    // Handle captures
    if (move.isCapture) {
        uint64_t captureBB = 1ULL << move.toSquare;
        if (isWhite) {
            if (pos.blackPawns & captureBB) { pos.blackPawns &= ~captureBB; }
            else if (pos.blackKnights & captureBB) { pos.blackKnights &= ~captureBB; }
            else if (pos.blackBishops & captureBB) { pos.blackBishops &= ~captureBB; }
            else if (pos.blackRooks & captureBB) { pos.blackRooks &= ~captureBB; }
            else if (pos.blackQueens & captureBB) { pos.blackQueens &= ~captureBB; }
            else if (pos.blackKing & captureBB) { pos.blackKing &= ~captureBB; }
        } else {
            if (pos.whitePawns & captureBB) { pos.whitePawns &= ~captureBB; }
            else if (pos.whiteKnights & captureBB) { pos.whiteKnights &= ~captureBB; }
            else if (pos.whiteBishops & captureBB) { pos.whiteBishops &= ~captureBB; }
            else if (pos.whiteRooks & captureBB) { pos.whiteRooks &= ~captureBB; }
            else if (pos.whiteQueens & captureBB) { pos.whiteQueens &= ~captureBB; }
            else if (pos.whiteKing & captureBB) { pos.whiteKing &= ~captureBB; }
        }
    }

    // Remove piece from its original square and place it in the new one
    if (isWhite) {
        if (pos.whitePawns & fromBB) { pos.whitePawns &= ~fromBB; pos.whitePawns |= toBB; }
        else if (pos.whiteKnights & fromBB) { pos.whiteKnights &= ~fromBB; pos.whiteKnights |= toBB; }
        else if (pos.whiteBishops & fromBB) { pos.whiteBishops &= ~fromBB; pos.whiteBishops |= toBB; }
        else if (pos.whiteRooks & fromBB) { pos.whiteRooks &= ~fromBB; pos.whiteRooks |= toBB; }
        else if (pos.whiteQueens & fromBB) { pos.whiteQueens &= ~fromBB; pos.whiteQueens |= toBB; }
        else if (pos.whiteKing & fromBB) { pos.whiteKing &= ~fromBB; pos.whiteKing |= toBB; }
    } else {
        if (pos.blackPawns & fromBB) { pos.blackPawns &= ~fromBB; pos.blackPawns |= toBB; }
        else if (pos.blackKnights & fromBB) { pos.blackKnights &= ~fromBB; pos.blackKnights |= toBB; }
        else if (pos.blackBishops & fromBB) { pos.blackBishops &= ~fromBB; pos.blackBishops |= toBB; }
        else if (pos.blackRooks & fromBB) { pos.blackRooks &= ~fromBB; pos.blackRooks |= toBB; }
        else if (pos.blackQueens & fromBB) { pos.blackQueens &= ~fromBB; pos.blackQueens |= toBB; }
        else if (pos.blackKing & fromBB) { pos.blackKing &= ~fromBB; pos.blackKing |= toBB; }
    }

    // Handle castling (move the rook)
    if (move.isCastling) {
        if (move.castlingType == Move::CastlingType::KINGSIDE) {
            if (isWhite) { pos.whiteRooks &= ~(1ULL << 7); pos.whiteRooks |= (1ULL << 5); }
            else { pos.blackRooks &= ~(1ULL << 63); pos.blackRooks |= (1ULL << 61); }
        } else { // Queenside
            if (isWhite) { pos.whiteRooks &= ~(1ULL << 0); pos.whiteRooks |= (1ULL << 3); }
            else { pos.blackRooks &= ~(1ULL << 56); pos.blackRooks |= (1ULL << 59); }
        }
    }

    // Handle En Passant
    if (move.toSquare == move.prevEnPassantSquare) {
        if (isWhite) { pos.blackPawns &= ~(1ULL << (move.toSquare - 8)); }
        else { pos.whitePawns &= ~(1ULL << (move.toSquare + 8)); }
    }

    // Handle Promotion
    if (move.isPromotion) {
        if (isWhite) { pos.whiteQueens |= toBB; pos.whitePawns &= ~toBB; }
        else { pos.blackQueens |= toBB; pos.blackPawns &= ~toBB; }
    }

    // Update composite bitboards
    pos.whitePieces = pos.whitePawns | pos.whiteKnights | pos.whiteBishops | pos.whiteRooks | pos.whiteQueens | pos.whiteKing;
    pos.blackPieces = pos.blackPawns | pos.blackKnights | pos.blackBishops | pos.blackRooks | pos.blackQueens | pos.blackKing;
    pos.occupiedSquares = pos.whitePieces | pos.blackPieces;
    pos.emptySquares = ~pos.occupiedSquares;
}

void undoMove(Position& pos, const Move& move) {
    uint64_t fromBB = 1ULL << move.fromSquare;
    uint64_t toBB = 1ULL << move.toSquare;
    bool isWhite = (move.color == Move::Color::WHITE);

    // Move piece back to its original square
    if (isWhite) {
        if (pos.whitePawns & toBB) { pos.whitePawns &= ~toBB; pos.whitePawns |= fromBB; }
        else if (pos.whiteKnights & toBB) { pos.whiteKnights &= ~toBB; pos.whiteKnights |= fromBB; }
        else if (pos.whiteBishops & toBB) { pos.whiteBishops &= ~toBB; pos.whiteBishops |= fromBB; }
        else if (pos.whiteRooks & toBB) { pos.whiteRooks &= ~toBB; pos.whiteRooks |= fromBB; }
        else if (pos.whiteQueens & toBB) { pos.whiteQueens &= ~toBB; pos.whiteQueens |= fromBB; }
        else if (pos.whiteKing & toBB) { pos.whiteKing &= ~toBB; pos.whiteKing |= fromBB; }
    } else {
        if (pos.blackPawns & toBB) { pos.blackPawns &= ~toBB; pos.blackPawns |= fromBB; }
        else if (pos.blackKnights & toBB) { pos.blackKnights &= ~toBB; pos.blackKnights |= fromBB; }
        else if (pos.blackBishops & toBB) { pos.blackBishops &= ~toBB; pos.blackBishops |= fromBB; }
        else if (pos.blackRooks & toBB) { pos.blackRooks &= ~toBB; pos.blackRooks |= fromBB; }
        else if (pos.blackQueens & toBB) { pos.blackQueens &= ~toBB; pos.blackQueens |= fromBB; }
        else if (pos.blackKing & toBB) { pos.blackKing &= ~toBB; pos.blackKing |= fromBB; }
    }

    // Restore captured piece
    if (move.isCapture) {
        uint64_t captureBB = 1ULL << move.toSquare;
        if (isWhite) {
            if (pos.blackPawns & captureBB) { pos.blackPawns |= captureBB; }
            else if (pos.blackKnights & captureBB) { pos.blackKnights |= captureBB; }
            else if (pos.blackBishops & captureBB) { pos.blackBishops |= captureBB; }
            else if (pos.blackRooks & captureBB) { pos.blackRooks |= captureBB; }
            else if (pos.blackQueens & captureBB) { pos.blackQueens |= captureBB; }
            else if (pos.blackKing & captureBB) { pos.blackKing |= captureBB; }
        } else {
            if (pos.whitePawns & captureBB) { pos.whitePawns |= captureBB; }
            else if (pos.whiteKnights & captureBB) { pos.whiteKnights |= captureBB; }
            else if (pos.whiteBishops & captureBB) { pos.whiteBishops |= captureBB; }
            else if (pos.whiteRooks & captureBB) { pos.whiteRooks |= captureBB; }
            else if (pos.whiteQueens & captureBB) { pos.whiteQueens |= captureBB; }
            else if (pos.whiteKing & captureBB) { pos.whiteKing |= captureBB; }
        }
    }

    // Undo Castling
    if (move.isCastling) {
        if (move.castlingType == Move::CastlingType::KINGSIDE) {
            if (isWhite) { pos.whiteRooks |= (1ULL << 7); pos.whiteRooks &= ~(1ULL << 5); }
            else { pos.blackRooks |= (1ULL << 63); pos.blackRooks &= ~(1ULL << 61); }
        } else { // Queenside
            if (isWhite) { pos.whiteRooks |= (1ULL << 0); pos.whiteRooks &= ~(1ULL << 3); }
            else { pos.blackRooks |= (1ULL << 56); pos.blackRooks &= ~(1ULL << 59); }
        }
    }

    // Undo En Passant
    if (move.fromSquare == move.prevEnPassantSquare) {
        if (isWhite) { pos.blackPawns |= (1ULL << (move.toSquare - 8)); }
        else { pos.whitePawns |= (1ULL << (move.toSquare + 8)); }
    }

    // Undo Promotion
    if (move.isPromotion) {
        if (isWhite) { pos.whiteQueens &= ~toBB; pos.whitePawns |= fromBB; }
        else { pos.blackQueens &= ~toBB; pos.blackPawns |= fromBB; }
    }

    // Update bitboards
    pos.whitePieces = pos.whitePawns | pos.whiteKnights | pos.whiteBishops | pos.whiteRooks | pos.whiteQueens | pos.whiteKing;
    pos.blackPieces = pos.blackPawns | pos.blackKnights | pos.blackBishops | pos.blackRooks | pos.blackQueens | pos.blackKing;
    pos.occupiedSquares = pos.whitePieces | pos.blackPieces;
    pos.emptySquares = ~pos.occupiedSquares;
}