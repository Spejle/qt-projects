#include "chessai.h"
#include <QRandomGenerator>
#include <algorithm>

// Standard Piece-Square Tables (from White's perspective)
static const int pawnPST[8][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0},
    { 50,  50,  50,  50,  50,  50,  50,  50},
    { 10,  10,  20,  30,  30,  20,  10,  10},
    {  5,   5,  10,  25,  25,  10,   5,   5},
    {  0,   0,   0,  20,  20,   0,   0,   0},
    {  5,  -5, -10,   0,   0, -10,  -5,   5},
    {  5,  10,  10, -20, -20,  10,  10,   5},
    {  0,   0,   0,   0,   0,   0,   0,   0}
};

static const int knightPST[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20,   0,   0,   0,   0, -20, -40},
    {-30,   0,  10,  15,  15,  10,   0, -30},
    {-30,   5,  15,  20,  20,  15,   5, -30},
    {-30,   0,  15,  20,  20,  15,   0, -30},
    {-30,   5,  10,  15,  15,  10,   5, -30},
    {-40, -20,   0,   5,   5,   0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}
};

static const int bishopPST[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10,   0,   0,   0,   0,   0,   0, -10},
    {-10,   0,   5,  10,  10,   5,   0, -10},
    {-10,   5,   5,  10,  10,   5,   5, -10},
    {-10,   0,  10,  10,  10,  10,   0, -10},
    {-10,  10,  10,  10,  10,  10,  10, -10},
    {-10,   5,   0,   0,   0,   0,   5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

static const int rookPST[8][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0},
    {  5,  10,  10,  10,  10,  10,  10,   5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    {  0,   0,   0,   5,   5,   0,   0,   0}
};

static const int queenPST[8][8] = {
    {-20, -10, -10,  -5,  -5, -10, -10, -20},
    {-10,   0,   0,   0,   0,   0,   0, -10},
    {-10,   0,   5,   5,   5,   5,   0, -10},
    { -5,   0,   5,   5,   5,   5,   0,  -5},
    {  0,   0,   5,   5,   5,   5,   0,  -5},
    {-10,   5,   5,   5,   5,   5,   0, -10},
    {-10,   0,   5,   0,   0,   0,   0, -10},
    {-20, -10, -10,  -5,  -5, -10, -10, -20}
};

static const int kingPST[8][8] = {
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-20, -30, -30, -40, -40, -30, -20, -20},
    {-10, -20, -20, -20, -20, -20, -10, -10},
    { 20,  20,   0,   0,   0,   0,  20,  20},
    { 20,  30,  10,   0,   0,  10,  30,  20}
};

ChessAI::ChessAI(QObject *parent)
    : QObject(parent)
{
}

int ChessAI::evaluatePiece(PieceType type, PieceColor color, int r, int c) {
    int val = 0;
    int tableR = (color == PieceColor::White) ? r : (7 - r);
    int tableC = c;

    switch (type) {
        case PieceType::Pawn:
            val = 100 + pawnPST[tableR][tableC];
            break;
        case PieceType::Knight:
            val = 320 + knightPST[tableR][tableC];
            break;
        case PieceType::Bishop:
            val = 330 + bishopPST[tableR][tableC];
            break;
        case PieceType::Rook:
            val = 500 + rookPST[tableR][tableC];
            break;
        case PieceType::Queen:
            val = 900 + queenPST[tableR][tableC];
            break;
        case PieceType::King:
            val = 20000 + kingPST[tableR][tableC];
            break;
        default:
            break;
    }
    return val;
}

int ChessAI::evaluate(ChessGame *game) {
    int total = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = game->pieceAt(r, c);
            if (p.isEmpty()) continue;
            int score = evaluatePiece(p.type, p.color, r, c);
            if (p.color == PieceColor::White) {
                total += score;
            } else {
                total -= score;
            }
        }
    }
    return total;
}

void ChessAI::scoreAndSortMoves(QVector<Move> &moves, ChessGame *game) {
    // Sort moves: high value captures first
    std::sort(moves.begin(), moves.end(), [game](const Move &a, const Move &b) {
        int scoreA = 0;
        int scoreB = 0;

        if (!a.capturedPiece.isEmpty()) {
            scoreA = 10 * a.capturedPiece.value() - a.movedPiece.value();
        }
        if (a.promotion != PieceType::None) {
            scoreA += 800;
        }

        if (!b.capturedPiece.isEmpty()) {
            scoreB = 10 * b.capturedPiece.value() - b.movedPiece.value();
        }
        if (b.promotion != PieceType::None) {
            scoreB += 800;
        }

        return scoreA > scoreB;
    });
}

int ChessAI::quiescence(ChessGame *game, int alpha, int beta, bool maximizingPlayer, int depthLimit) {
    int standPat = evaluate(game);

    if (depthLimit <= 0) return standPat;

    if (maximizingPlayer) {
        if (standPat >= beta) return beta;
        if (standPat > alpha) alpha = standPat;

        QVector<Move> legal = game->generateLegalMoves(PieceColor::White);
        scoreAndSortMoves(legal, game);

        for (auto &m : legal) {
            if (m.capturedPiece.isEmpty() && m.promotion == PieceType::None) continue;
            game->makeMove(m);
            int score = quiescence(game, alpha, beta, false, depthLimit - 1);
            game->undoMove();

            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        }
        return alpha;
    } else {
        if (standPat <= alpha) return alpha;
        if (standPat < beta) beta = standPat;

        QVector<Move> legal = game->generateLegalMoves(PieceColor::Black);
        scoreAndSortMoves(legal, game);

        for (auto &m : legal) {
            if (m.capturedPiece.isEmpty() && m.promotion == PieceType::None) continue;
            game->makeMove(m);
            int score = quiescence(game, alpha, beta, true, depthLimit - 1);
            game->undoMove();

            if (score <= alpha) return alpha;
            if (score < beta) beta = score;
        }
        return beta;
    }
}

int ChessAI::minimax(ChessGame *game, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) {
        return quiescence(game, alpha, beta, maximizingPlayer, 2);
    }

    PieceColor currentColor = maximizingPlayer ? PieceColor::White : PieceColor::Black;
    QVector<Move> legalMoves = game->generateLegalMoves(currentColor);

    if (legalMoves.isEmpty()) {
        if (game->isKingInCheck(currentColor)) {
            // Checkmate
            return maximizingPlayer ? -100000 - depth : 100000 + depth;
        }
        // Stalemate
        return 0;
    }

    scoreAndSortMoves(legalMoves, game);

    if (maximizingPlayer) {
        int maxEval = -9999999;
        for (auto &m : legalMoves) {
            game->makeMove(m);
            int eval = minimax(game, depth - 1, alpha, beta, false);
            game->undoMove();

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Pruning
        }
        return maxEval;
    } else {
        int minEval = 9999999;
        for (auto &m : legalMoves) {
            game->makeMove(m);
            int eval = minimax(game, depth - 1, alpha, beta, true);
            game->undoMove();

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Pruning
        }
        return minEval;
    }
}

Move ChessAI::findBestMove(ChessGame *game, PieceColor aiColor, AIDifficulty difficulty) {
    m_aiColor = aiColor;
    QVector<Move> legalMoves = game->generateLegalMoves(aiColor);

    if (legalMoves.isEmpty()) return Move();

    // Easy: shallow search + randomness
    if (difficulty == AIDifficulty::Easy) {
        if (QRandomGenerator::global()->bounded(100) < 35) {
            int idx = QRandomGenerator::global()->bounded(legalMoves.size());
            return legalMoves[idx];
        }
    }

    int targetDepth = 2;
    if (difficulty == AIDifficulty::Easy) targetDepth = 1;
    else if (difficulty == AIDifficulty::Medium) targetDepth = 3;
    else if (difficulty == AIDifficulty::Hard) targetDepth = 4;

    scoreAndSortMoves(legalMoves, game);

    Move bestMove = legalMoves[0];
    bool isMax = (aiColor == PieceColor::White);

    int alpha = -9999999;
    int beta = 9999999;

    if (isMax) {
        int bestScore = -9999999;
        for (auto &m : legalMoves) {
            game->makeMove(m);
            int score = minimax(game, targetDepth - 1, alpha, beta, false);
            game->undoMove();

            if (difficulty == AIDifficulty::Easy) {
                score += QRandomGenerator::global()->bounded(-25, 26);
            }

            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
            alpha = std::max(alpha, score);
        }
    } else {
        int bestScore = 9999999;
        for (auto &m : legalMoves) {
            game->makeMove(m);
            int score = minimax(game, targetDepth - 1, alpha, beta, true);
            game->undoMove();

            if (difficulty == AIDifficulty::Easy) {
                score += QRandomGenerator::global()->bounded(-25, 26);
            }

            if (score < bestScore) {
                bestScore = score;
                bestMove = m;
            }
            beta = std::min(beta, score);
        }
    }

    return bestMove;
}

void ChessAI::findBestMoveAsync(ChessGame *game, PieceColor aiColor, AIDifficulty difficulty) {
    // Clone board state to safely search without disturbing GUI thread
    ChessGame *aiGame = new ChessGame();
    aiGame->reset();

    // Replay moves to get exact game state
    const auto &history = game->moveHistory();
    for (auto m : history) {
        aiGame->makeMove(m);
    }

    QThread *thread = QThread::create([this, aiGame, aiColor, difficulty]() {
        Move best = findBestMove(aiGame, aiColor, difficulty);
        delete aiGame;
        emit bestMoveReady(best);
    });

    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
