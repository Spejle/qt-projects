#include "chessboardwidget.h"
#include "piecerenderer.h"
#include "promotiondialog.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFont>
#include <algorithm>

ChessBoardWidget::ChessBoardWidget(ChessGame *game, QWidget *parent)
    : QWidget(parent)
    , m_game(game)
{
    setMouseTracking(true);
    setMinimumSize(420, 420);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_game, &ChessGame::moveMade, this, [this](const Move &) {
        clearSelection();
        update();
    });

    connect(m_game, &ChessGame::boardReset, this, [this]() {
        clearSelection();
        update();
    });

    connect(m_game, &ChessGame::gameStatusChanged, this, [this](GameStatus) {
        update();
    });
}

void ChessBoardWidget::setTheme(BoardTheme theme) {
    m_theme = theme;
    update();
}

void ChessBoardWidget::setFlipped(bool flipped) {
    m_isFlipped = flipped;
    update();
}

void ChessBoardWidget::clearSelection() {
    m_selectedPos = {-1, -1};
    m_currentLegalMoves.clear();
    m_isDragging = false;
    update();
}

QRectF ChessBoardWidget::boardRect() const {
    const qreal margin = 26.0;
    qreal side = std::min(width() - 2 * margin, height() - 2 * margin);
    if (side < 100) side = 100;
    qreal left = (width() - side) / 2.0;
    qreal top = (height() - side) / 2.0;
    return QRectF(left, top, side, side);
}

qreal ChessBoardWidget::squareSize() const {
    return boardRect().width() / 8.0;
}

QRectF ChessBoardWidget::squareRect(int r, int c) const {
    QRectF bRect = boardRect();
    qreal sq = squareSize();

    int visualCol = m_isFlipped ? (7 - c) : c;
    int visualRow = m_isFlipped ? (7 - r) : r;

    return QRectF(bRect.left() + visualCol * sq, bRect.top() + visualRow * sq, sq, sq);
}

BoardPos ChessBoardWidget::posAtPoint(const QPointF &pt) const {
    QRectF bRect = boardRect();
    if (!bRect.contains(pt)) return {-1, -1};

    qreal sq = squareSize();
    int visualCol = static_cast<int>((pt.x() - bRect.left()) / sq);
    int visualRow = static_cast<int>((pt.y() - bRect.top()) / sq);

    if (visualCol < 0 || visualCol >= 8 || visualRow < 0 || visualRow >= 8) {
        return {-1, -1};
    }

    int c = m_isFlipped ? (7 - visualCol) : visualCol;
    int r = m_isFlipped ? (7 - visualRow) : visualRow;

    return {r, c};
}

bool ChessBoardWidget::canPlayerMovePiece(const Piece &p) const {
    if (!m_interactionAllowed || p.isEmpty()) return false;
    if (m_game->status() == GameStatus::Checkmate ||
        m_game->status() == GameStatus::Stalemate ||
        m_game->status() == GameStatus::DrawFiftyMoves ||
        m_game->status() == GameStatus::DrawInsufficientMaterial ||
        m_game->status() == GameStatus::Resignation) {
        return false;
    }

    if (p.color != m_game->currentTurn()) return false;

    if (m_gameMode == GameMode::PlayerVsAI ||
        m_gameMode == GameMode::PlayerVsAntigravity ||
        m_gameMode == GameMode::PlayerVsClaudeHaiku) {
        if (p.color != m_humanColor) return false;
    }

    return true;
}

void ChessBoardWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QRectF bRect = boardRect();
    qreal sq = squareSize();

    // 1. Draw Board & coordinates
    drawBoard(p, bRect);
    drawCoordinates(p, bRect, sq);

    // 2. Draw Highlights
    drawHighlights(p, sq);

    // 3. Draw Legal Move Markers
    drawLegalMoves(p, sq);

    // 4. Draw Pieces
    drawPieces(p, sq);

    // 5. Draw Dragged Piece
    if (m_isDragging) {
        drawDraggedPiece(p, sq);
    }
}

void ChessBoardWidget::drawBoard(QPainter &p, const QRectF &bRect) {
    QColor lightCol, darkCol;

    switch (m_theme) {
        case BoardTheme::TournamentGreen:
            lightCol = QColor(238, 238, 210);
            darkCol = QColor(118, 150, 86);
            break;
        case BoardTheme::ClassicWood:
            lightCol = QColor(240, 217, 181);
            darkCol = QColor(181, 136, 99);
            break;
        case BoardTheme::ModernSlate:
            lightCol = QColor(223, 227, 232);
            darkCol = QColor(113, 130, 150);
            break;
        case BoardTheme::DarkGlass:
            lightCol = QColor(84, 91, 107);
            darkCol = QColor(42, 45, 53);
            break;
    }

    // Outer board border
    p.setPen(QPen(QColor(40, 40, 40, 60), 2.0));
    p.setBrush(darkCol);
    p.drawRoundedRect(bRect.adjusted(-2, -2, 2, 2), 4, 4);

    p.setPen(Qt::NoPen);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            QRectF sqR = squareRect(r, c);
            bool isLight = ((r + c) % 2 == 0);
            p.setBrush(isLight ? lightCol : darkCol);
            p.drawRect(sqR);
        }
    }
}

void ChessBoardWidget::drawCoordinates(QPainter &p, const QRectF &bRect, qreal sq) {
    QFont f = p.font();
    f.setPointSize(std::max(9, static_cast<int>(sq * 0.17)));
    f.setBold(true);
    p.setFont(f);
    p.setPen(QColor(110, 110, 110));

    // Files (a-h)
    for (int col = 0; col < 8; ++col) {
        int actualCol = m_isFlipped ? (7 - col) : col;
        char fileChar = static_cast<char>('a' + actualCol);
        qreal x = bRect.left() + col * sq;
        QRectF topRect(x, bRect.top() - 22, sq, 20);
        QRectF botRect(x, bRect.bottom() + 3, sq, 20);
        p.drawText(topRect, Qt::AlignCenter, QString(fileChar));
        p.drawText(botRect, Qt::AlignCenter, QString(fileChar));
    }

    // Ranks (1-8)
    for (int row = 0; row < 8; ++row) {
        int actualRank = m_isFlipped ? (row + 1) : (8 - row);
        qreal y = bRect.top() + row * sq;
        QRectF leftRect(bRect.left() - 24, y, 20, sq);
        QRectF rightRect(bRect.right() + 4, y, 20, sq);
        p.drawText(leftRect, Qt::AlignCenter, QString::number(actualRank));
        p.drawText(rightRect, Qt::AlignCenter, QString::number(actualRank));
    }
}

void ChessBoardWidget::drawHighlights(QPainter &p, qreal) {
    // 1. Last move highlights
    BoardPos lastFrom = m_game->lastMoveFrom();
    BoardPos lastTo = m_game->lastMoveTo();
    if (lastFrom.isValid()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(246, 246, 105, 120));
        p.drawRect(squareRect(lastFrom.row, lastFrom.col));
    }
    if (lastTo.isValid()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(246, 246, 105, 150));
        p.drawRect(squareRect(lastTo.row, lastTo.col));
    }

    // 2. Selected square highlight
    if (m_selectedPos.isValid()) {
        p.setPen(QPen(QColor(255, 215, 0), 2.5));
        p.setBrush(QColor(255, 235, 59, 110));
        p.drawRect(squareRect(m_selectedPos.row, m_selectedPos.col));
    }

    // 3. King in check highlight (Crimson)
    if (m_game->status() == GameStatus::Check || m_game->status() == GameStatus::Checkmate) {
        BoardPos kPos = m_game->kingPosition(m_game->currentTurn());
        if (kPos.isValid()) {
            QRectF r = squareRect(kPos.row, kPos.col);
            QRadialGradient rad(r.center(), r.width() / 2.0);
            rad.setColorAt(0.0, QColor(244, 67, 54, 210));
            rad.setColorAt(0.8, QColor(220, 20, 60, 160));
            rad.setColorAt(1.0, QColor(178, 34, 34, 0));
            p.setPen(Qt::NoPen);
            p.setBrush(rad);
            p.drawRect(r);
        }
    }

    // 4. Hover highlight
    if (m_hoverPos.isValid() && m_hoverPos != m_selectedPos) {
        p.setPen(QPen(QColor(255, 255, 255, 180), 2.0));
        p.setBrush(QColor(255, 255, 255, 30));
        p.drawRect(squareRect(m_hoverPos.row, m_hoverPos.col));
    }
}

void ChessBoardWidget::drawLegalMoves(QPainter &p, qreal sq) {
    p.setPen(Qt::NoPen);

    for (const auto &m : m_currentLegalMoves) {
        QRectF sqR = squareRect(m.to.row, m.to.col);
        QPointF center = sqR.center();

        Piece targetPiece = m_game->pieceAt(m.to);
        if (targetPiece.isEmpty() && !m.isEnPassant) {
            // Small smooth dot for empty square
            p.setBrush(QColor(30, 30, 30, 80));
            p.drawEllipse(center, sq * 0.16, sq * 0.16);
        } else {
            // Hollow target ring for captures
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(30, 30, 30, 90), sq * 0.09));
            p.drawEllipse(center, sq * 0.40, sq * 0.40);
            p.setPen(Qt::NoPen);
        }
    }
}

void ChessBoardWidget::drawPieces(QPainter &p, qreal sq) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (m_isDragging && m_selectedPos == BoardPos(r, c)) {
                continue; // Dragged piece is drawn floating on top
            }
            Piece piece = m_game->pieceAt(r, c);
            if (!piece.isEmpty()) {
                QRectF sqR = squareRect(r, c);
                // Inset piece slightly inside square for neat margin
                qreal pad = sq * 0.05;
                QRectF pieceRect = sqR.adjusted(pad, pad, -pad, -pad);
                PieceRenderer::drawPiece(p, piece.type, piece.color, pieceRect);
            }
        }
    }
}

void ChessBoardWidget::drawDraggedPiece(QPainter &p, qreal sq) {
    if (m_draggedPiece.isEmpty()) return;

    qreal size = sq * 1.05;
    QRectF dragRect(m_dragPos.x() - size / 2.0, m_dragPos.y() - size / 2.0, size, size);

    // Drop shadow under dragged piece
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 50));
    p.drawEllipse(dragRect.adjusted(size * 0.15, size * 0.25, -size * 0.15, 0));

    PieceRenderer::drawPiece(p, m_draggedPiece.type, m_draggedPiece.color, dragRect);
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    BoardPos clickedPos = posAtPoint(event->position());
    if (!clickedPos.isValid()) {
        clearSelection();
        return;
    }

    Piece clickedPiece = m_game->pieceAt(clickedPos);

    // If we already have a selected piece and clicked a valid target square -> execute move
    if (m_selectedPos.isValid() && clickedPos != m_selectedPos) {
        for (const auto &m : m_currentLegalMoves) {
            if (m.to == clickedPos) {
                tryExecuteMove(clickedPos);
                return;
            }
        }
    }

    // Select friendly piece
    if (canPlayerMovePiece(clickedPiece)) {
        m_selectedPos = clickedPos;
        m_currentLegalMoves = m_game->legalMovesForSquare(clickedPos);
        m_isDragging = true;
        m_dragPos = event->position();
        m_draggedPiece = clickedPiece;
        update();
    } else {
        clearSelection();
    }
}

void ChessBoardWidget::mouseMoveEvent(QMouseEvent *event) {
    BoardPos hover = posAtPoint(event->position());
    if (hover != m_hoverPos) {
        m_hoverPos = hover;
    }

    if (m_isDragging) {
        m_dragPos = event->position();
        update();
    } else {
        update();
    }
}

void ChessBoardWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_isDragging) {
        BoardPos dropPos = posAtPoint(event->position());
        m_isDragging = false;

        if (dropPos.isValid() && dropPos != m_selectedPos) {
            bool found = false;
            for (const auto &m : m_currentLegalMoves) {
                if (m.to == dropPos) {
                    found = true;
                    break;
                }
            }
            if (found) {
                tryExecuteMove(dropPos);
                return;
            }
        }
        update();
    }
}

void ChessBoardWidget::tryExecuteMove(const BoardPos &toPos) {
    QVector<Move> candidateMoves;
    for (const auto &m : m_currentLegalMoves) {
        if (m.to == toPos) {
            candidateMoves.append(m);
        }
    }

    if (candidateMoves.isEmpty()) {
        clearSelection();
        return;
    }

    Move moveToExecute = candidateMoves[0];

    // Check pawn promotion
    if (candidateMoves.size() > 1 && candidateMoves[0].promotion != PieceType::None) {
        PromotionDialog promoDlg(m_game->currentTurn(), this);
        if (promoDlg.exec() == QDialog::Accepted) {
            PieceType chosen = promoDlg.selectedPiece();
            for (const auto &m : candidateMoves) {
                if (m.promotion == chosen) {
                    moveToExecute = m;
                    break;
                }
            }
        } else {
            // Default to queen if dialog closed
            moveToExecute = candidateMoves[0];
        }
    }

    clearSelection();
    m_game->makeMove(moveToExecute);
    emit userMadeMove(moveToExecute);
}

void ChessBoardWidget::resizeEvent(QResizeEvent *) {
    update();
}
