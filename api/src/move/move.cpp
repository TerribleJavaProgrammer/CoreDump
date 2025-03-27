#include "move.h"

std::string toAlgebraic(int square)
{
    std::string algebraic = "";
    char file = square % 8 + 'a';
    char rank = square / 8 + '1';
    algebraic += file;
    algebraic += rank;
    return algebraic;
}

struct Move
{

    int fromSquare;  // Source square (0-63)
    int toSquare;    // Destination square (0-63)
    bool isCapture;  // True if this move captures a piece
    bool isCastling; // True if this is a castling move

    PieceType pieceType;             // Type of piece being moved
    Color color;                     // Color of the piece being moved
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
         int enPassantSquare = -1, uint8_t castlingRights = 0, int kingSquare = -1)
        : fromSquare(from), toSquare(to), isCapture(capture), pieceType(type), color(col),
          isCastling(castling), castlingType(castlingType), isPromotion(promotion),
          promotionPiece(promoPiece), capturedPieceBitboard(capturedBitboard),
          capturedPieceType(capturedType), prevWhitePieces(prevWhite),
          prevBlackPieces(prevBlack), prevOccupied(prevOccupiedSquares),
          prevEnPassantSquare(enPassantSquare), prevCastlingRights(castlingRights),
          prevKingSquare(kingSquare) {}

    // Default constructor
    Move()
        : fromSquare(-1),                     // Undefined (no square)
          toSquare(-1),                       // Undefined (no square)
          isCapture(false),                   // Undefined (no capture)
          pieceType(PieceType::NONE),         // Undefined (no piece)
          color(Color::WHITE),                // Default color (can be changed later)
          isCastling(false),                  // Undefined (not castling)
          castlingType(CastlingType::NONE),   // Undefined (no castling)
          isPromotion(false),                 // Undefined (no promotion)
          promotionPiece(PieceType::NONE),    // Undefined (no promotion piece)
          capturedPieceBitboard(0),           // Undefined (no captured piece)
          capturedPieceType(PieceType::NONE), // Undefined (no captured piece type)
          prevWhitePieces(0),                 // Undefined (no previous white pieces)
          prevBlackPieces(0),                 // Undefined (no previous black pieces)
          prevOccupied(0),                    // Undefined (no previous occupied squares)
          prevEnPassantSquare(-1),            // Undefined (no en passant square)
          prevCastlingRights(0),              // Undefined (no previous castling rights)
          prevKingSquare(-1)                  // Undefined (no previous king position)
    {
    }

    std::string getPgn()
    {
        // {piece}{from}{to}
        // promotion is {normal}={new piece}
        // check appends +
        // mate appends # instead
        std::ostringstream pgn;
        if (castlingType == CastlingType::NONE)
        {
            // if black, offset to achieve lower case
            char piece = piecePgn(pieceType);
            if (color == Color::BLACK)
            {
                piece += 32;
            }
            pgn << piece;
            pgn << toAlgebraic(fromSquare);
            if (isCapture)
            {
                pgn << 'x';
            }
            pgn << toAlgebraic(toSquare);
            if (isPromotion)
            {
                pgn << '=';
                pgn << piecePgn(promotionPiece);
            }
        }
        else if (castlingType == CastlingType::KINGSIDE)
        {
            pgn << "O-O";
        }
        else
        {
            pgn << "O-O-O";
        }
        // TODO add check and mate
        return pgn.str();
    }

    // Define the equality operator for Move
    bool operator==(const Move &other) const
    {
        return fromSquare == other.fromSquare &&
               toSquare == other.toSquare &&
               isCapture == other.isCapture &&
               pieceType == other.pieceType &&
               color == other.color &&
               isCastling == other.isCastling &&
               castlingType == other.castlingType &&
               isPromotion == other.isPromotion &&
               promotionPiece == other.promotionPiece &&
               capturedPieceBitboard == other.capturedPieceBitboard &&
               capturedPieceType == other.capturedPieceType &&
               prevWhitePieces == other.prevWhitePieces &&
               prevBlackPieces == other.prevBlackPieces &&
               prevOccupied == other.prevOccupied &&
               prevEnPassantSquare == other.prevEnPassantSquare &&
               prevCastlingRights == other.prevCastlingRights &&
               prevKingSquare == other.prevKingSquare;
    }
};