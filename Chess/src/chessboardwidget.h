#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#include <QWidget>
#include "types.h"
#include "chessgame.h"

class ChessBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoardWidget(ChessGame *game, QWidget *parent = nullptr);

    void setTheme(BoardTheme theme);
    BoardTheme theme() const { return m_theme; }

    void setFlipped(bool flipped);
    bool isFlipped() const { return m_isFlipped; }
    void toggleFlip() { setFlipped(!m_isFlipped); }

    void setInteractive(bool allow) { m_interactionAllowed = allow; }
    bool isInteractive() const { return m_interactionAllowed; }

    void setHumanColor(PieceColor c) { m_humanColor = c; }
    PieceColor humanColor() const { return m_humanColor; }

    void setGameMode(GameMode mode) { m_gameMode = mode; }
    GameMode gameMode() const { return m_gameMode; }

    void clearSelection();

signals:
    void userMadeMove(const Move &move);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QRectF boardRect() const;
    qreal squareSize() const;
    QRectF squareRect(int r, int c) const;
    BoardPos posAtPoint(const QPointF &pt) const;

    void drawBoard(QPainter &p, const QRectF &bRect);
    void drawCoordinates(QPainter &p, const QRectF &bRect, qreal sqSize);
    void drawHighlights(QPainter &p, qreal sqSize);
    void drawLegalMoves(QPainter &p, qreal sqSize);
    void drawPieces(QPainter &p, qreal sqSize);
    void drawDraggedPiece(QPainter &p, qreal sqSize);

    bool canPlayerMovePiece(const Piece &p) const;
    void tryExecuteMove(const BoardPos &toPos);

    ChessGame *m_game;
    BoardTheme m_theme = BoardTheme::TournamentGreen;
    bool m_isFlipped = false;
    bool m_interactionAllowed = true;
    GameMode m_gameMode = GameMode::PlayerVsPlayer;
    PieceColor m_humanColor = PieceColor::White;

    BoardPos m_selectedPos = {-1, -1};
    BoardPos m_hoverPos = {-1, -1};
    QVector<Move> m_currentLegalMoves;

    bool m_isDragging = false;
    QPointF m_dragPos;
    Piece m_draggedPiece;
};

#endif // CHESSBOARDWIDGET_H
