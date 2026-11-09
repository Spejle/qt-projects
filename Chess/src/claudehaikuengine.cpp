#include "claudehaikuengine.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

ClaudeHaikuEngine::ClaudeHaikuEngine(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ClaudeHaikuEngine::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &ClaudeHaikuEngine::onProcessError);
}

void ClaudeHaikuEngine::cancelRequest() {
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void ClaudeHaikuEngine::requestMove(ChessGame *game, PieceColor aiColor) {
    cancelRequest();

    m_game = game;
    m_aiColor = aiColor;

    QVector<Move> legalMoves = game->generateLegalMoves(aiColor);
    if (legalMoves.isEmpty()) {
        emit errorOccurred(tr("No legal moves available."));
        return;
    }

    // Build legal moves in UCI format
    QStringList uciList;
    for (const auto &m : legalMoves) {
        QString uci = m.from.toAlgebraic() + m.to.toAlgebraic();
        if (m.promotion != PieceType::None) {
            switch (m.promotion) {
                case PieceType::Queen:  uci += "q"; break;
                case PieceType::Rook:   uci += "r"; break;
                case PieceType::Bishop: uci += "b"; break;
                case PieceType::Knight: uci += "n"; break;
                default: break;
            }
        }
        uciList.append(uci);
    }

    QStringList historyList;
    for (const auto &m : game->moveHistory()) {
        historyList.append(m.sanNotation);
    }
    QString historyStr = historyList.isEmpty() ? QStringLiteral("Game start") : historyList.join(" ");

    QString colorStr = (aiColor == PieceColor::White) ? "White" : "Black";
    QString fenStr = game->exportFEN();

    QString prompt = QString(
        "You are playing chess as %1 with maximum tactical precision. "
        "The board FEN is: '%2'. "
        "Move history: %3. "
        "Your legal moves in UCI format are: [%4]. "
        "Analyze the position with high effort and pick the single best winning move. "
        "Provide a 1-sentence strategic explanation. "
        "Respond STRICTLY in JSON format with no other text outside the JSON block: "
        "{\"uci\": \"<chosen_move>\", \"commentary\": \"<one sentence tactical reason>\"}"
    ).arg(colorStr, fenStr, historyStr, uciList.join(", "));

    emit thinkingProgress(tr("Claude Haiku (High Effort) is deeply analyzing the board... ⚡"));

    QStringList args;
    args << "-p";
    args << "--model" << "haiku";
    args << "--effort" << "high";
    args << prompt;

    m_process->start("/usr/local/bin/claude", args);
}

void ClaudeHaikuEngine::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        // Fallback to first legal move
        if (m_game) {
            auto legal = m_game->generateLegalMoves(m_aiColor);
            if (!legal.isEmpty()) {
                emit moveReady(legal[0], tr("Claude Haiku fallback move executed."));
                return;
            }
        }
        emit errorOccurred(tr("Claude process exited with code %1").arg(exitCode));
        return;
    }

    QString output = QString::fromUtf8(m_process->readAllStandardOutput());

    // Extract JSON block { ... }
    int startIdx = output.indexOf('{');
    int endIdx = output.lastIndexOf('}');

    QString uci;
    QString commentary;

    if (startIdx != -1 && endIdx > startIdx) {
        QString jsonStr = output.mid(startIdx, endIdx - startIdx + 1);
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            uci = obj["uci"].toString().trimmed();
            commentary = obj["commentary"].toString().trimmed();
        }
    }

    if (m_game) {
        QVector<Move> legal = m_game->generateLegalMoves(m_aiColor);
        Move selectedMove = findMoveFromUci(uci, legal);
        if (selectedMove.from.isValid()) {
            QString fullComm = commentary.isEmpty()
                ? tr("Claude Haiku: Played %1.").arg(selectedMove.sanNotation)
                : QString("Claude Haiku (High Effort): %1").arg(commentary);
            selectedMove.commentary = fullComm;
            emit moveReady(selectedMove, fullComm);
            return;
        } else if (!legal.isEmpty()) {
            legal[0].commentary = tr("Claude Haiku: Selected move %1.").arg(legal[0].sanNotation);
            emit moveReady(legal[0], legal[0].commentary);
            return;
        }
    }

    emit errorOccurred(tr("No matching legal move found in Claude output."));
}

void ClaudeHaikuEngine::onProcessError(QProcess::ProcessError error) {
    Q_UNUSED(error);
    if (m_game) {
        auto legal = m_game->generateLegalMoves(m_aiColor);
        if (!legal.isEmpty()) {
            emit moveReady(legal[0], tr("Claude process error, playing safe fallback."));
        }
    }
}

Move ClaudeHaikuEngine::findMoveFromUci(const QString &uci, const QVector<Move> &legalMoves) {
    if (uci.length() < 4) return Move();

    QString fromAlg = uci.mid(0, 2);
    QString toAlg = uci.mid(2, 2);

    BoardPos from = BoardPos::fromAlgebraic(fromAlg);
    BoardPos to = BoardPos::fromAlgebraic(toAlg);

    PieceType promo = PieceType::None;
    if (uci.length() >= 5) {
        char p = uci[4].toLatin1();
        if (p == 'q' || p == 'Q') promo = PieceType::Queen;
        else if (p == 'r' || p == 'R') promo = PieceType::Rook;
        else if (p == 'b' || p == 'B') promo = PieceType::Bishop;
        else if (p == 'n' || p == 'N') promo = PieceType::Knight;
    }

    for (const auto &m : legalMoves) {
        if (m.from == from && m.to == to) {
            if (promo != PieceType::None) {
                if (m.promotion == promo) return m;
            } else {
                return m;
            }
        }
    }

    return Move();
}
