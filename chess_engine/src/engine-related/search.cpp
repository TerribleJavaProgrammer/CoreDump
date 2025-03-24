#include "engine-related/search/search.h"

std::atomic<uint64_t> nodeCount = 0;
std::atomic<uint64_t> leafNodeCount = 0;

int negamax(Position& pos, int depth, int alpha, int beta, Move::Color color, int ply, 
    std::chrono::high_resolution_clock::time_point startTime, double timeLimit) {
    nodeCount++;
    // Transposition Table Lookup
    TTEntry* ttEntry = probeTT(pos.zobristKey, depth, alpha, beta);
    if (ttEntry && ttEntry->depth >= depth) { 
        if (ttEntry->flag == EXACT) return ttEntry->score;
        if (ttEntry->flag == LOWERBOUND && ttEntry->score >= beta) return ttEntry->score;
        if (ttEntry->flag == UPPERBOUND && ttEntry->score <= alpha) return ttEntry->score;
    }

    // Base Case: Quiescence Search at Depth 0
    if (depth == 0) {
        return quiescenceSearch(pos, alpha, beta, color, ply);
    }

    std::vector<Move> moves = generateMoves(pos, color);
    sortMoves(moves, pos, ply, color);  // Best move ordering

    // Checkmate / Stalemate Detection
    if (moves.empty()) return (isInCheck(pos, color) ? -KING_VALUE : 0);

    
    // **Null Move Pruning** (Skip losing positions)
    /*if (depth >= 3 && !isInCheck(pos, color)) {
        int score = -negamax(pos, depth - 3, -beta, -beta + 1, color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE, ply + 1);
        if (score >= beta) return beta;  // Beta cutoff (opponent is winning)
    }*/

    int bestScore = -INF;
    Move bestMove;
    bool isPV = false;  // Principal Variation (best line so far)

    for (size_t i = 0; i < moves.size(); i++) {
        auto elapsedTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - startTime
        ).count();
        if (elapsedTime >= timeLimit) return evaluatePosition(pos, color);
        Move& move = moves[i];

        //  **Futility Pruning** (Skip obviously bad moves)
        if (depth <= 3 && !move.isCapture && !isInCheck(pos, color) && evaluatePosition(pos, color) + 200 <= alpha) {
            continue;
        }

        makeMove(pos, move);

        int searchDepth = depth - 1;

        //  **Late Move Reductions (LMR)**
        if (!isPV && i >= 4 && !move.isCapture && depth >= 3) {
            searchDepth -= std::min(2, depth / 2);  // Reduce depth more aggressively
        }        

        int score = -negamax(pos, searchDepth, -beta, -alpha, color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE, ply + 1, startTime, timeLimit);
        undoMove(pos, move);

        if (score >= beta) {
            // **Beta Cutoff: Store killer move & history heuristic**
            if (!move.isCapture) {
                // SEG FAULT
                //storeKillerMove(move, ply);
                //storeHistoryHeuristic(move, depth, color);
            }
            return beta;  // Prune
        }

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            isPV = true;  // Mark as Principal Variation
        }

        alpha = std::max(alpha, score);
    }

    // Store result in Transposition Table
    TTFlag flag = (bestScore <= alpha) ? UPPERBOUND : ((bestScore >= beta) ? LOWERBOUND : EXACT);
    storeTT(pos.computeHash(), depth, bestScore, bestMove, flag);
    return bestScore;
}

int quiescenceSearch(Position& pos, int alpha, int beta, Move::Color color, int ply) {
    int standPat = evaluatePosition(pos, color);
    if (standPat >= beta) return beta;  // Beta cutoff
    if (standPat > alpha) alpha = standPat;

    std::vector<Move> captures = generateCaptures(pos, color);
    sortMoves(captures, pos, ply, color);

    for (Move& move : captures) {
        // Skip bad captures using SEE
        if (!SEE(move)) continue; 

        Position newPos = pos;
        makeMove(newPos, move);
        leafNodeCount++;
        int score = -quiescenceSearch(newPos, -beta, -alpha, 
                        (color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE), ply + 1);
        undoMove(newPos, move);

        if (score >= beta) return beta;  // Beta cutoff
        alpha = std::max(alpha, score);
    }

    return alpha;
}