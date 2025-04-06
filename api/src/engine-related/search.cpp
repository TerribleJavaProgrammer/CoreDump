#include "engine-related/search.h"

namespace coredump
{

    // New algorithm: Minimax w/ AB pruning. Used for debugging and testing (not to mention the fact that it doesn't sound like a slur.)
    // Added 4/5/25
    int minimax(std::chrono::high_resolution_clock::time_point startTime, double timeLimit,
        const Position &pos, int depth, int alpha, int beta, Color maximizingColor, Color currentColor) {
        bool noMovesMax = generateMoves(pos, maximizingColor).size() == 0;
        bool noMovesMin = generateMoves(pos, maximizingColor == Color::WHITE ? Color::BLACK : Color::WHITE).size() == 0;
        bool isCheckMax = isInCheck(pos, maximizingColor);
        bool isCheckMin = isInCheck(pos, maximizingColor == Color::WHITE ? Color::BLACK : Color::WHITE);

        bool maxLoss = noMovesMax && isCheckMax;
        bool minLoss = noMovesMin && isCheckMin;
        bool stalemate = noMovesMax && noMovesMin && !isCheckMax && !isCheckMin;

        if (depth == 0 || maxLoss || minLoss || stalemate) {
            return evaluatePosition(pos, maximizingColor);
        }

        if (maximizingColor == currentColor) {
            int bestScore = -INT_MAX;
            std::vector<Move> moves = generateMoves(pos, maximizingColor);
            for (const Move &move : moves) {
                Position tempPos(pos, move);
                auto elapsedTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                if (elapsedTime >= timeLimit) {
                    return std::max(bestScore, evaluatePosition(tempPos, maximizingColor));
                }
                int score = minimax(startTime, timeLimit, tempPos, depth - 1, alpha, beta,
                    maximizingColor, currentColor == Color::WHITE ? Color::BLACK : Color::WHITE);
                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, score);
                if (beta <= alpha) {
                    break; // Beta cutoff
                }
            }
            return bestScore;
        } else {
            int bestScore = INT_MAX;
            std::vector<Move> moves = generateMoves(pos, currentColor);
            for (const Move &move : moves) {
                Position tempPos(pos, move);
                auto elapsedTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                if (elapsedTime >= timeLimit) {
                    return std::min(bestScore, evaluatePosition(tempPos, currentColor));
                }
                int score = minimax(startTime, timeLimit, tempPos, depth - 1, alpha, beta,
                    maximizingColor, currentColor == Color::WHITE ? Color::BLACK : Color::WHITE);
                bestScore = std::min(bestScore, score);
                beta = std::min(beta, score);
                if (beta <= alpha) {
                    break; // Alpha cutoff
                }
            }
            return bestScore;
        }
    }

    // ! This function is where the magic happens. Optimizing its speed is of upmost importance.
    // Negamax with Alpha-Beta Pruning
    int negamax(const Position &pos, int depth, int alpha, int beta, Color color, int ply,
                std::chrono::high_resolution_clock::time_point startTime, double timeLimit, std::atomic<uint64_t> &nodeCount, std::atomic<uint64_t> &leafNodeCount)
    {
        Position tempPos(pos);
        nodeCount++;
        // Transposition Table Lookup
        TTEntry *ttEntry = probeTT(pos.computeHash(), depth, alpha, beta);
        if (ttEntry && ttEntry->depth >= depth)
        {
            if (ttEntry->flag == EXACT)
                return ttEntry->score;
            if (ttEntry->flag == LOWERBOUND && ttEntry->score >= beta)
                return ttEntry->score;
            if (ttEntry->flag == UPPERBOUND && ttEntry->score <= alpha)
                return ttEntry->score;
        }

        // Base Case: Quiescence Search at Depth 0
        if (depth == 0)
        {
            return quiescenceSearch(pos, alpha, beta, color, ply, leafNodeCount);
        }

        std::vector<Move> moves = generateMoves(pos, color);
        sortMoves(moves, pos, ply, color); // Best move ordering

        // Checkmate / Stalemate Detection
        if (moves.empty())
            return (isInCheck(pos, color) ? -KING_VALUE : 0);

        // // **Null Move Pruning** (Skip losing positions)
        if (depth >= 3 && !isInCheck(pos, color))
        {
            int score = -negamax(pos, depth - 3, -beta, -beta + 1, invertColor(color), ply + 1, startTime, timeLimit, nodeCount, leafNodeCount);
            if (score >= beta)
                return beta; // Beta cutoff (opponent is winning)
        }

        int bestScore = -INT_MAX;
        Move bestMove;
        bool isPV = false; // Principal Variation (best line so far)

        for (size_t i = 0; i < moves.size(); i++)
        {
            auto elapsedTime = std::chrono::duration<double>(
                                   std::chrono::high_resolution_clock::now() - startTime)
                                   .count();
            if (elapsedTime >= timeLimit)
                return evaluatePosition(pos, color);
            const Move &move = moves[i];

            //  **Futility Pruning** (Skip obviously bad moves)
            if (depth <= 3 && !move.isCapture && !isInCheck(pos, color) && evaluatePosition(pos, color) + 200 <= alpha)
            {
                continue;
            }

            tempPos.makeMove(move);
            int searchDepth = depth - 1;
            //  **Late Move Reductions (LMR)**
            if (!isPV && i >= 4 && !move.isCapture && depth >= 3)
            {
                searchDepth -= std::min(2, depth / 2); // Reduce depth more aggressively
            }
            const Color otherColor = invertColor(color);
            // Recursive call to this function but of the other color.
            int score = -negamax(tempPos, searchDepth, -beta, -alpha, otherColor, ply + 1, startTime, timeLimit, nodeCount, leafNodeCount);
            // One by one, we pop out of the recursive calls and undo each move back up the tree
            tempPos.undoMove(move);

            if (score >= beta)
            {
                // **Beta Cutoff: Store killer move & history heuristic**
                //if (!move.isCapture)
                //{
                //    // SEG FAULT
                //    storeKillerMove(move, ply);
                //    storeHistoryHeuristic(move, depth, color);
                //}
                return beta; // Prune
            }

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move;
                isPV = true; // Mark as Principal Variation
            }

            alpha = std::max(alpha, score);
        }

        // Store result in Transposition Table
        TTFlag flag = (bestScore <= alpha) ? UPPERBOUND : ((bestScore >= beta) ? LOWERBOUND : EXACT);
        storeTT(pos.computeHash(), depth, bestScore, bestMove, flag);
        return bestScore;
    }

    // Search all capture moves that stem from this move
    // "Please mom, just one more search! It'll only take a few milliseconds!"
    int quiescenceSearch(const Position &pos, int alpha, int beta, Color color, int ply, std::atomic<uint64_t> &leafNodeCount)
    {
        int standPat = evaluatePosition(pos, color);
        if (standPat >= beta)
            return beta; // Beta cutoff
        if (standPat > alpha)
            alpha = standPat;

        std::vector<Move> captures = generateCaptures(pos, color);
        sortMoves(captures, pos, ply, color);

        for (Move &move : captures)
        {
            // Skip bad captures using SEE
            if (!SEE(move))
                continue;

            const Position tempPos(pos, move);
            leafNodeCount++;
            int score = -quiescenceSearch(tempPos, -beta, -alpha, invertColor(color), ply + 1, leafNodeCount);

            if (score >= beta)
                return beta; // Beta cutoff
            alpha = std::max(alpha, score);
        }

        return alpha;
    }
}
