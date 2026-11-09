#ifndef CLAUDEHAIKUENGINE_H
#define CLAUDEHAIKUENGINE_H

#include <QObject>
#include <QProcess>
#include "types.h"
#include "chessgame.h"

class ClaudeHaikuEngine : public QObject {
    Q_OBJECT

public:
    explicit ClaudeHaikuEngine(QObject *parent = nullptr);

    void requestMove(ChessGame *game, PieceColor aiColor);
    void cancelRequest();

signals:
    void moveReady(const Move &move, const QString &commentary);
    void thinkingProgress(const QString &message);
    void errorOccurred(const QString &err);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    Move findMoveFromUci(const QString &uci, const QVector<Move> &legalMoves);

    ChessGame *m_game = nullptr;
    PieceColor m_aiColor = PieceColor::Black;
    QProcess *m_process = nullptr;
};

#endif // CLAUDEHAIKUENGINE_H
