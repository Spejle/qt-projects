#ifndef ANTIGRAVITYENGINE_H
#define ANTIGRAVITYENGINE_H

#include <QObject>
#include <QTimer>
#include <QFileSystemWatcher>
#include "types.h"
#include "chessgame.h"

class AntigravityEngine : public QObject {
    Q_OBJECT

public:
    explicit AntigravityEngine(QObject *parent = nullptr);

    void requestMove(ChessGame *game, PieceColor aiColor);
    void cancelRequest();

    static QString stateFilePath();
    static QString moveFilePath();

signals:
    void moveReady(const Move &move, const QString &commentary);
    void thinkingProgress(const QString &message);
    void errorOccurred(const QString &err);

private slots:
    void checkForManualMove();
    void onAutoThinkTimeout();

private:
    void writeGameStateFile();
    bool tryReadManualMove();
    Move computeNativeMove(QString &outCommentary);
    Move findMoveFromUci(const QString &uci, const QVector<Move> &legalMoves);

    ChessGame *m_game = nullptr;
    PieceColor m_aiColor = PieceColor::Black;

    QTimer *m_pollTimer = nullptr;
    QTimer *m_autoThinkTimer = nullptr;
    QFileSystemWatcher *m_watcher = nullptr;

    bool m_waitingForMove = false;
};

#endif // ANTIGRAVITYENGINE_H
