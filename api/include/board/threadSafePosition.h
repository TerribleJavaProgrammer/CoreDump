#pragma once

#include <mutex>
#include "board/position.h"

// Barely used. To prune.
namespace coredump
{
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
}
