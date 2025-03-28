#include "engine-related/prioritization.h"

// Sort moves from 
int sortMoves(std::vector<Move> &moves, const Position &pos, int ply, Color color)
{
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b)
              {
                  int scoreA = 0, scoreB = 0;

                  // Fetch TT best move
                  TTEntry *tt = probeTT(pos.computeHash(), 0, -INF, INF);
                  const Move ttBestMove = (tt ? tt->bestMove : Move());

                  // Prioritize TT Move
                  if (a == ttBestMove)
                      scoreA += 10000;
                  if (b == ttBestMove)
                      scoreB += 10000;

                  // Killer Moves
                  if (a == killerMoves[ply][0])
                      scoreA += 9000;
                  if (b == killerMoves[ply][0])
                      scoreB += 9000;
                  if (a == killerMoves[ply][1])
                      scoreA += 8000;
                  if (b == killerMoves[ply][1])
                      scoreB += 8000;

                  // History Heuristic
                  // Ensure valid move indices before accessing the array
                  int colorIndex = (color == Color::WHITE) ? 0 : 1;

                  if (colorIndex >= 0 && colorIndex < 2 &&
                      a.fromSquare >= 0 && a.fromSquare < 64 &&
                      a.toSquare >= 0 && a.toSquare < 64 &&
                      b.fromSquare >= 0 && b.fromSquare < 64 &&
                      b.toSquare >= 0 && b.toSquare < 64)
                  {

                      scoreA += historyHeuristic[colorIndex][a.fromSquare][a.toSquare];
                      scoreB += historyHeuristic[colorIndex][b.fromSquare][b.toSquare];
                  }

                  // MVV-LVA for captures
                  if (a.isCapture)
                      scoreA += 100 * getPieceValue(a.capturedPieceType) - getPieceValue(a.pieceType);
                  if (b.isCapture)
                      scoreB += 100 * getPieceValue(b.capturedPieceType) - getPieceValue(b.pieceType);

                  return scoreA > scoreB; // Higher score first
              });

    return 0;
}