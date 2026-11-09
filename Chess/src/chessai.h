#ifndef CHESSAI_H
#define CHESSAI_H

#include <QObject>
#include <QThread>
#include "types.h"
#include "chessgame.h"

class ChessAI : public QObject {
    Q_OBJECT

public:
    explicit ChessAI(QObject *parent = nullptr);

    void findBestMoveAsync(ChessGame *game, PieceColor aiColor, AIDifficulty difficulty);
    Move findBestMove(ChessGame *game, PieceColor aiColor, AIDifficulty difficulty);

signals:
    void bestMoveReady(const Move &move);

private:
    int evaluate(ChessGame *game);
    int evaluatePiece(PieceType type, PieceColor color, int r, int c);
    int minimax(ChessGame *game, int depth, int alpha, int beta, bool maximizingPlayer);
    int quiescence(ChessGame *game, int alpha, int beta, bool maximizingPlayer, int depthLimit);
    void scoreAndSortMoves(QVector<Move> &moves, ChessGame *game);

    PieceColor m_aiColor = PieceColor::Black;
};

#endif // CHESSAI_H
