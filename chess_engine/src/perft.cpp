// FINISHED

#include "perft.h"

// Perft function (single-threaded)
uint64_t perft(const Position& pos, int depth, Move::Color color) {
    if (depth == 0) return 1;

    std::vector<Move> moves = generateMoves(pos, color);
    uint64_t nodes = 0;

    for (const Move& move : moves) {
        Position newPos = pos;
        newPos.updatePosition(move);
        nodes += perft(newPos, depth - 1, (color == Move::Color::WHITE) ? Move::Color::BLACK : Move::Color::WHITE);
    }
    return nodes;
}

// Perft function (multi-threaded)
uint64_t parallelPerft(const Position& pos, int depth, Move::Color color, int numThreads) {
    if (depth == 0) return 1;

    std::vector<Move> moves = generateMoves(pos, color);
    size_t numMoves = moves.size();
    std::atomic<uint64_t> totalNodes = 0;

    // Debug: Print total moves
    std::cout << "Parallel Perft: Depth " << depth << " | " << numMoves 
              << " moves found | " << numThreads << " threads\n" << std::flush;
    
    // Restrict thread count to avoid excessive overhead
    size_t threadCount = std::min(static_cast<size_t>(numThreads), numMoves);
    std::vector<std::future<uint64_t>> futures;

    size_t chunkSize = numMoves / threadCount;
    size_t remaining = numMoves % threadCount; // Distribute remainder moves evenly

    size_t start = 0;
    for (size_t i = 0; i < threadCount; ++i) {
        size_t end = start + chunkSize + (i < remaining ? 1 : 0);

        // Launch threads asynchronously
        futures.push_back(std::async(std::launch::async, [&, start, end]() {
            uint64_t localNodes = 0;
            for (size_t j = start; j < end; ++j) {
                Position newPos = pos;
                newPos.updatePosition(moves[j]);
                uint64_t nodes = perft(newPos, depth - 1, 
                    (color == Move::Color::WHITE) ? Move::Color::BLACK : Move::Color::WHITE);
                
                localNodes += nodes;
                
                // Debug: Ensure each move contributes to perft
                std::cout << "Move " << j << " computed " << nodes << " nodes\n" << std::flush;
            }

            totalNodes.fetch_add(localNodes, std::memory_order_relaxed);
            return localNodes;
        }));

        start = end;
    }

    // Wait for all threads
    uint64_t finalCount = 0;
    for (auto& future : futures) {
        try {
            finalCount += future.get();
        } catch (const std::exception& e) {
            std::cerr << "Thread exception: " << e.what() << std::endl;
        }
    }

    return finalCount;
}