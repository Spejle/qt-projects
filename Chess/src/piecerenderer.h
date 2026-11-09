#ifndef PIECERENDERER_H
#define PIECERENDERER_H

#include <QPainter>
#include <QRectF>
#include <QPixmap>
#include "types.h"

class PieceRenderer {
public:
    static void drawPiece(QPainter &painter, PieceType type, PieceColor color, const QRectF &rect);
    static QPixmap piecePixmap(PieceType type, PieceColor color, int size);

private:
    static void drawPawn(QPainter &p, PieceColor color, const QRectF &rect);
    static void drawRook(QPainter &p, PieceColor color, const QRectF &rect);
    static void drawKnight(QPainter &p, PieceColor color, const QRectF &rect);
    static void drawBishop(QPainter &p, PieceColor color, const QRectF &rect);
    static void drawQueen(QPainter &p, PieceColor color, const QRectF &rect);
    static void drawKing(QPainter &p, PieceColor color, const QRectF &rect);

    static QBrush fillBrush(PieceColor color, const QRectF &r);
    static QPen outlinePen(PieceColor color);
    static QPen detailPen(PieceColor color);
};

#endif // PIECERENDERER_H
