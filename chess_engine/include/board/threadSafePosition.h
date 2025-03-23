#ifndef THREADSAFEPOSITION_H
#define THREADSAFEPOSITION_H

#include <mutex>
#include "board/position.h"

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
            Position newPos = pos.getPosition();
            newPos.updatePosition(move);
            pos = newPos;
        }
};

#endif // THREADSAFEPOSITION_H