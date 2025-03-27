#ifndef NULLMOVESTATE_H
#define NULLMOVESTATE_H

#include <stdint.h>

struct NullMoveState
{
    uint64_t zobristKey;
    int enPassantSquare;
};

#endif // NULLMOVESTATE_H