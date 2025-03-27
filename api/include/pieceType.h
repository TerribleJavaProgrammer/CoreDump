#ifndef PIECETYPE_H
#define PIECETYPE_H
// Enum for piece types
enum class PieceType
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

char piecePgn(PieceType piece)
{
    switch (piece)
    {
    case PieceType::PAWN:
        return 'P';
    case PieceType::KNIGHT:
        return 'N';
    case PieceType::BISHOP:
        return 'B';
    case PieceType::ROOK:
        return 'R';
    case PieceType::QUEEN:
        return 'Q';
    case PieceType::KING:
        return 'K';
    default:
        return '!';
    }
}

#endif // PIECETYPE_H