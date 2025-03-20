/*
* Chess engine using bitboards and magic bitboard move generation
* Created on: 3-6-25
* Features: 
* - Bitboard representation
* - Magic bitboard move generation
* - Negamax search with alpha-beta pruning
* - Position evaluation with piece-square tables
* - Legal move generation with castling and promotions
*/

// Required header files for various functionalities
#include <stdlib.h>      // Standard library functions
#include <iostream>      // Input/output operations
#include <vector>        // Dynamic arrays
#include <string>        // String handling
#include <map>           // Key-value associations
#include <algorithm>     // Algorithms like sort
#include <cmath>         // Mathematical functions
#include <memory>        // Smart pointers
#include <thread>        // Threading support
#include <future>        // Asynchronous operations
#include <mutex>         // Thread synchronization
#include <chrono>        // Time-related functions
#include <array>         // Fixed-size arrays
#include <random>        // Random number generation
#include <bitset>        // Bit manipulation
#include <unordered_map> // Hash tables

// Pre-calculated magic numbers for rook move generation
// These numbers, when multiplied with blocker configurations, produce unique indices for move lookup
constexpr std::array<uint64_t, 64> ROOK_MAGICS = {
    0x0080001020400080ULL, 0x0040001000200040ULL, 0x0080081000200080ULL, 0x0080040800100080ULL,
    0x0080020400080080ULL, 0x0080010200040080ULL, 0x0080008001000200ULL, 0x0080002040800100ULL,
    0x0000800020400080ULL, 0x0000400020005000ULL, 0x0000801000200080ULL, 0x0000800800100080ULL,
    0x0000800400080080ULL, 0x0000800200040080ULL, 0x0000800100020080ULL, 0x0000800040800100ULL,
    0x0000208000400080ULL, 0x0000404000201000ULL, 0x0000808010002000ULL, 0x0000808008001000ULL,
    0x0000808004000800ULL, 0x0000808002000400ULL, 0x0000010100020004ULL, 0x0000020000408104ULL,
    0x0000208080004000ULL, 0x0000200040005000ULL, 0x0000100080200080ULL, 0x0000080080100080ULL,
    0x0000040080080080ULL, 0x0000020080040080ULL, 0x0000010080800200ULL, 0x0000800080004100ULL,
    0x0000204000800080ULL, 0x0000200040401000ULL, 0x0000100080802000ULL, 0x0000080080801000ULL,
    0x0000040080800800ULL, 0x0000020080800400ULL, 0x0000020001010004ULL, 0x0000800040800100ULL,
    0x0000204000808000ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
    0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000010002008080ULL, 0x0000004081020004ULL,
    0x0000204000800080ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
    0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000800100020080ULL, 0x0000800041000080ULL,
    0x00FFFCDDFCED714AULL, 0x007FFCDDFCED714AULL, 0x003FFFCDFFD88096ULL, 0x0000040810002101ULL,
    0x0001000204080011ULL, 0x0001000204000801ULL, 0x0001000082000401ULL, 0x0001FFFAABFAD1A2ULL
};

// Pre-calculated magic numbers for bishop move generation
// Similar to rook magics but for diagonal movements
constexpr std::array<uint64_t, 64> BISHOP_MAGICS = {
    0x0002020202020200ULL, 0x0002020202020000ULL, 0x0004010202000000ULL, 0x0004040080000000ULL,
    0x0001104000000000ULL, 0x0000821040000000ULL, 0x0000410410400000ULL, 0x0000104104104000ULL,
    0x0000040404040400ULL, 0x0000020202020200ULL, 0x0000040102020000ULL, 0x0000040400800000ULL,
    0x0000011040000000ULL, 0x0000008210400000ULL, 0x0000004104104000ULL, 0x0000002082082000ULL,
    0x0004000808080800ULL, 0x0002000404040400ULL, 0x0001000202020200ULL, 0x0000800802004000ULL,
    0x0000800400A00000ULL, 0x0000200100884000ULL, 0x0000400082082000ULL, 0x0000200041041000ULL,
    0x0002080010101000ULL, 0x0001040008080800ULL, 0x0000208004010400ULL, 0x0000404004010200ULL,
    0x0000840000802000ULL, 0x0000404002011000ULL, 0x0000808001041000ULL, 0x0000404000820800ULL,
    0x0001041000202000ULL, 0x0000820800101000ULL, 0x0000104400080800ULL, 0x0000020080080080ULL,
    0x0000404040040100ULL, 0x0000808100020100ULL, 0x0001010100020800ULL, 0x0000808080010400ULL,
    0x0000820820004000ULL, 0x0000410410002000ULL, 0x0000082088001000ULL, 0x0000002011000800ULL,
    0x0000080100400400ULL, 0x0001010101000200ULL, 0x0002020202000400ULL, 0x0001010101000200ULL,
    0x0000410410400000ULL, 0x0000208208200000ULL, 0x0000002084100000ULL, 0x0000000020880000ULL,
    0x0000001002020000ULL, 0x0000040408020000ULL, 0x0004040404040000ULL, 0x0002020202020000ULL,
    0x0000104104104000ULL, 0x0000002082082000ULL, 0x0000000020841000ULL, 0x0000000000208800ULL,
    0x0000000010020200ULL, 0x0000000404080200ULL, 0x0000040404040400ULL, 0x0002020202020200ULL
};

// Pre-calculated knight attack patterns for each square
// Each number represents possible knight moves from that square
constexpr uint64_t KNIGHT_ATTACKS[64] = {
    0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000a1100ULL, 0x0000000000142200ULL,
    0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000a01000ULL, 0x0000000000402000ULL,
    0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000a110011ULL, 0x0000000014220022ULL,
    0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000a0100010ULL, 0x0000000040200020ULL,
    0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000a1100110aULL, 0x0000001422002214ULL,
    0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000a0100010a0ULL, 0x0000004020002040ULL,
    0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000a1100110a00ULL, 0x0000142200221400ULL,
    0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000a0100010a000ULL, 0x0000402000204000ULL,
    0x0002040004020000ULL, 0x0005080008050000ULL, 0x000a1100110a0000ULL, 0x0014220022140000ULL,
    0x0028440044280000ULL, 0x0050880088500000ULL, 0x00a0100010a00000ULL, 0x0040200020400000ULL,
    0x0204000402000000ULL, 0x0508000805000000ULL, 0x0a1100110a000000ULL, 0x1422002214000000ULL,
    0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL,
    0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL,
    0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL,
    0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110a0000000000ULL, 0x0022140000000000ULL,
    0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010a00000000000ULL, 0x0020400000000000ULL
};

// Pre-calculated king attack patterns for each square
// Each number represents possible king moves from that square
constexpr uint64_t KING_ATTACKS[64] = {
    0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL, 0x0000000000001C14ULL,
    0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000E0A0ULL, 0x000000000000C040ULL,
    0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
    0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000E0A0E0ULL, 0x0000000000C040C0ULL,
    0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000E0A0E00ULL, 0x000000001C141C00ULL,
    0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000E0A0E000ULL, 0x00000000C040C000ULL,
    0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000E0A0E0000ULL, 0x0000001C141C0000ULL,
    0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000E0A0E00000ULL, 0x000000C040C00000ULL,
    0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000E0A0E000000ULL, 0x00001C141C000000ULL,
    0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000E0A0E0000000ULL, 0x0000C040C0000000ULL,
    0x0003020300000000ULL, 0x0007050700000000ULL, 0x000E0A0E00000000ULL, 0x001C141C00000000ULL,
    0x0038283800000000ULL, 0x0070507000000000ULL, 0x00E0A0E000000000ULL, 0x00C040C000000000ULL,
    0x0302030000000000ULL, 0x0705070000000000ULL, 0x0E0A0E0000000000ULL, 0x1C141C0000000000ULL,
    0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
    0x0203000000000000ULL, 0x0507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL,
    0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL
};

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

// Constants for chess board size and evaluation
constexpr int BOARD_SIZE = 64;  // Total number of squares on the chess board

// Material values for each piece type (in centipawns)
constexpr int PAWN_VALUE = 100;     // Base value for pawns
constexpr int KNIGHT_VALUE = 300;    // Base value for knights
constexpr int BISHOP_VALUE = 330;    // Base value for bishops
constexpr int ROOK_VALUE = 500;      // Base value for rooks
constexpr int QUEEN_VALUE = 900;     // Base value for queens
constexpr int KING_VALUE = 20000;    // Base value for king (very high to prioritize king safety)

// Piece-Square Tables (PST) for positional evaluation
// Higher values indicate better squares for that piece
// Tables are oriented from White's perspective (a1 is bottom-left)

// Pawn PST - Encourages pawns to:
// - Advance towards promotion
// - Control the center
// - Maintain pawn structure
constexpr std::array<int, 64> PAWN_PST = {
    0,  0,  0,  0,  0,  0,  0,  0,     // 8th rank
    50, 50, 50, 50, 50, 50, 50, 50,    // 7th rank (promotion potential)
    10, 10, 20, 30, 30, 20, 10, 10,    // 6th rank
    5,  5, 10, 25, 25, 10,  5,  5,     // 5th rank
    0,  0,  0, 20, 20,  0,  0,  0,     // 4th rank
    5, -5,-10,  0,  0,-10, -5,  5,     // 3rd rank
    5, 10, 10,-20,-20, 10, 10,  5,     // 2nd rank
    0,  0,  0,  0,  0,  0,  0,  0      // 1st rank
};

// Knight PST - Encourages knights to:
// - Occupy central squares
// - Avoid edge squares
constexpr std::array<int, 64> KNIGHT_PST = {
    -50,-40,-30,-30,-30,-30,-40,-50,   // Edge penalties
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,   // Center bonuses
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

// Bishop PST - Encourages bishops to:
// - Control long diagonals
// - Support center control
// - Stay active
constexpr std::array<int, 64> BISHOP_PST = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

// Rook PST - Encourages rooks to:
// - Control open files
// - Move to 7th rank
// - Support from behind pawns
constexpr std::array<int, 64> ROOK_PST = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,    // 7th rank bonus
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0     // Slight bonus for central files
};

// Queen PST - Encourages queens to:
// - Stay protected early game
// - Control center when active
// - Avoid early development
constexpr std::array<int, 64> QUEEN_PST = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// King PST - Encourages kings to:
// - Stay protected behind pawn shield
// - Castle early
// - Avoid center in middlegame
constexpr std::array<int, 64> KING_PST = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,   // Castled position bonus
    20, 30, 10,  0,  0, 10, 30, 20    // Corner protection bonus
};

// **BITBOARDS**
// Each bitboard is a 64-bit integer where each bit represents a square on the chess board
// 1 indicates presence of a piece, 0 indicates empty square

// White piece bitboards - track positions of all white pieces by type
uint64_t whitePawns;    // Represents all white pawns on the board
uint64_t whiteKnights;  // Represents all white knights on the board
uint64_t whiteBishops;  // Represents all white bishops on the board
uint64_t whiteRooks;    // Represents all white rooks on the board
uint64_t whiteQueens;   // Represents all white queens on the board
uint64_t whiteKing;     // Represents the white king's position

// Black piece bitboards - track positions of all black pieces by type
uint64_t blackPawns;    // Represents all black pawns on the board
uint64_t blackKnights;  // Represents all black knights on the board
uint64_t blackBishops;  // Represents all black bishops on the board
uint64_t blackRooks;    // Represents all black rooks on the board
uint64_t blackQueens;   // Represents all black queens on the board
uint64_t blackKing;     // Represents the black king's position

// Composite bitboards - derived from individual piece bitboards
uint64_t occupiedSquares;  // All squares occupied by any piece (white | black)
uint64_t whitePieces;      // All squares occupied by white pieces
uint64_t blackPieces;      // All squares occupied by black pieces
uint64_t emptySquares;     // All empty squares (~occupiedSquares)

// **STRUCTS**
// Structure to represent a chess move
struct Move {
    int fromSquare;     // Source square (0-63)
    int toSquare;       // Destination square (0-63)
    bool isCapture;     // True if this move captures a piece
    
    // Enum for piece types
    enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
    PieceType pieceType;  // Type of piece being moved
    
    // Enum for piece colors
    enum class Color { WHITE, BLACK };
    Color color;        // Color of the piece being moved
    
    bool isCastling;    // True if this is a castling move
    // Enum for castling types
    enum class CastlingType { NONE, KINGSIDE, QUEENSIDE };
    CastlingType castlingType;  // Type of castling move
    
    bool isPromotion;          // True if this is a pawn promotion
    PieceType promotionPiece;  // Piece type to promote to
};

// Structure for magic bitboard lookup entries
struct MagicEntry {
    uint64_t mask;               // Mask for relevant occupancy bits
    uint64_t magic;             // Magic number for this square
    int shift;                  // Right shift amount
    std::vector<uint64_t> attacks;  // Pre-calculated attack patterns
};

// Structure to track castling rights
struct CastlingRights {
    bool whiteKingside = true;   // Can white castle kingside?
    bool whiteQueenside = true;  // Can white castle queenside?
    bool blackKingside = true;   // Can black castle kingside?
    bool blackQueenside = true;  // Can black castle queenside?
};

// Structure to store a complete position
struct Position {
    // Individual piece bitboards
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    // Composite bitboards
    uint64_t whitePieces, blackPieces, occupiedSquares, emptySquares;
    // Castling rights for this position
    CastlingRights castling;
    // En passant
    int enPassantSquare;  // -1 if no en passant possible, otherwise square number
};

// Structure to store a search result
struct SearchResult {
    int score;
    Move bestMove;
    
    SearchResult(int s, Move m) : score(s), bestMove(m) {}
};

// Global castling rights state
CastlingRights castlingRights;

// Initialize magic lookup tables for efficient move generation
std::array<MagicEntry, 64> rookTable;     // Stores pre-calculated rook move patterns
std::array<MagicEntry, 64> bishopTable;   // Stores pre-calculated bishop move patterns

// **BITBOARD MANIPULATION FUNCTIONS**
uint64_t setBit(uint64_t board, int square);      // Sets a bit to 1 at specified square
uint64_t clearBit(uint64_t board, int square);    // Sets a bit to 0 at specified square
bool getBit(uint64_t board, int square);          // Returns the bit value at specified square

// **BOARD STATE MANAGEMENT**
void initializeBitboards();                       // Sets up initial chess position
void updateBitboards(const Move& move);           // Updates board state after a move
void printBitBoard(uint64_t board);               // Debugging: prints binary representation
void displayBoard();                              // Prints human-readable board
void displayLegalMoves(int square, Move::Color color);  // Shows legal moves for a piece

// **MOVE GENERATION HELPERS**
uint64_t generateRookMask(int square);            // Creates attack mask for rooks
uint64_t generateBishopMask(int square);          // Creates attack mask for bishops
uint64_t generateRookAttacks(int square, uint64_t blockers);    // Gets rook attacks with blockers
uint64_t generateBishopAttacks(int square, uint64_t blockers);  // Gets bishop attacks with blockers

// **MAGIC BITBOARD INITIALIZATION**
void initializeMagicBitboards();                  // Sets up magic move lookup tables

// **PIECE MOVE GENERATION**
uint64_t getRookMoves(int square, uint64_t occupied);    // Gets legal rook moves
uint64_t getBishopMoves(int square, uint64_t occupied);  // Gets legal bishop moves
uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied);  // Gets pawn moves
uint64_t getKnightMoves(int square);              // Gets legal knight moves
uint64_t getKingMoves(int square);                // Gets legal king moves
uint64_t getQueenMoves(int square, uint64_t occupied);   // Gets legal queen moves

// LOCALS
uint64_t getPawnMovesLocal(int square, Move::Color color, uint64_t occupied, const Position& pos);
bool isSquareAttackedLocal(int square, Move::Color attackingColor, const Position& pos);
bool isInCheckLocal(const Position& pos, Move::Color color);
uint64_t getCastlingMovesLocal(Move::Color color, uint64_t occupied, const Position& pos);
int evaluatePositionLocal(const Position& pos);
std::vector<Move> generateMovesLocal(const Position& pos, Move::Color color);

// **GAME STATE CHECKING**
bool isSquareAttacked(int square, Move::Color attackingColor);  // Checks if square is under attack
bool isInCheck(Move::Color color);                // Checks if specified color is in check
uint64_t getCastlingMoves(Move::Color color, uint64_t occupied);  // Gets legal castling moves

// **POSITION EVALUATION AND SEARCH**
int evaluatePosition();                           // Evaluates current position in centipawns
std::vector<Move> generateMoves(const Position& pos, Move::Color color);  // Generates all legal moves
void sortMoves(std::vector<Move>& moves);         // Sorts moves for better pruning (MVV/LVA)
Move findBestMove(Move::Color color, int depth);   // Finds best move using negamax search

// **ATTACK TABLE INITIALIZATION**
std::array<int, 64> rookShifts;      // Shift amounts for rook magic numbers
std::array<int, 64> bishopShifts;    // Shift amounts for bishop magic numbers

// **PRE-CALCULATED ATTACK TABLES**
std::vector<uint64_t> rookAttackTable;    // Lookup table for rook attacks
std::vector<uint64_t> bishopAttackTable;  // Lookup table for bishop attacks

// Default value for en passant
int enPassantSquare = -1;
const int R = 2;  // Null move reduction depth
const int NULL_MOVE_MARGIN = 300;  // Safety margin for null move pruning

// Constants for futility pruning
const int FUTILITY_MARGIN = 100;      // 1 pawn
const int EXTENDED_FUTILITY_MARGIN = 300;  // 3 pawns

const int NUM_THREADS = 4;  // Number of threads to use
std::mutex searchMutex;     // Mutex for thread synchronization

// Saves the current board state into a Position struct
Position saveBitboards() {
    std::lock_guard<std::mutex> lock(searchMutex);
    Position pos;
    
    // Save all piece bitboards
    pos.whitePawns = whitePawns;
    pos.whiteKnights = whiteKnights;
    pos.whiteBishops = whiteBishops;
    pos.whiteRooks = whiteRooks;
    pos.whiteQueens = whiteQueens;
    pos.whiteKing = whiteKing;
    
    pos.blackPawns = blackPawns;
    pos.blackKnights = blackKnights;
    pos.blackBishops = blackBishops;
    pos.blackRooks = blackRooks;
    pos.blackQueens = blackQueens;
    pos.blackKing = blackKing;
    
    // Save composite bitboards
    pos.whitePieces = whitePieces;
    pos.blackPieces = blackPieces;
    pos.occupiedSquares = occupiedSquares;
    pos.emptySquares = emptySquares;
    
    // Save castling rights
    pos.castling = castlingRights;
    // Save en passant square
    pos.enPassantSquare = enPassantSquare;

    return pos;
}

// main function; holds game loop
int main() {
    // Initialize the game state
    initializeBitboards();        // Set up initial piece positions
    initializeMagicBitboards();   // Pre-calculate move lookup tables
    bool gameOver = false;
    Move::Color currentPlayer = Move::Color::WHITE;  // White moves first
    bool playerIsWhite = true;    // Default: human plays white

    // Let player choose their color
    std::cout << "Play as (w)hite or (b)lack? ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    playerIsWhite = (tolower(choice) != 'b');

    // Main game loop
    while (!gameOver) {
        // Display current game state
        std::cout << "\n====================\n";
        std::cout << (currentPlayer == Move::Color::WHITE ? "White" : "Black") << " to move\n";
        if (isInCheck(currentPlayer)) {
            std::cout << "CHECK!\n";
        }
        displayBoard();

        // Determine if it's player's turn
        bool isPlayerTurn = (currentPlayer == Move::Color::WHITE) == playerIsWhite;

        if (isPlayerTurn) {
            // Human player's turn
            std::cout << "\nCommands:\n";
            std::cout << "- Move: e2 e4\n";
            std::cout << "- Show moves: moves e2\n";
            std::cout << "- Quit: quit\n> ";
            
            std::string input;
            std::getline(std::cin, input);

            // Handle quit command
            if (input == "quit") {
                break;
            }

            // Handle show moves command
            if (input.substr(0, 5) == "moves") {
                if (input.length() != 8) {
                    std::cout << "Invalid format. Use 'moves e2'\n";
                    continue;
                }
                
                // Convert algebraic notation to square index
                int file = input[6] - 'a';
                int rank = input[7] - '1';
                
                if (file < 0 || file > 7 || rank < 0 || rank > 7) {
                    std::cout << "Invalid square.\n";
                    continue;
                }

                int square = rank * 8 + file;
                displayLegalMoves(square, currentPlayer);
                continue;
            }

            // Handle move command
            if (input.length() != 5 || input[2] != ' ') {
                std::cout << "Invalid move format. Use 'e2 e4'\n";
                continue;
            }

            // Create and validate move
            Move move;
            move.fromSquare = (input[1] - '1') * 8 + (input[0] - 'a');
            move.toSquare = (input[4] - '1') * 8 + (input[3] - 'a');
            move.color = currentPlayer;

            // Check if move is legal
            std::vector<Move> legalMoves = generateMoves(saveBitboards(), currentPlayer);
            auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
                [&move](const Move& m) {
                    return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
                });

            if (moveIt == legalMoves.end()) {
                std::cout << "Illegal move.\n";
                continue;
            }

            // Copy all properties from legal move
            move = *moveIt;

            // Handle pawn promotion
            if (move.isPromotion) {
                std::cout << "Choose promotion piece (Q/R/B/N): ";
                char choice;
                std::cin >> choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                switch (toupper(choice)) {
                    case 'Q': move.promotionPiece = Move::PieceType::QUEEN; break;
                    case 'R': move.promotionPiece = Move::PieceType::ROOK; break;
                    case 'B': move.promotionPiece = Move::PieceType::BISHOP; break;
                    case 'N': move.promotionPiece = Move::PieceType::KNIGHT; break;
                    default: move.promotionPiece = Move::PieceType::QUEEN; break;
                }
            }

            updateBitboards(move);
        } else {
            // Computer's turn
            std::cout << "Computer is thinking...\n";
            Move computerMove = findBestMove(currentPlayer, 4); // Search 4 ply deep
            
            // Convert move to algebraic notation for display
            std::string moveStr = 
                std::string(1, 'a' + (computerMove.fromSquare % 8)) +
                std::string(1, '1' + (computerMove.fromSquare / 8)) + " " +
                std::string(1, 'a' + (computerMove.toSquare % 8)) +
                std::string(1, '1' + (computerMove.toSquare / 8));
                
            std::cout << "Computer plays: " << moveStr << std::endl;
            
            updateBitboards(computerMove);
        }

        // Check for game end conditions
        std::vector<Move> nextMoves = generateMoves(saveBitboards(), currentPlayer);
        if (nextMoves.empty()) {
            if (isInCheck(currentPlayer)) {
                std::cout << "\nCheckmate! " 
                         << (currentPlayer == Move::Color::WHITE ? "Black" : "White") 
                         << " wins!\n";
            } else {
                std::cout << "\nStalemate! Game is a draw.\n";
            }
            gameOver = true;
            continue;
        }

        // Switch to next player
        currentPlayer = (currentPlayer == Move::Color::WHITE) ? 
            Move::Color::BLACK : Move::Color::WHITE;
    }

    std::cout << "\nGame ended.\n";
    return 0;
}

// helper functions
// Displays the current chess board state in a human-readable format
// Uses Unicode chess pieces and coordinate system (a-h, 1-8)
void displayBoard() {
    // Loop through ranks from top (8) to bottom (1)
    for (int rank = 7; rank >= 0; --rank) {
        // Print rank number
        std::cout << (rank + 1) << " ";
        
        // Loop through files from left (a) to right (h)
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;  // Convert rank/file to square index
            
            // Check each piece type and print corresponding Unicode symbol
            if (getBit(whitePawns, square)) std::cout << "♟ ";        // White pawn
            else if (getBit(whiteKnights, square)) std::cout << "♞ ";  // White knight
            else if (getBit(whiteBishops, square)) std::cout << "♝ ";  // White bishop
            else if (getBit(whiteRooks, square)) std::cout << "♜ ";    // White rook
            else if (getBit(whiteQueens, square)) std::cout << "♛ ";   // White queen
            else if (getBit(whiteKing, square)) std::cout << "♚ ";     // White king
            else if (getBit(blackPawns, square)) std::cout << "♙ ";    // Black pawn
            else if (getBit(blackKnights, square)) std::cout << "♘ ";  // Black knight
            else if (getBit(blackBishops, square)) std::cout << "♗ ";  // Black bishop
            else if (getBit(blackRooks, square)) std::cout << "♖ ";    // Black rook
            else if (getBit(blackQueens, square)) std::cout << "♕ ";   // Black queen
            else if (getBit(blackKing, square)) std::cout << "♔ ";     // Black king
            else std::cout << ". ";                                    // Empty square
        }
        std::cout << std::endl;
    }
    // Print file letters at bottom
    std::cout << "  a b c d e f g h" << std::endl;
}

// Displays all legal moves for a piece at the given square
// Uses 'x' to mark possible moves, 'O' for selected piece, and '*' for other pieces
void displayLegalMoves(int square, Move::Color color) {
    // Initialize move bitboard and get occupied squares
    uint64_t moves = 0ULL;
    uint64_t occupied = whitePieces | blackPieces;
    uint64_t ownPieces = (color == Move::Color::WHITE) ? whitePieces : blackPieces;

    // Get moves based on piece type at the selected square
    if (getBit(whitePawns | blackPawns, square))
        moves = getPawnMoves(square, color, occupied);        // Pawn moves
    else if (getBit(whiteRooks | blackRooks, square))
        moves = getRookMoves(square, occupied);               // Rook moves
    else if (getBit(whiteBishops | blackBishops, square))
        moves = getBishopMoves(square, occupied);             // Bishop moves
    else if (getBit(whiteQueens | blackQueens, square))
        moves = getQueenMoves(square, occupied);              // Queen moves
    else if (getBit(whiteKnights | blackKnights, square))
        moves = getKnightMoves(square);                       // Knight moves
    else if (getBit(whiteKing | blackKing, square))
        moves = getKingMoves(square);                         // King moves

    // Remove moves that would capture own pieces
    moves &= ~ownPieces;

    // Display the board with legal moves marked
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; ++file) {
            int currentSquare = rank * 8 + file;
            if (getBit(moves, currentSquare))
                std::cout << "x ";                           // Possible move
            else if (currentSquare == square)
                std::cout << "O ";                           // Selected piece
            else if (getBit(occupied, currentSquare))
                std::cout << "* ";                           // Other pieces
            else
                std::cout << ". ";                          // Empty square
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl;
}

// **BITBOARD INTERACTION FUNCTIONS**
// Sets a bit to 1 at the specified square position
// Parameters:
//   board: The bitboard to modify
//   square: The square position (0-63) where to set the bit
// Returns: Modified bitboard with bit set to 1 at specified position
uint64_t setBit(uint64_t board, int square) {
    return board | (1ULL << square);  // Uses bitwise OR to set the bit
}

// Sets a bit to 0 at the specified square position
// Parameters:
//   board: The bitboard to modify
//   square: The square position (0-63) where to clear the bit
// Returns: Modified bitboard with bit set to 0 at specified position
uint64_t clearBit(uint64_t board, int square) {
    return board & ~(1ULL << square);  // Uses bitwise AND with inverted mask
}

// Gets the value of a bit at the specified square position
// Parameters:
//   board: The bitboard to check
//   square: The square position (0-63) to examine
// Returns: true if bit is 1, false if bit is 0
bool getBit(uint64_t board, int square) {
    return (board >> square) & 1;  // Shifts bit to LSB position and masks
}

// Initializes all bitboards to the starting chess position
// Sets up the initial game state with pieces in their standard positions
void initializeBitboards() {
    // Reset all bitboards to empty (all zeros)
    whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKing = 0;
    blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKing = 0;
    occupiedSquares = whitePieces = blackPieces = emptySquares = 0;

    // Initialize white pieces
    for (int i = 8; i < 16; ++i) {
        whitePawns = setBit(whitePawns, i);  // Place pawns on 2nd rank
    }
    whiteRooks = setBit(whiteRooks, 0);      // A1
    whiteRooks = setBit(whiteRooks, 7);      // H1
    whiteKnights = setBit(whiteKnights, 1);  // B1
    whiteKnights = setBit(whiteKnights, 6);  // G1
    whiteBishops = setBit(whiteBishops, 2);  // C1
    whiteBishops = setBit(whiteBishops, 5);  // F1
    whiteQueens = setBit(whiteQueens, 3);    // D1
    whiteKing = setBit(whiteKing, 4);        // E1

    // Initialize black pieces
    for (int i = 48; i < 56; ++i) {
        blackPawns = setBit(blackPawns, i);  // Place pawns on 7th rank
    }
    blackRooks = setBit(blackRooks, 56);     // A8
    blackRooks = setBit(blackRooks, 63);     // H8
    blackKnights = setBit(blackKnights, 57); // B8
    blackKnights = setBit(blackKnights, 62); // G8
    blackBishops = setBit(blackBishops, 58); // C8
    blackBishops = setBit(blackBishops, 61); // F8
    blackQueens = setBit(blackQueens, 59);   // D8
    blackKing = setBit(blackKing, 60);       // E8

    // Update composite bitboards
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    occupiedSquares = whitePieces | blackPieces;
    emptySquares = ~occupiedSquares;         // All squares not occupied
}

// Updates the bitboards after a move is made
// Parameters:
//   move: The move to apply to the board
void updateBitboards(const Move& move) {
    std::lock_guard<std::mutex> lock(searchMutex);
    uint64_t fromBB = 1ULL << move.fromSquare;
    uint64_t toBB = 1ULL << move.toSquare;
    bool isWhite = move.color == Move::Color::WHITE;

    // Reset en passant square
    int oldEnPassantSquare = enPassantSquare;
    enPassantSquare = -1;

    // 1. Remove piece from source square
    switch (move.pieceType) {
        case Move::PieceType::PAWN:   isWhite ? whitePawns &= ~fromBB : blackPawns &= ~fromBB; break;
        case Move::PieceType::KNIGHT: isWhite ? whiteKnights &= ~fromBB : blackKnights &= ~fromBB; break;
        case Move::PieceType::BISHOP: isWhite ? whiteBishops &= ~fromBB : blackBishops &= ~fromBB; break;
        case Move::PieceType::ROOK:   isWhite ? whiteRooks &= ~fromBB : blackRooks &= ~fromBB; break;
        case Move::PieceType::QUEEN:  isWhite ? whiteQueens &= ~fromBB : blackQueens &= ~fromBB; break;
        case Move::PieceType::KING:   isWhite ? whiteKing &= ~fromBB : blackKing &= ~fromBB; break;
    }

    // 2. Handle special pawn moves
    if (move.pieceType == Move::PieceType::PAWN) {
        // Set en passant square for double pawn push
        if (abs(move.toSquare - move.fromSquare) == 16) {
            enPassantSquare = isWhite ? move.fromSquare + 8 : move.fromSquare - 8;
        }
        
        // Handle en passant capture
        if (move.toSquare == oldEnPassantSquare) {
            uint64_t capturedPawnBB = 1ULL << (isWhite ? move.toSquare - 8 : move.toSquare + 8);
            if (isWhite) {
                blackPawns &= ~capturedPawnBB;
                blackPieces &= ~capturedPawnBB;
            } else {
                whitePawns &= ~capturedPawnBB;
                whitePieces &= ~capturedPawnBB;
            }
        }
    }

    // 3. Handle captures (except en passant)
    if (move.isCapture && move.toSquare != oldEnPassantSquare) {
        uint64_t* targetBoards[] = {
            isWhite ? &blackPawns : &whitePawns,
            isWhite ? &blackKnights : &whiteKnights,
            isWhite ? &blackBishops : &whiteBishops,
            isWhite ? &blackRooks : &whiteRooks,
            isWhite ? &blackQueens : &whiteQueens,
            isWhite ? &blackKing : &whiteKing
        };
        
        for (uint64_t* board : targetBoards) {
            if (*board & toBB) {
                *board &= ~toBB;
                break;
            }
        }
    }

    // 4. Place piece on destination square
    if (move.isPromotion) {
        switch (move.promotionPiece) {
            case Move::PieceType::QUEEN:  isWhite ? whiteQueens |= toBB : blackQueens |= toBB; break;
            case Move::PieceType::ROOK:   isWhite ? whiteRooks |= toBB : blackRooks |= toBB; break;
            case Move::PieceType::BISHOP: isWhite ? whiteBishops |= toBB : blackBishops |= toBB; break;
            case Move::PieceType::KNIGHT: isWhite ? whiteKnights |= toBB : blackKnights |= toBB; break;
        }
    } else {
        switch (move.pieceType) {
            case Move::PieceType::PAWN:   isWhite ? whitePawns |= toBB : blackPawns |= toBB; break;
            case Move::PieceType::KNIGHT: isWhite ? whiteKnights |= toBB : blackKnights |= toBB; break;
            case Move::PieceType::BISHOP: isWhite ? whiteBishops |= toBB : blackBishops |= toBB; break;
            case Move::PieceType::ROOK:   isWhite ? whiteRooks |= toBB : blackRooks |= toBB; break;
            case Move::PieceType::QUEEN:  isWhite ? whiteQueens |= toBB : blackQueens |= toBB; break;
            case Move::PieceType::KING:   isWhite ? whiteKing |= toBB : blackKing |= toBB; break;
        }
    }

    // 5. Handle castling
    if (move.isCastling) {
        uint64_t rookFromBB, rookToBB;
        if (isWhite) {
            if (move.castlingType == Move::CastlingType::KINGSIDE) {
                rookFromBB = 1ULL << 7;  // H1
                rookToBB = 1ULL << 5;    // F1
            } else {
                rookFromBB = 1ULL << 0;  // A1
                rookToBB = 1ULL << 3;    // D1
            }
            whiteRooks = (whiteRooks & ~rookFromBB) | rookToBB;
        } else {
            if (move.castlingType == Move::CastlingType::KINGSIDE) {
                rookFromBB = 1ULL << 63; // H8
                rookToBB = 1ULL << 61;   // F8
            } else {
                rookFromBB = 1ULL << 56; // A8
                rookToBB = 1ULL << 59;   // D8
            }
            blackRooks = (blackRooks & ~rookFromBB) | rookToBB;
        }
    }

    // 6. Update castling rights
    if (move.pieceType == Move::PieceType::KING) {
        if (isWhite) {
            castlingRights.whiteKingside = false;
            castlingRights.whiteQueenside = false;
        } else {
            castlingRights.blackKingside = false;
            castlingRights.blackQueenside = false;
        }
    } else if (move.pieceType == Move::PieceType::ROOK) {
        if (isWhite) {
            if (move.fromSquare == 0) castlingRights.whiteQueenside = false;
            if (move.fromSquare == 7) castlingRights.whiteKingside = false;
        } else {
            if (move.fromSquare == 56) castlingRights.blackQueenside = false;
            if (move.fromSquare == 63) castlingRights.blackKingside = false;
        }
    }

    // 7. Update composite bitboards
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    occupiedSquares = whitePieces | blackPieces;
    emptySquares = ~occupiedSquares;
}

void updateBitboardsLocal(Position& pos, const Move& move) {
    uint64_t fromBB = 1ULL << move.fromSquare;
    uint64_t toBB = 1ULL << move.toSquare;
    bool isWhite = move.color == Move::Color::WHITE;

    // 1. Remove piece from source square
    switch (move.pieceType) {
        case Move::PieceType::PAWN:   
            isWhite ? pos.whitePawns &= ~fromBB : pos.blackPawns &= ~fromBB; 
            break;
        case Move::PieceType::KNIGHT: 
            isWhite ? pos.whiteKnights &= ~fromBB : pos.blackKnights &= ~fromBB; 
            break;
        case Move::PieceType::BISHOP: 
            isWhite ? pos.whiteBishops &= ~fromBB : pos.blackBishops &= ~fromBB; 
            break;
        case Move::PieceType::ROOK:   
            isWhite ? pos.whiteRooks &= ~fromBB : pos.blackRooks &= ~fromBB; 
            break;
        case Move::PieceType::QUEEN:  
            isWhite ? pos.whiteQueens &= ~fromBB : pos.blackQueens &= ~fromBB; 
            break;
        case Move::PieceType::KING:   
            isWhite ? pos.whiteKing &= ~fromBB : pos.blackKing &= ~fromBB; 
            break;
    }

    // 2. Handle captures
    if (move.isCapture) {
        if (isWhite) {
            pos.blackPieces &= ~toBB;
            if (pos.blackPawns & toBB) pos.blackPawns &= ~toBB;
            if (pos.blackKnights & toBB) pos.blackKnights &= ~toBB;
            if (pos.blackBishops & toBB) pos.blackBishops &= ~toBB;
            if (pos.blackRooks & toBB) pos.blackRooks &= ~toBB;
            if (pos.blackQueens & toBB) pos.blackQueens &= ~toBB;
        } else {
            pos.whitePieces &= ~toBB;
            if (pos.whitePawns & toBB) pos.whitePawns &= ~toBB;
            if (pos.whiteKnights & toBB) pos.whiteKnights &= ~toBB;
            if (pos.whiteBishops & toBB) pos.whiteBishops &= ~toBB;
            if (pos.whiteRooks & toBB) pos.whiteRooks &= ~toBB;
            if (pos.whiteQueens & toBB) pos.whiteQueens &= ~toBB;
        }
    }

    // 3. Place piece on destination square
    if (move.isPromotion) {
        switch (move.promotionPiece) {
            case Move::PieceType::QUEEN:  
                isWhite ? pos.whiteQueens |= toBB : pos.blackQueens |= toBB; 
                break;
            case Move::PieceType::ROOK:   
                isWhite ? pos.whiteRooks |= toBB : pos.blackRooks |= toBB; 
                break;
            case Move::PieceType::BISHOP: 
                isWhite ? pos.whiteBishops |= toBB : pos.blackBishops |= toBB; 
                break;
            case Move::PieceType::KNIGHT: 
                isWhite ? pos.whiteKnights |= toBB : pos.blackKnights |= toBB; 
                break;
        }
    } else {
        switch (move.pieceType) {
            case Move::PieceType::PAWN:   
                isWhite ? pos.whitePawns |= toBB : pos.blackPawns |= toBB; 
                break;
            case Move::PieceType::KNIGHT: 
                isWhite ? pos.whiteKnights |= toBB : pos.blackKnights |= toBB; 
                break;
            case Move::PieceType::BISHOP: 
                isWhite ? pos.whiteBishops |= toBB : pos.blackBishops |= toBB; 
                break;
            case Move::PieceType::ROOK:   
                isWhite ? pos.whiteRooks |= toBB : pos.blackRooks |= toBB; 
                break;
            case Move::PieceType::QUEEN:  
                isWhite ? pos.whiteQueens |= toBB : pos.blackQueens |= toBB; 
                break;
            case Move::PieceType::KING:   
                isWhite ? pos.whiteKing |= toBB : pos.blackKing |= toBB; 
                break;
        }
    }

    // 4. Update composite bitboards
    pos.whitePieces = pos.whitePawns | pos.whiteKnights | pos.whiteBishops | 
                      pos.whiteRooks | pos.whiteQueens | pos.whiteKing;
    pos.blackPieces = pos.blackPawns | pos.blackKnights | pos.blackBishops | 
                      pos.blackRooks | pos.blackQueens | pos.blackKing;
    pos.occupiedSquares = pos.whitePieces | pos.blackPieces;
    pos.emptySquares = ~pos.occupiedSquares;
}

// debugging function
void printBitBoard(uint64_t board) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << (getBit(board, square) ? "1 " : "0 ");
        }
        std::cout << std::endl;
    }
}

// Generates a mask of potential blocking squares for a rook on a given square
// Excludes edge squares since they don't affect sliding piece movement calculations 
uint64_t generateRookMask(int square) {
    uint64_t mask = 0ULL;
    int rank = square / 8;  // Get rank (0-7) from square number
    int file = square % 8;  // Get file (0-7) from square number

    // Generate rays in all 4 directions (excluding edges)
    // North ray
    for (int r = rank + 1; r < 7; r++) 
        mask |= (1ULL << (r * 8 + file));
    // South ray  
    for (int r = rank - 1; r > 0; r--) 
        mask |= (1ULL << (r * 8 + file));
    // East ray
    for (int f = file + 1; f < 7; f++) 
        mask |= (1ULL << (rank * 8 + f));
    // West ray
    for (int f = file - 1; f > 0; f--) 
        mask |= (1ULL << (rank * 8 + f));

    return mask;
}

// Generates a mask of potential blocking squares for a bishop on a given square
// Similar to rook mask but for diagonal movements
uint64_t generateBishopMask(int square) {
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate rays in all 4 diagonal directions (excluding edges)
    // Northeast ray
    for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++)
        mask |= (1ULL << (r * 8 + f));
    // Southeast ray  
    for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--)
        mask |= (1ULL << (r * 8 + f));
    // Northwest ray
    for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++)
        mask |= (1ULL << (r * 8 + f));
    // Southwest ray
    for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--)
        mask |= (1ULL << (r * 8 + f));

    return mask;
}

// Generates all possible rook attacks for a given square and blocker configuration
uint64_t generateRookAttacks(int square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks in all 4 directions (stopping at blockers)
    // North ray
    for (int r = rank + 1; r < 8; r++) {
        attacks |= (1ULL << (r * 8 + file));
        if (blockers & (1ULL << (r * 8 + file))) break;
    }
    // South ray
    for (int r = rank - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + file));
        if (blockers & (1ULL << (r * 8 + file))) break;
    }
    // East ray
    for (int f = file + 1; f < 8; f++) {
        attacks |= (1ULL << (rank * 8 + f));
        if (blockers & (1ULL << (rank * 8 + f))) break;
    }
    // West ray
    for (int f = file - 1; f >= 0; f--) {
        attacks |= (1ULL << (rank * 8 + f));
        if (blockers & (1ULL << (rank * 8 + f))) break;
    }

    return attacks;
}

// Generates all possible bishop attacks from a given square with blocker configuration
// Parameters:
//   square: The square the bishop is on (0-63)
//   blockers: Bitboard representing all blocking pieces
// Returns: Bitboard with all possible bishop moves/attacks
uint64_t generateBishopAttacks(int square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks in all 4 diagonal directions (stopping at blockers)
    // Northeast ray
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;  // Stop if blocker found
    }
    // Southeast ray
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;  // Stop if blocker found
    }
    // Northwest ray
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;  // Stop if blocker found
    }
    // Southwest ray
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;  // Stop if blocker found
    }

    return attacks;
}

// Function to initialize the magic bitboard tables
// Pre-calculates attack patterns for all squares and possible blocker configurations
void initializeMagicBitboards() {
    // Initialize rook tables
    for (int square = 0; square < 64; square++) {
        rookTable[square].mask = generateRookMask(square);
        rookTable[square].magic = ROOK_MAGICS[square];
        rookTable[square].shift = 64 - __builtin_popcountll(rookTable[square].mask);
        
        uint64_t mask = rookTable[square].mask;
        int variations = 1 << __builtin_popcountll(mask);
        
        // Pre-allocate with proper size
        rookTable[square].attacks.clear();
        rookTable[square].attacks.resize(variations);
        
        for (int i = 0; i < variations; i++) {
            uint64_t blockers = 0ULL;
            int bit = 0;
            for (int j = 0; j < 64; j++) {
                if (mask & (1ULL << j)) {
                    if (i & (1 << bit)) {
                        blockers |= (1ULL << j);
                    }
                    bit++;
                }
            }
            
            uint64_t index = ((blockers * rookTable[square].magic) >> rookTable[square].shift);
            index &= (variations - 1);  // Ensure index is within bounds
            rookTable[square].attacks[index] = generateRookAttacks(square, blockers);
        }
    }

    // Similar changes for bishop table initialization
    for (int square = 0; square < 64; square++) {
        bishopTable[square].mask = generateBishopMask(square);
        bishopTable[square].magic = BISHOP_MAGICS[square];
        bishopTable[square].shift = 64 - __builtin_popcountll(bishopTable[square].mask);
        
        uint64_t mask = bishopTable[square].mask;
        int variations = 1 << __builtin_popcountll(mask);
        
        bishopTable[square].attacks.clear();
        bishopTable[square].attacks.resize(variations);
        
        for (int i = 0; i < variations; i++) {
            uint64_t blockers = 0ULL;
            int bit = 0;
            for (int j = 0; j < 64; j++) {
                if (mask & (1ULL << j)) {
                    if (i & (1 << bit)) {
                        blockers |= (1ULL << j);
                    }
                    bit++;
                }
            }
            
            uint64_t index = ((blockers * bishopTable[square].magic) >> bishopTable[square].shift);
            index &= (variations - 1);  // Ensure index is within bounds
            bishopTable[square].attacks[index] = generateBishopAttacks(square, blockers);
        }
    }
}

// Functions to get moves using magic bitboards
uint64_t getRookMoves(int square, uint64_t occupied) {
    MagicEntry& entry = rookTable[square];
    uint64_t blockers = occupied & entry.mask;
    uint64_t index = ((blockers * entry.magic) >> entry.shift);
    if (index >= entry.attacks.size()) {
        return 0ULL;  // Return empty bitboard if index is out of bounds
    }
    return entry.attacks[index];
}

uint64_t getBishopMoves(int square, uint64_t occupied) {
    MagicEntry& entry = bishopTable[square];
    uint64_t blockers = occupied & entry.mask;
    uint64_t index = ((blockers * entry.magic) >> entry.shift);
    
    index &= (entry.attacks.size() - 1);
    // Safety check for array bounds
    if (index >= entry.attacks.size()) {
        return 0ULL;  // Return empty bitboard if index is out of bounds
    }
    return entry.attacks[index];
}

uint64_t getQueenMoves(int square, uint64_t occupied) {
    // Queen moves are combination of rook and bishop moves
    return getRookMoves(square, occupied) | getBishopMoves(square, occupied);
}

uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied) {
    uint64_t moves = 0ULL;
    uint64_t pawnBB = 1ULL << square;

    if (color == Move::Color::WHITE) {
        // Single pushes forward
        uint64_t singlePush = (pawnBB << 8) & ~occupied;
        moves |= singlePush;
        
        // Double push from starting position
        if (pawnBB & RANK_2) {
            moves |= ((singlePush << 8) & ~occupied & RANK_4);
        }
        // Captures to the left
        if ((square % 8) != 0) {
            moves |= ((pawnBB << 7) & blackPieces & ~FILE_H);
        }
        // Captures to the right
        if ((square % 8) != 7) {
            moves |= ((pawnBB << 9) & blackPieces & ~FILE_A);
        }
        // en passant captures
        if (enPassantSquare != -1 && square >= 24 && square < 32) { // White pawns on rank 5
            if ((square % 8) != 0 && enPassantSquare == square + 7) {
                moves |= (1ULL << enPassantSquare);
            }
            if ((square % 8) != 7 && enPassantSquare == square + 9) {
                moves |= (1ULL << enPassantSquare);
            }
        }
    } else {  // Black pawns
        // Single pushes forward
        uint64_t singlePush = (pawnBB >> 8) & ~occupied;
        moves |= singlePush;
        
        // Double push from starting position
        if (pawnBB & RANK_7) {
            moves |= ((singlePush >> 8) & ~occupied & RANK_5);
        }
        // Captures to the left
        if ((square % 8) != 0) {
            moves |= ((pawnBB >> 9) & whitePieces & ~FILE_H);
        }
        // Captures to the right
        if ((square % 8) != 7) {
            moves |= ((pawnBB >> 7) & whitePieces & ~FILE_A);
        }
        // En passant captures
        if (enPassantSquare != -1 && square >= 32 && square < 40) { // Black pawns on rank 4
            if ((square % 8) != 0 && enPassantSquare == square - 9) {
                moves |= (1ULL << enPassantSquare);
            }
            if ((square % 8) != 7 && enPassantSquare == square - 7) {
                moves |= (1ULL << enPassantSquare);
            }
        }
    }

    return moves;
}

// Local version of getPawnMoves that works with a Position struct
uint64_t getPawnMovesLocal(int square, Move::Color color, uint64_t occupied, const Position& pos) {
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

uint64_t getKnightMoves(int square) {
    return KNIGHT_ATTACKS[square];  // Return pre-calculated knight attacks
}

uint64_t getKingMoves(int square) {
    return KING_ATTACKS[square];  // Return pre-calculated king attacks
}

bool isSquareAttacked(int square, Move::Color attackingColor) {
    uint64_t occupied = whitePieces | blackPieces;
    uint64_t attackers = (attackingColor == Move::Color::WHITE) ? whitePieces : blackPieces;
    
    // Check for pawn attacks
    if (attackingColor == Move::Color::WHITE) {
        // White pawns attack diagonally upward
        if (square > 7 && ((square % 8) > 0 && getBit(whitePawns, square - 9) ||
            (square % 8) < 7 && getBit(whitePawns, square - 7))) {
            return true;
        }
    } else {
        // Black pawns attack diagonally downward
        if (square < 56 && ((square % 8) > 0 && getBit(blackPawns, square + 7) ||
            (square % 8) < 7 && getBit(blackPawns, square + 9))) {
            return true;
        }
    }

    // Check for knight attacks
    uint64_t knightAttacks = getKnightMoves(square);
    if (knightAttacks & (attackingColor == Move::Color::WHITE ? whiteKnights : blackKnights))
        return true;

    // Check for bishop/queen attacks
    uint64_t bishopAttacks = getBishopMoves(square, occupied);
    if (bishopAttacks & (attackingColor == Move::Color::WHITE ? 
        (whiteBishops | whiteQueens) : (blackBishops | blackQueens)))
        return true;

    // Check for rook/queen attacks
    uint64_t rookAttacks = getRookMoves(square, occupied);
    if (rookAttacks & (attackingColor == Move::Color::WHITE ? 
        (whiteRooks | whiteQueens) : (blackRooks | blackQueens)))
        return true;

    // Check for king attacks
    uint64_t kingAttacks = getKingMoves(square);
    if (kingAttacks & (attackingColor == Move::Color::WHITE ? whiteKing : blackKing))
        return true;

    return false;
}

// Local version of isSquareAttacked that works with a Position struct
bool isSquareAttackedLocal(int square, Move::Color attackingColor, const Position& pos) {
    uint64_t occupied = pos.whitePieces | pos.blackPieces;
    
    // Check for pawn attacks
    if (attackingColor == Move::Color::WHITE) {
        if (square > 7 && ((square % 8) > 0 && getBit(pos.whitePawns, square - 9) ||
            (square % 8) < 7 && getBit(pos.whitePawns, square - 7))) {
            return true;
        }
    } else {
        if (square < 56 && ((square % 8) > 0 && getBit(pos.blackPawns, square + 7) ||
            (square % 8) < 7 && getBit(pos.blackPawns, square + 9))) {
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

bool isInCheck(Move::Color color) {
    uint64_t kingBB = (color == Move::Color::WHITE) ? whiteKing : blackKing;
    int kingSquare = __builtin_ctzll(kingBB);  // Get king's square (least significant set bit)
    return isSquareAttacked(kingSquare, color == Move::Color::WHITE ? 
        Move::Color::BLACK : Move::Color::WHITE);
}

// Local version of isInCheck that works with a Position struct
bool isInCheckLocal(const Position& pos, Move::Color color) {
    uint64_t kingBB = (color == Move::Color::WHITE) ? pos.whiteKing : pos.blackKing;
    int kingSquare = __builtin_ctzll(kingBB);
    return isSquareAttackedLocal(kingSquare, color == Move::Color::WHITE ? 
        Move::Color::BLACK : Move::Color::WHITE, pos);
} 

uint64_t getCastlingMoves(Move::Color color, uint64_t occupied) {
    uint64_t moves = 0ULL;
    
    if (color == Move::Color::WHITE) {
        if (castlingRights.whiteKingside) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 5) && !getBit(occupied, 6) &&
                !isSquareAttacked(4, Move::Color::BLACK) &&
                !isSquareAttacked(5, Move::Color::BLACK) &&
                !isSquareAttacked(6, Move::Color::BLACK)) {
                moves |= (1ULL << 6);  // g1
            }
        }
        if (castlingRights.whiteQueenside) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 1) && !getBit(occupied, 2) && !getBit(occupied, 3) &&
                !isSquareAttacked(4, Move::Color::BLACK) &&
                !isSquareAttacked(3, Move::Color::BLACK) &&
                !isSquareAttacked(2, Move::Color::BLACK)) {
                moves |= (1ULL << 2);  // c1
            }
        }
    } else {
        if (castlingRights.blackKingside) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 61) && !getBit(occupied, 62) &&
                !isSquareAttacked(60, Move::Color::WHITE) &&
                !isSquareAttacked(61, Move::Color::WHITE) &&
                !isSquareAttacked(62, Move::Color::WHITE)) {
                moves |= (1ULL << 62);  // g8
            }
        }
        if (castlingRights.blackQueenside) {
            // Check if squares between king and rook are empty and not attacked
            if (!getBit(occupied, 57) && !getBit(occupied, 58) && !getBit(occupied, 59) &&
                !isSquareAttacked(60, Move::Color::WHITE) &&
                !isSquareAttacked(59, Move::Color::WHITE) &&
                !isSquareAttacked(58, Move::Color::WHITE)) {
                moves |= (1ULL << 58);  // c8
            }
        }
    }
    
    return moves;
}

// Local version of getCastlingMoves that works with a Position struct
uint64_t getCastlingMovesLocal(Move::Color color, uint64_t occupied, const Position& pos) {
    uint64_t moves = 0ULL;
    
    if (color == Move::Color::WHITE) {
        if (pos.castling.whiteKingside) {
            if (!getBit(occupied, 5) && !getBit(occupied, 6) &&
                !isSquareAttackedLocal(4, Move::Color::BLACK, pos) &&
                !isSquareAttackedLocal(5, Move::Color::BLACK, pos) &&
                !isSquareAttackedLocal(6, Move::Color::BLACK, pos)) {
                moves |= (1ULL << 6);  // g1
            }
        }
        if (pos.castling.whiteQueenside) {
            if (!getBit(occupied, 1) && !getBit(occupied, 2) && !getBit(occupied, 3) &&
                !isSquareAttackedLocal(4, Move::Color::BLACK, pos) &&
                !isSquareAttackedLocal(3, Move::Color::BLACK, pos) &&
                !isSquareAttackedLocal(2, Move::Color::BLACK, pos)) {
                moves |= (1ULL << 2);  // c1
            }
        }
    } else {
        if (pos.castling.blackKingside) {
            if (!getBit(occupied, 61) && !getBit(occupied, 62) &&
                !isSquareAttackedLocal(60, Move::Color::WHITE, pos) &&
                !isSquareAttackedLocal(61, Move::Color::WHITE, pos) &&
                !isSquareAttackedLocal(62, Move::Color::WHITE, pos)) {
                moves |= (1ULL << 62);  // g8
            }
        }
        if (pos.castling.blackQueenside) {
            if (!getBit(occupied, 57) && !getBit(occupied, 58) && !getBit(occupied, 59) &&
                !isSquareAttackedLocal(60, Move::Color::WHITE, pos) &&
                !isSquareAttackedLocal(59, Move::Color::WHITE, pos) &&
                !isSquareAttackedLocal(58, Move::Color::WHITE, pos)) {
                moves |= (1ULL << 58);  // c8
            }
        }
    }
    
    return moves;
}

// Restores a previously saved board state from a Position struct
void restoreBitboards(const Position& pos) {
    std::lock_guard<std::mutex> lock(searchMutex);
    // Restore all piece bitboards
    whitePawns = pos.whitePawns;
    whiteKnights = pos.whiteKnights;
    whiteBishops = pos.whiteBishops;
    whiteRooks = pos.whiteRooks;
    whiteQueens = pos.whiteQueens;
    whiteKing = pos.whiteKing;
    
    blackPawns = pos.blackPawns;
    blackKnights = pos.blackKnights;
    blackBishops = pos.blackBishops;
    blackRooks = pos.blackRooks;
    blackQueens = pos.blackQueens;
    blackKing = pos.blackKing;
    
    // Restore composite bitboards
    whitePieces = pos.whitePieces;
    blackPieces = pos.blackPieces;
    occupiedSquares = pos.occupiedSquares;
    emptySquares = pos.emptySquares;
    
    // Restore castling rights
    castlingRights = pos.castling;
    // restore en passant square
    enPassantSquare = pos.enPassantSquare;
}

// Evaluates the current position from White's perspective in centipawns
int evaluatePosition() {
    int score = 0;
    
    // Single pass through the board
    for (int square = 0; square < 64; square++) {
        // White pieces (positive scores)
        if (getBit(whitePawns, square)) {
            score += PAWN_VALUE + PAWN_PST[square];
        }
        else if (getBit(whiteKnights, square)) {
            score += KNIGHT_VALUE + KNIGHT_PST[square];
        }
        else if (getBit(whiteBishops, square)) {
            score += BISHOP_VALUE + BISHOP_PST[square];
        }
        else if (getBit(whiteRooks, square)) {
            score += ROOK_VALUE + ROOK_PST[square];
        }
        else if (getBit(whiteQueens, square)) {
            score += QUEEN_VALUE + QUEEN_PST[square];
        }
        else if (getBit(whiteKing, square)) {
            score += KING_PST[square];  // King value not added to avoid huge scores
        }
        
        // Black pieces (negative scores)
        else if (getBit(blackPawns, square)) {
            score -= PAWN_VALUE + PAWN_PST[square ^ 56];
        }
        else if (getBit(blackKnights, square)) {
            score -= KNIGHT_VALUE + KNIGHT_PST[square ^ 56];
        }
        else if (getBit(blackBishops, square)) {
            score -= BISHOP_VALUE + BISHOP_PST[square ^ 56];
        }
        else if (getBit(blackRooks, square)) {
            score -= ROOK_VALUE + ROOK_PST[square ^ 56];
        }
        else if (getBit(blackQueens, square)) {
            score -= QUEEN_VALUE + QUEEN_PST[square ^ 56];
        }
        else if (getBit(blackKing, square)) {
            score -= KING_PST[square ^ 56];
        }
    }
    
    return score;
}

// Local version of evaluatePosition that works with a Position struct
int evaluatePositionLocal(const Position& pos) {
    int score = 0;
    
    for (int square = 0; square < 64; square++) {
        // White pieces (positive scores)
        if (getBit(pos.whitePawns, square)) {
            score += PAWN_VALUE + PAWN_PST[square];
        }
        else if (getBit(pos.whiteKnights, square)) {
            score += KNIGHT_VALUE + KNIGHT_PST[square];
        }
        else if (getBit(pos.whiteBishops, square)) {
            score += BISHOP_VALUE + BISHOP_PST[square];
        }
        else if (getBit(pos.whiteRooks, square)) {
            score += ROOK_VALUE + ROOK_PST[square];
        }
        else if (getBit(pos.whiteQueens, square)) {
            score += QUEEN_VALUE + QUEEN_PST[square];
        }
        else if (getBit(pos.whiteKing, square)) {
            score += KING_PST[square];
        }
        
        // Black pieces (negative scores)
        else if (getBit(pos.blackPawns, square)) {
            score -= PAWN_VALUE + PAWN_PST[square ^ 56];
        }
        else if (getBit(pos.blackKnights, square)) {
            score -= KNIGHT_VALUE + KNIGHT_PST[square ^ 56];
        }
        else if (getBit(pos.blackBishops, square)) {
            score -= BISHOP_VALUE + BISHOP_PST[square ^ 56];
        }
        else if (getBit(pos.blackRooks, square)) {
            score -= ROOK_VALUE + ROOK_PST[square ^ 56];
        }
        else if (getBit(pos.blackQueens, square)) {
            score -= QUEEN_VALUE + QUEEN_PST[square ^ 56];
        }
        else if (getBit(pos.blackKing, square)) {
            score -= KING_PST[square ^ 56];
        }
    }
    
    return score;
}

// Helper function to copy a Position
Position copyPosition(const Position& pos) {
    Position newPos;
    newPos.whitePawns = pos.whitePawns;
    newPos.whiteKnights = pos.whiteKnights;
    newPos.whiteBishops = pos.whiteBishops;
    newPos.whiteRooks = pos.whiteRooks;
    newPos.whiteQueens = pos.whiteQueens;
    newPos.whiteKing = pos.whiteKing;
    
    newPos.blackPawns = pos.blackPawns;
    newPos.blackKnights = pos.blackKnights;
    newPos.blackBishops = pos.blackBishops;
    newPos.blackRooks = pos.blackRooks;
    newPos.blackQueens = pos.blackQueens;
    newPos.blackKing = pos.blackKing;
    
    newPos.whitePieces = pos.whitePieces;
    newPos.blackPieces = pos.blackPieces;
    newPos.occupiedSquares = pos.occupiedSquares;
    newPos.emptySquares = pos.emptySquares;
    
    newPos.castling = pos.castling;
    newPos.enPassantSquare = pos.enPassantSquare;
    
    return newPos;
}

// Add this near other struct/class definitions
class ThreadSafePosition {
    private:
        Position pos;
        mutable std::mutex mutex;
    
    public:
        ThreadSafePosition(const Position& initial) : pos(initial) {}
        
        Position get() const {
            std::lock_guard<std::mutex> lock(mutex);
            return pos;
        }
        
        void update(const Position& newPos) {
            std::lock_guard<std::mutex> lock(mutex);
            pos = newPos;
        }
        
        void makeMove(const Move& move) {
            std::lock_guard<std::mutex> lock(mutex);
            Position newPos = copyPosition(pos);
            updateBitboardsLocal(newPos, move);
            pos = newPos;
        }
};

SearchResult quiescenceSearch(ThreadSafePosition& threadPos, int alpha, int beta, Move::Color color);
SearchResult negamax(ThreadSafePosition& threadPos, int depth, int alpha, int beta, 
                    Move::Color color, int ply);  // Negamax search with alpha-beta pruning

std::vector<Move> generateMoves(const Position& pos, Move::Color color) {
    std::vector<Move> moves;
    uint64_t pieces = (color == Move::Color::WHITE) ? pos.whitePieces : pos.blackPieces;
    uint64_t occupied = pos.whitePieces | pos.blackPieces;
    
    // Generate moves for each piece type
    for (int square = 0; square < 64; square++) {
        if (!getBit(pieces, square)) continue;
        
        uint64_t legalMoves = 0ULL;
        Move move;
        move.fromSquare = square;
        move.color = color;
        move.isCastling = false;
        move.isPromotion = false;
        
        // Pawns
        if (getBit(whitePawns | blackPawns, square)) {
            move.pieceType = Move::PieceType::PAWN;
            legalMoves = getPawnMoves(square, color, occupied);
            
            // Check for promotions
            if ((color == Move::Color::WHITE && square >= 48 && square < 56) ||
                (color == Move::Color::BLACK && square >= 8 && square < 16)) {
                move.isPromotion = true;
            }
        }
        // Knights
        else if (getBit(whiteKnights | blackKnights, square)) {
            move.pieceType = Move::PieceType::KNIGHT;
            legalMoves = getKnightMoves(square) & ~pieces;
        }
        // Bishops
        else if (getBit(whiteBishops | blackBishops, square)) {
            move.pieceType = Move::PieceType::BISHOP;
            legalMoves = getBishopMoves(square, occupied) & ~pieces;
        }
        // Rooks
        else if (getBit(whiteRooks | blackRooks, square)) {
            move.pieceType = Move::PieceType::ROOK;
            legalMoves = getRookMoves(square, occupied) & ~pieces;
        }
        // Queens
        else if (getBit(whiteQueens | blackQueens, square)) {
            move.pieceType = Move::PieceType::QUEEN;
            legalMoves = getQueenMoves(square, occupied) & ~pieces;
        }
        // Kings
        else if (getBit(whiteKing | blackKing, square)) {
            move.pieceType = Move::PieceType::KING;
            legalMoves = getKingMoves(square) & ~pieces;
            
            // Add castling moves
            uint64_t castlingMoves = getCastlingMoves(color, occupied);
            if (castlingMoves) {
                while (castlingMoves) {
                    int toSquare = __builtin_ctzll(castlingMoves);
                    Move castlingMove = move;
                    castlingMove.toSquare = toSquare;
                    castlingMove.isCastling = true;
                    castlingMove.castlingType = (toSquare % 8 > 4) ? 
                        Move::CastlingType::KINGSIDE : Move::CastlingType::QUEENSIDE;
                    moves.push_back(castlingMove);
                    castlingMoves &= castlingMoves - 1;
                }
            }
        }

        // Convert bitboard of legal moves to actual Move objects
        while (legalMoves) {
            int toSquare = __builtin_ctzll(legalMoves);
            move.toSquare = toSquare;
            move.isCapture = getBit((color == Move::Color::WHITE ? blackPieces : whitePieces), toSquare);
            
            if (move.isPromotion) {
                // Generate all possible promotion pieces
                Move promotionMove = move;
                promotionMove.promotionPiece = Move::PieceType::QUEEN;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::ROOK;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::BISHOP;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::KNIGHT;
                moves.push_back(promotionMove);
            } else {
                moves.push_back(move);
            }
            
            legalMoves &= legalMoves - 1;  // clear least significant bit
        }
    }
    
    return moves;
}

std::vector<Move> generateMovesLocal(const Position& pos, Move::Color color) {
    std::vector<Move> moves;
    uint64_t pieces = (color == Move::Color::WHITE) ? pos.whitePieces : pos.blackPieces;
    uint64_t occupied = pos.whitePieces | pos.blackPieces;
    
    // Generate moves for each piece type
    for (int square = 0; square < 64; square++) {
        if (!getBit(pieces, square)) continue;
        
        uint64_t legalMoves = 0ULL;
        Move move;
        move.fromSquare = square;
        move.color = color;
        move.isCastling = false;
        move.isPromotion = false;
        
        // Determine piece type and get legal moves
        if (getBit(color == Move::Color::WHITE ? pos.whitePawns : pos.blackPawns, square)) {
            move.pieceType = Move::PieceType::PAWN;
            legalMoves = getPawnMovesLocal(square, color, occupied, pos);
            
            if ((color == Move::Color::WHITE && square >= 48 && square < 56) ||
                (color == Move::Color::BLACK && square >= 8 && square < 16)) {
                move.isPromotion = true;
            }
        }
        else if (getBit(color == Move::Color::WHITE ? pos.whiteKnights : pos.blackKnights, square)) {
            move.pieceType = Move::PieceType::KNIGHT;
            legalMoves = getKnightMoves(square) & ~pieces;
        }
        else if (getBit(color == Move::Color::WHITE ? pos.whiteBishops : pos.blackBishops, square)) {
            move.pieceType = Move::PieceType::BISHOP;
            legalMoves = getBishopMoves(square, occupied) & ~pieces;
        }
        else if (getBit(color == Move::Color::WHITE ? pos.whiteRooks : pos.blackRooks, square)) {
            move.pieceType = Move::PieceType::ROOK;
            legalMoves = getRookMoves(square, occupied) & ~pieces;
        }
        else if (getBit(color == Move::Color::WHITE ? pos.whiteQueens : pos.blackQueens, square)) {
            move.pieceType = Move::PieceType::QUEEN;
            legalMoves = getQueenMoves(square, occupied) & ~pieces;
        }
        else if (getBit(color == Move::Color::WHITE ? pos.whiteKing : pos.blackKing, square)) {
            move.pieceType = Move::PieceType::KING;
            legalMoves = getKingMoves(square) & ~pieces;
            
            // Add castling moves
            uint64_t castlingMoves = getCastlingMovesLocal(color, occupied, pos);
            if (castlingMoves) {
                while (castlingMoves) {
                    int toSquare = __builtin_ctzll(castlingMoves);
                    Move castlingMove = move;
                    castlingMove.toSquare = toSquare;
                    castlingMove.isCastling = true;
                    castlingMove.castlingType = (toSquare % 8 > 4) ? 
                        Move::CastlingType::KINGSIDE : Move::CastlingType::QUEENSIDE;
                    moves.push_back(castlingMove);
                    castlingMoves &= castlingMoves - 1;
                }
            }
        }

        // Convert bitboard of legal moves to actual Move objects
        while (legalMoves) {
            int toSquare = __builtin_ctzll(legalMoves);
            move.toSquare = toSquare;
            move.isCapture = getBit((color == Move::Color::WHITE ? pos.blackPieces : pos.whitePieces), toSquare);
            
            if (move.isPromotion) {
                Move promotionMove = move;
                for (auto piece : {Move::PieceType::QUEEN, Move::PieceType::ROOK, 
                                 Move::PieceType::BISHOP, Move::PieceType::KNIGHT}) {
                    promotionMove.promotionPiece = piece;
                    moves.push_back(promotionMove);
                }
            } else {
                moves.push_back(move);
            }
            
            legalMoves &= legalMoves - 1;
        }
    }
    
    // Filter out moves that would leave the king in check
    moves.erase(
        std::remove_if(moves.begin(), moves.end(),
            [&pos, color](const Move& move) {
                Position newPos = copyPosition(pos);
                updateBitboardsLocal(newPos, move);
                return isInCheckLocal(newPos, color);
            }
        ),
        moves.end()
    );
    
    return moves;
}

int getPieceValue(Move::PieceType piece) {
    switch (piece) {
        case Move::PieceType::PAWN:   return PAWN_VALUE;
        case Move::PieceType::KNIGHT: return KNIGHT_VALUE;
        case Move::PieceType::BISHOP: return BISHOP_VALUE;
        case Move::PieceType::ROOK:   return ROOK_VALUE;
        case Move::PieceType::QUEEN:  return QUEEN_VALUE;
        case Move::PieceType::KING:   return KING_VALUE;
        default: return 0;
    }
}

void sortMoves(std::vector<Move>& moves) {
    std::sort(moves.begin(), moves.end(), 
        [](const Move& a, const Move& b) {
            // Simple capture prioritization
            if (a.isCapture != b.isCapture) {
                return a.isCapture > b.isCapture;  // Captures first
            }

            // For promotions, prefer queen promotions
            if (a.isPromotion && b.isPromotion) {
                return a.promotionPiece < b.promotionPiece;  // Queen (0) before other pieces
            }

            return false;  // Keep original order for non-captures
        });
}

SearchResult negamax(ThreadSafePosition& threadPos, int depth, int alpha, int beta, 
                     Move::Color color, int ply) {
    if (depth == 0) {
        return quiescenceSearch(threadPos, alpha, beta, color);
    }

    Position pos = threadPos.get();
    std::vector<Move> moves = generateMovesLocal(pos, color);
    
    if (moves.empty()) {
        if (isInCheckLocal(pos, color)) return {-KING_VALUE + ply, Move()};
        return {0, Move()};
    }

    sortMoves(moves);
    SearchResult best = {-KING_VALUE * 2, moves[0]};

    for (const Move& move : moves) {
        Position newPos = copyPosition(pos);
        updateBitboardsLocal(newPos, move);
        threadPos.update(newPos);

        SearchResult result = negamax(
            threadPos,
            depth - 1,
            -beta,
            -alpha,
            color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE,
            ply + 1
        );

        threadPos.update(pos);
        
        int score = -result.score;
        if (score > best.score) {
            best.score = score;
            best.bestMove = move;
        }
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }

    return best;
}

SearchResult quiescenceSearch(ThreadSafePosition& threadPos, int alpha, int beta, Move::Color color) {
    Position pos = threadPos.get();
    int standPat = evaluatePositionLocal(pos);
    if (color == Move::Color::BLACK) standPat = -standPat;
    
    if (standPat >= beta)
        return SearchResult{beta, Move()};  // Use uniform initialization
    if (alpha < standPat)
        alpha = standPat;

    std::vector<Move> captures;
    for (const Move& move : generateMovesLocal(pos, color)) {
        if (move.isCapture || move.isPromotion) {
            captures.push_back(move);
        }
    }
    
    std::sort(captures.begin(), captures.end(),
        [](const Move& a, const Move& b) {
            if (a.isPromotion != b.isPromotion)
                return a.isPromotion > b.isPromotion;
            return getPieceValue(a.pieceType) < getPieceValue(b.pieceType);
        });

    SearchResult best{standPat, Move()};  // Use uniform initialization
    for (const Move& move : captures) {
        Position newPos = copyPosition(pos);
        updateBitboardsLocal(newPos, move);
        threadPos.update(newPos);
        
        SearchResult result = quiescenceSearch(
            threadPos,
            -beta,
            -alpha,
            color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE
        );
        
        threadPos.update(pos);
        int score = -result.score;
        
        if (score >= beta)
            return SearchResult{beta, move};  // Use uniform initialization
            
        if (score > alpha) {
            alpha = score;
            best = SearchResult{score, move};  // Use uniform initialization
        }
    }
    
    return best;
}

Move findBestMove(Move::Color color, int maxDepth) {
    // Create thread-safe position
    ThreadSafePosition threadPos(saveBitboards());
    Position initialPos = threadPos.get();
    
    std::vector<Move> rootMoves = generateMovesLocal(initialPos, color);
    sortMoves(rootMoves);

    struct ThreadResult {
        std::atomic<int> score{-KING_VALUE * 2};
        Move bestMove;
        std::mutex mutex;
    };
    auto result = std::make_shared<ThreadResult>();

    // Calculate optimal thread count
    const int numThreads = std::min(
        static_cast<int>(std::thread::hardware_concurrency()),
        static_cast<int>(rootMoves.size())
    );

    std::vector<std::thread> threads;
    
    // Launch threads with round-robin move distribution
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&threadPos, &result, &rootMoves, i, numThreads, maxDepth, color]() {
            ThreadSafePosition localThreadPos(threadPos.get());
            int localBestScore = -KING_VALUE * 2;
            Move localBestMove;

            // Process moves assigned to this thread
            for (int moveIdx = i; moveIdx < rootMoves.size(); moveIdx += numThreads) {
                Position currentPos = localThreadPos.get();
                updateBitboardsLocal(currentPos, rootMoves[moveIdx]);
                localThreadPos.update(currentPos);

                auto searchResult = negamax(
                    localThreadPos,
                    maxDepth - 1,
                    -KING_VALUE * 2,
                    KING_VALUE * 2,
                    color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE,
                    1
                );

                int score = -searchResult.score;
                if (score > localBestScore) {
                    localBestScore = score;
                    localBestMove = rootMoves[moveIdx];
                }
            }

            // Update global best if local is better
            if (localBestScore > result->score) {
                std::lock_guard<std::mutex> lock(result->mutex);
                if (localBestScore > result->score) {
                    result->score = localBestScore;
                    result->bestMove = localBestMove;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return result->bestMove;
}

// end of file