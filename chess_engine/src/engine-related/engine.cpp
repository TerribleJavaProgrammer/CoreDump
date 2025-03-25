#include "engine-related/engine.h"

Move findBestMove(Move::Color color, int maxDepth, double timeLimitSeconds, bool debug) {
    nodeCount = 0;
    leafNodeCount = 0;
    ThreadSafePosition threadPos(currPosition.getPosition());
    Position initialPos = threadPos.get();

    std::vector<Move> rootMoves = generateMoves(initialPos, color);
    sortMoves(rootMoves, initialPos, 0, color);
    if (debug) {
        std::cout << "============================\n";
        std::cout << "Starting Search\n";
        std::cout << "Root Moves: " << rootMoves.size() << "\n";
        std::cout << "Max Depth: " << maxDepth << "\n";
        std::cout << "Threads: " << std::thread::hardware_concurrency() << "\n";
        std::cout << "============================\n";
    }
    struct ThreadResult {
        std::atomic<int> score{-KING_VALUE * 2};
        Move bestMove;
        std::mutex mutex;
    };

    auto result = std::make_shared<ThreadResult>();

    auto startTime = std::chrono::high_resolution_clock::now();

    const int numThreads = std::min(
        static_cast<int>(std::thread::hardware_concurrency()),
        static_cast<int>(rootMoves.size())
    );

    std::atomic<size_t> moveIndex{0};  

    Move bestMoveSoFar;

    // **Iterative Deepening Loop**
    for (int depth = 1; depth <= maxDepth; depth++) {
        moveIndex = 0;  // Reset move index for new depth
        std::vector<std::thread> threads;
        for (int threadId = 0; threadId < numThreads; threadId++) {
            threads.emplace_back([&, threadId]() {
                int BestScore = -KING_VALUE * 2;
                Move BestMove;
                Position localPosition = threadPos.get();

                while (true) {
                    // **Check if time is up**
                    auto elapsedTime = std::chrono::duration<double>(
                        std::chrono::high_resolution_clock::now() - startTime
                    ).count();
                    if (elapsedTime >= timeLimitSeconds) return;  

                    size_t index = moveIndex.fetch_add(1);  
                    if (index >= rootMoves.size()) break;

                    Position currentPos = localPosition;
                    makeMove(currentPos, rootMoves[index]);

                    SearchResult searchResult;
                    searchResult.score = negamax(
                        currentPos,  
                        depth,
                        -KING_VALUE * 2,
                        KING_VALUE * 2,
                        (color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE),
                        0,
                        startTime,
                        timeLimitSeconds
                    );
                    // int negamax(Position& pos, int depth, int alpha, int beta, Move::Color color, int ply, std::chrono::_V2::system_clock::time_point startTime, double timeLimit)
                    int score = -searchResult.score;
                    leafNodeCount++;
                    undoMove(currentPos, rootMoves[index]);

                    if (score > BestScore) {
                        BestScore = score;
                        BestMove = rootMoves[index];
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(result->mutex);
                    if (BestScore > result->score) {
                        result->score = BestScore;
                        result->bestMove = BestMove;
                    }
                }
            });
        }

        for (auto& thread : threads) thread.join();

        bestMoveSoFar = result->bestMove;

        double elapsedTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - startTime
        ).count();
        if (debug) std::cout << ">> Current Depth: " << depth
                    << " | Best Move: " << bestMoveSoFar.fromSquare 
                    << " -> " << bestMoveSoFar.toSquare
                    << " | Score: " << result->score
                    << " | Node Count: " << nodeCount
                    << " | Leaf Node Count: " << leafNodeCount
                    << " | Time Elapsed: " << elapsedTime << "s\n";
        if (elapsedTime >= timeLimitSeconds) {
            if (debug) std::cout << "Time limit reached. Stopping search at depth " << depth << ".\n";
            break;
        }
    }

    double totalTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - startTime
    ).count();
    double nps = nodeCount / totalTime;
    double lnps = leafNodeCount / totalTime;
    if (debug) {
        std::cout << "============================\n";
        std::cout << "Search Completed!\n";
        std::cout << "Total Time: " << totalTime << "s\n";
        std::cout << "Nodes Searched: " << nodeCount << "\n";
        std::cout << "Nodes Per Second (NPS): " << nps << "\n";
        std::cout << "Leaf Nodes Evaluated: " << leafNodeCount << "\n";
        std::cout << "Leaf Nodes Per Second (LNPS): " << lnps << "\n";
        std::cout << "Final Best Move: " << bestMoveSoFar.fromSquare << " -> " 
                << bestMoveSoFar.toSquare << " (Score: " << result->score << ")\n";
        std::cout << "============================\n";
    }
    return bestMoveSoFar;
}