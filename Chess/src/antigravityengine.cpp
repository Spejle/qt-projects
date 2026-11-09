#include "antigravityengine.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

QString AntigravityEngine::stateFilePath() {
    return QStringLiteral("/home/spejle/Chess/antigravity_state.json");
}

QString AntigravityEngine::moveFilePath() {
    return QStringLiteral("/home/spejle/Chess/antigravity_move.json");
}

AntigravityEngine::AntigravityEngine(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_autoThinkTimer(new QTimer(this))
    , m_watcher(new QFileSystemWatcher(this))
{
    m_pollTimer->setInterval(200);
    connect(m_pollTimer, &QTimer::timeout, this, &AntigravityEngine::checkForManualMove);

    m_autoThinkTimer->setSingleShot(true);
    connect(m_autoThinkTimer, &QTimer::timeout, this, &AntigravityEngine::onAutoThinkTimeout);

    // Watch directory for move file creation
    m_watcher->addPath(QDir("/home/spejle/Chess").absolutePath());
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &AntigravityEngine::checkForManualMove);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &AntigravityEngine::checkForManualMove);
}

void AntigravityEngine::requestMove(ChessGame *game, PieceColor aiColor) {
    m_game = game;
    m_aiColor = aiColor;
    m_waitingForMove = true;

    // Check if an existing move file is already waiting
    if (tryReadManualMove()) {
        return;
    }

    // Write current board state for Antigravity inspection
    writeGameStateFile();

    emit thinkingProgress(tr("Antigravity is calculating strategic move... 🚀"));

    // Start polling for manual move from Antigravity session
    m_pollTimer->start();

    // Auto-think timeout: after 700ms, compute move natively if no manual move injected
    m_autoThinkTimer->start(700);
}

void AntigravityEngine::cancelRequest() {
    m_waitingForMove = false;
    m_pollTimer->stop();
    m_autoThinkTimer->stop();
}

void AntigravityEngine::writeGameStateFile() {
    if (!m_game) return;

    QJsonObject root;
    root["fen"] = m_game->exportFEN();
    root["turn"] = (m_aiColor == PieceColor::White) ? "white" : "black";
    root["status"] = "waiting_for_antigravity";

    QJsonArray historyArr;
    for (const auto &m : m_game->moveHistory()) {
        historyArr.append(m.sanNotation);
    }
    root["history"] = historyArr;

    QJsonArray legalArr;
    QVector<Move> legalMoves = m_game->generateLegalMoves(m_aiColor);
    for (const auto &m : legalMoves) {
        QJsonObject mObj;
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
        mObj["uci"] = uci;
        mObj["san"] = m.sanNotation.isEmpty() ? uci : m.sanNotation;
        mObj["isCapture"] = !m.capturedPiece.isEmpty() || m.isEnPassant;
        legalArr.append(mObj);
    }
    root["legalMoves"] = legalArr;

    QFile file(stateFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void AntigravityEngine::checkForManualMove() {
    if (!m_waitingForMove) return;
    tryReadManualMove();
}

bool AntigravityEngine::tryReadManualMove() {
    QFile moveFile(moveFilePath());
    if (!moveFile.exists() || !moveFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = moveFile.readAll();
    moveFile.close();
    QFile::remove(moveFilePath());

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject obj = doc.object();
    QString uci = obj["uci"].toString();
    QString comm = obj["commentary"].toString();

    if (m_game) {
        QVector<Move> legal = m_game->generateLegalMoves(m_aiColor);
        Move move = findMoveFromUci(uci, legal);
        if (move.from.isValid()) {
            cancelRequest();
            if (comm.isEmpty()) {
                comm = tr("Antigravity: Playing %1.").arg(move.sanNotation);
            }
            emit moveReady(move, comm);
            return true;
        }
    }

    return false;
}

void AntigravityEngine::onAutoThinkTimeout() {
    if (!m_waitingForMove) return;

    if (tryReadManualMove()) return;

    QString commentary;
    Move bestMove = computeNativeMove(commentary);

    cancelRequest();

    if (bestMove.from.isValid()) {
        emit moveReady(bestMove, commentary);
    } else {
        emit errorOccurred(tr("No legal moves available."));
    }
}

Move AntigravityEngine::computeNativeMove(QString &outCommentary) {
    if (!m_game) return Move();

    QVector<Move> legalMoves = m_game->generateLegalMoves(m_aiColor);
    if (legalMoves.isEmpty()) return Move();

    QString fen = m_game->exportFEN();
    QString fenPrefix = fen.section(' ', 0, 3);

    // 1. Antigravity Opening Book (Pure C++)
    struct BookEntry {
        const char *fen;
        const char *uci;
        const char *commentary;
    };

    static const BookEntry openingBook[] = {
        // Initial move if White
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", "e2e4", "1. e4 — The King's Pawn! Fischer called it 'Best by test.' Directly claiming the center."},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", "d2d4", "1. d4 — The Queen's Pawn! Seizing central control with solid piece protection."},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", "g1f3", "1. Nf3 — The Réti Opening! Developing the knight flexibly before locking pawn structure."},

        // Response to 1. e4
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3", "e7e5", "1... e5 — The Open Game! Clashing directly in the center against your King's Pawn."},
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3", "c7c5", "1... c5 — The Sicilian Defense! Fighting for central control asymmetrically."},
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3", "e7e6", "1... e6 — The French Defense! Solid pawn chain preparing a dynamic d5 counter-strike."},
        {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3", "c7c6", "1... c6 — The Caro-Kann Defense! Solid structure keeping bishop diagonals open."},

        // Response to 1. d4
        {"rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3", "g8f6", "1... Nf6 — The Indian Defenses! Hypermodern control stopping e4."},
        {"rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3", "d7d5", "1... d5 — Classical Closed Game! Meeting White head-on in the center."},

        // After 1. e4 e5 2. Nf3
        {"rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq -", "b8c6", "2... Nc6 — Defending e5 while developing the knight to its best post."},
        {"rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq -", "g8f6", "2... Nf6 — Petroff Defense! An immediate counter-attack on your e4 pawn."},

        // After 1. e4 c5 2. Nf3
        {"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq -", "d7d6", "2... d6 — Classical Sicilian preparation for rapid piece development."},
        {"rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq -", "e7e6", "2... e6 — Flexible Sicilian pawn formation heading into Kan/Taimanov lines."}
    };

    QVector<QPair<QString, QString>> bookMatches;
    for (const auto &entry : openingBook) {
        if (fenPrefix.startsWith(entry.fen)) {
            bookMatches.append({QString::fromLatin1(entry.uci), QString::fromLatin1(entry.commentary)});
        }
    }

    if (!bookMatches.isEmpty()) {
        int idx = QRandomGenerator::global()->bounded(bookMatches.size());
        auto match = bookMatches[idx];
        Move bookMove = findMoveFromUci(match.first, legalMoves);
        if (bookMove.from.isValid()) {
            outCommentary = QString("Antigravity: %1").arg(match.second);
            return bookMove;
        }
    }

    // 2. Tactical & Positional Strategic Evaluator
    Move bestMove = legalMoves[0];
    int bestScore = -999999;

    for (const auto &m : legalMoves) {
        int score = 0;
        bool isCapture = (!m.capturedPiece.isEmpty() || m.isEnPassant);

        if (isCapture) {
            score += 250 + (m.capturedPiece.value() * 10 - m.movedPiece.value());
        }
        if (m.isCastling) {
            score += 180;
        }
        if (m.promotion != PieceType::None) {
            score += 850;
        }

        // Center control squares
        if ((m.to.row == 3 || m.to.row == 4) && (m.to.col == 3 || m.to.col == 4)) {
            score += 50;
        } else if ((m.to.row >= 2 && m.to.row <= 5) && (m.to.col >= 2 && m.to.col <= 5)) {
            score += 25;
        }

        // Random subtle variation for organic play
        score += QRandomGenerator::global()->bounded(0, 30);

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    // Generate Natural Language Commentary in Pure C++
    bool isCapture = (!bestMove.capturedPiece.isEmpty() || bestMove.isEnPassant);
    if (isCapture) {
        outCommentary = QString("Antigravity: Tactical exchange on %1! Calculating favorable piece imbalances.")
                            .arg(bestMove.to.toAlgebraic());
    } else if (bestMove.isCastling) {
        outCommentary = QString("Antigravity: Castling to safety! King protected and Rook activated on the central file.");
    } else if (bestMove.movedPiece.type == PieceType::Knight) {
        outCommentary = QString("Antigravity: Repositioning the Knight to %1 for optimal multi-directional outpost control.")
                            .arg(bestMove.to.toAlgebraic());
    } else if (bestMove.movedPiece.type == PieceType::Bishop) {
        outCommentary = QString("Antigravity: Unleashing the Bishop along an open diagonal targeting %1.")
                            .arg(bestMove.to.toAlgebraic());
    } else if (bestMove.movedPiece.type == PieceType::Rook) {
        outCommentary = QString("Antigravity: Controlling the open file with the Rook on %1.")
                            .arg(bestMove.to.toAlgebraic());
    } else if (bestMove.movedPiece.type == PieceType::Queen) {
        outCommentary = QString("Antigravity: Maneuvering the Queen to %1 into an active attacking posture.")
                            .arg(bestMove.to.toAlgebraic());
    } else if (bestMove.movedPiece.type == PieceType::Pawn) {
        outCommentary = QString("Antigravity: Advancing pawn to %1 to claim central space and restrict your pieces.")
                            .arg(bestMove.to.toAlgebraic());
    } else {
        outCommentary = QString("Antigravity: Moving King to %1 to maintain piece safety.")
                            .arg(bestMove.to.toAlgebraic());
    }

    return bestMove;
}

Move AntigravityEngine::findMoveFromUci(const QString &uci, const QVector<Move> &legalMoves) {
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
