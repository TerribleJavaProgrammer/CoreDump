#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>
#include <algorithm>
#include "board/bitboard.h"
#include "board/position.h"
#include "move/move.h"
#include "board/magic/magicentry.h"
#include "board/magic/magicbitboard.h"
#include "board/board.h"

// Bitboard constants for file and rank masks
constexpr uint64_t FILE_A = 0x0101010101010101ULL;  // Leftmost file
constexpr uint64_t FILE_H = 0x8080808080808080ULL;  // Rightmost file

// Bitboard constants for specific ranks
constexpr uint64_t RANK_1 = 0x00000000000000FFULL;  // Bottom rank (white's back rank)
constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;  // White pawn starting rank
constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;  // White pawn double push target rank
constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;  // Black pawn double push target rank
constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;  // Black pawn starting rank
constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;  // Top rank (black's back rank)

std::vector<Move> generateMoves(const Position& pos, Move::Color color); // Generatse all legal moves
uint64_t getRookMoves(int square, uint64_t occupied);    // Gets legal rook moves
uint64_t getBishopMoves(int square, uint64_t occupied);  // Gets legal bishop moves
uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied, const Position& pos);  // Gets pawn moves
uint64_t getKnightMoves(int square);              // Gets legal knight moves
uint64_t getKingMoves(int square);                // Gets legal king moves
uint64_t getQueenMoves(int square, uint64_t occupied);   // Gets legal queen moves
uint64_t getCastlingMoves(Move::Color color, uint64_t occupied, const Position& pos); // Gets legal castling moves
bool isSquareAttacked(int square, Move::Color attackingColor, const Position& pos); 
std::vector<Move> generateCaptures(const Position& pos, Move::Color color); // Generates all legal captures
bool isInCheck(const Position& pos, Move::Color color); // Returns whether the color passed in is in check

#endif // MOVEGEN_H