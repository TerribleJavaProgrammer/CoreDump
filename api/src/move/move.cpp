#include "move/move.h"

namespace coredump
{
    std::string Move::toAlgebraic(int square)
    {
        std::string algebraic = "";
        char file = square % 8 + 'a';
        char rank = square / 8 + '1';
        algebraic += file;
        algebraic += rank;
        return algebraic;
    }

    int Move::fromAlgebraic(char file, char rank)
    {
        return (rank - '1') * 8 + file - 'a';
    }

    // Constructor
    Move::Move(int from, int to, bool capture, PieceType type, Color col, bool castling,
               CastlingType castlingType, bool promotion,
               PieceType promoPiece, uint64_t capturedBitboard,
               PieceType capturedType, uint64_t prevWhite,
               uint64_t prevBlack, uint64_t prevOccupiedSquares,
               int enPassantSquare, uint8_t castlingRights, int kingSquare)
        : fromSquare(from), toSquare(to), isCapture(capture), pieceType(type), color(col),
          isCastling(castling), castlingType(castlingType), isPromotion(promotion),
          promotionPiece(promoPiece), capturedPieceBitboard(capturedBitboard),
          capturedPieceType(capturedType), prevWhitePieces(prevWhite),
          prevBlackPieces(prevBlack), prevOccupied(prevOccupiedSquares),
          prevEnPassantSquare(enPassantSquare), prevCastlingRights(castlingRights),
          prevKingSquare(kingSquare) {}

    Move::Move(int from, int to, Color col) : Move()
    {
        fromSquare = from;
        toSquare = to;
        color = col;
    }

    // Constructor from string
    Move::Move(std::string moveStr, Color col)
        : Move() // Use the default constructor to initialize all fields
    {
        if (moveStr.length() != 5 || moveStr[2] != ' ')
        {
            throw std::invalid_argument("Invalid move format. Expected format: 'a1 a2'");
        }
        fromSquare = fromAlgebraic(moveStr[0], moveStr[1]);
        toSquare = fromAlgebraic(moveStr[3], moveStr[4]);
        color = col;
    }

    // Default constructor
    Move::Move()
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

    std::string Move::getPgn()
    {
        // {piece}{from}{to}
        // promotion is {normal}={new piece}
        // check appends +
        // mate appends # instead
        std::ostringstream pgn;
        if (castlingType == CastlingType::NONE)
        {
            char piece = piecePgn(pieceType);
            // // if black, offset to achieve lower case
            // if (color == Color::BLACK)
            // {
            //     piece += 32;
            // }
            // Pawn is default, so doesn't get added
            if (piece != 'P')
            {
                pgn << piece;
            }
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
    bool Move::operator==(const Move &other) const
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
}
