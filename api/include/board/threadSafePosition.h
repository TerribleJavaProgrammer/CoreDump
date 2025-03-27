#ifndef THREADSAFEPOSITION_H
#define THREADSAFEPOSITION_H

#include <mutex>
#include "board/position.h"

// Barely used. To prune.

class ThreadSafePosition
{
private:
    Position pos;
    mutable std::mutex mutex;

public:
    ThreadSafePosition(const Position &initial) : pos(initial) {}

    Position get() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return pos;
    }
};

#endif // THREADSAFEPOSITION_H