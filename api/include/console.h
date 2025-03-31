#pragma once

#include "engine-related/engine.h"
#include <iostream>
#include <sstream>
#include <string>

namespace coredump
{
    inline PieceType getPromotionPiece(char piece)
    {
        switch (piece)
        {
        case 'Q':
            return PieceType::QUEEN;
        case 'N':
            return PieceType::KNIGHT;
        case 'B':
            return PieceType::BISHOP;
        case 'R':
            return PieceType::ROOK;
        default:
            return PieceType::NONE;
        }
    }
    int start_console();
}