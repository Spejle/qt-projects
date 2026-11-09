#include "piecerenderer.h"
#include <QPainterPath>
#include <QLinearGradient>

QBrush PieceRenderer::fillBrush(PieceColor color, const QRectF &r) {
    QLinearGradient grad(r.topLeft(), r.bottomRight());
    if (color == PieceColor::White) {
        grad.setColorAt(0.0, QColor(255, 255, 255));
        grad.setColorAt(0.6, QColor(245, 240, 230));
        grad.setColorAt(1.0, QColor(220, 212, 198));
    } else {
        grad.setColorAt(0.0, QColor(68, 64, 60));
        grad.setColorAt(0.5, QColor(42, 39, 37));
        grad.setColorAt(1.0, QColor(20, 18, 17));
    }
    return QBrush(grad);
}

QPen PieceRenderer::outlinePen(PieceColor color) {
    if (color == PieceColor::White) {
        return QPen(QColor(40, 36, 32), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    } else {
        return QPen(QColor(15, 12, 10), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    }
}

QPen PieceRenderer::detailPen(PieceColor color) {
    if (color == PieceColor::White) {
        return QPen(QColor(140, 130, 115), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    } else {
        return QPen(QColor(120, 115, 110), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    }
}

void PieceRenderer::drawPiece(QPainter &p, PieceType type, PieceColor color, const QRectF &rect) {
    if (type == PieceType::None || color == PieceColor::None) return;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(rect.topLeft());
    p.scale(rect.width() / 100.0, rect.height() / 100.0);

    QRectF localRect(0, 0, 100, 100);

    switch (type) {
        case PieceType::Pawn:
            drawPawn(p, color, localRect);
            break;
        case PieceType::Rook:
            drawRook(p, color, localRect);
            break;
        case PieceType::Knight:
            drawKnight(p, color, localRect);
            break;
        case PieceType::Bishop:
            drawBishop(p, color, localRect);
            break;
        case PieceType::Queen:
            drawQueen(p, color, localRect);
            break;
        case PieceType::King:
            drawKing(p, color, localRect);
            break;
        default:
            break;
    }

    p.restore();
}

QPixmap PieceRenderer::piecePixmap(PieceType type, PieceColor color, int size) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    drawPiece(p, type, color, QRectF(0, 0, size, size));
    p.end();
    return pix;
}

void PieceRenderer::drawPawn(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base pedestal
    QPainterPath basePath;
    basePath.moveTo(22, 86);
    basePath.quadTo(20, 80, 26, 78);
    basePath.lineTo(74, 78);
    basePath.quadTo(80, 80, 78, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Body
    QPainterPath bodyPath;
    bodyPath.moveTo(32, 78);
    bodyPath.cubicTo(36, 64, 43, 56, 42, 48);
    bodyPath.lineTo(58, 48);
    bodyPath.cubicTo(57, 56, 64, 64, 68, 78);
    bodyPath.closeSubpath();
    p.drawPath(bodyPath);

    // Collar
    p.drawRoundedRect(QRectF(36, 44, 28, 7), 3, 3);

    // Head
    p.drawEllipse(QPointF(50, 31), 14, 14);

    // Highlight detail
    p.setPen(detail);
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRectF(42, 23, 16, 16), 50 * 16, 120 * 16);
    p.drawLine(QPointF(30, 82), QPointF(70, 82));
}

void PieceRenderer::drawRook(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base
    QPainterPath basePath;
    basePath.moveTo(20, 86);
    basePath.quadTo(18, 80, 24, 78);
    basePath.lineTo(76, 78);
    basePath.quadTo(82, 80, 80, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Tower Body
    QPainterPath bodyPath;
    bodyPath.moveTo(28, 78);
    bodyPath.lineTo(33, 42);
    bodyPath.lineTo(67, 42);
    bodyPath.lineTo(72, 78);
    bodyPath.closeSubpath();
    p.drawPath(bodyPath);

    // Cornice
    p.drawRoundedRect(QRectF(22, 38, 56, 6), 2, 2);

    // Battlements
    QPainterPath topPath;
    topPath.moveTo(22, 38);
    topPath.lineTo(22, 22);
    topPath.lineTo(33, 22);
    topPath.lineTo(33, 29);
    topPath.lineTo(44, 29);
    topPath.lineTo(44, 22);
    topPath.lineTo(56, 22);
    topPath.lineTo(56, 29);
    topPath.lineTo(67, 29);
    topPath.lineTo(67, 22);
    topPath.lineTo(78, 22);
    topPath.lineTo(78, 38);
    topPath.closeSubpath();
    p.drawPath(topPath);

    // Details: central arrow slit
    p.setPen(detail);
    p.setBrush(Qt::NoBrush);
    p.drawLine(QPointF(50, 50), QPointF(50, 68));
    p.drawLine(QPointF(46, 56), QPointF(54, 56));
    p.drawLine(QPointF(27, 82), QPointF(73, 82));
}

void PieceRenderer::drawKnight(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base
    QPainterPath basePath;
    basePath.moveTo(20, 86);
    basePath.quadTo(18, 80, 24, 78);
    basePath.lineTo(76, 78);
    basePath.quadTo(82, 80, 80, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Horse Head silhouette
    QPainterPath horse;
    horse.moveTo(68, 78);
    horse.cubicTo(69, 64, 69, 44, 64, 30);
    // Ears
    horse.lineTo(62, 17);
    horse.lineTo(53, 23);
    horse.lineTo(48, 19);
    // Forehead and snout
    horse.cubicTo(42, 26, 32, 32, 24, 38);
    horse.quadTo(21, 44, 24, 49);
    // Mouth and lower jaw
    horse.lineTo(34, 48);
    horse.quadTo(28, 54, 34, 56);
    // Throat curve down to chest
    horse.cubicTo(45, 58, 33, 68, 29, 78);
    horse.closeSubpath();
    p.drawPath(horse);

    // Details: Eye, nostril, mane
    p.setPen(detail);
    p.setBrush(color == PieceColor::White ? QColor(40, 36, 32) : QColor(220, 215, 205));
    p.drawEllipse(QPointF(39, 36), 2.5, 2.5);

    p.setBrush(Qt::NoBrush);
    // Nostril
    p.drawArc(QRectF(25, 42, 5, 5), 0, 180 * 16);
    // Mane notches
    p.drawLine(QPointF(64, 33), QPointF(57, 36));
    p.drawLine(QPointF(67, 44), QPointF(59, 48));
    p.drawLine(QPointF(68, 56), QPointF(60, 60));
    p.drawLine(QPointF(27, 82), QPointF(73, 82));
}

void PieceRenderer::drawBishop(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base
    QPainterPath basePath;
    basePath.moveTo(21, 86);
    basePath.quadTo(19, 80, 25, 78);
    basePath.lineTo(75, 78);
    basePath.quadTo(81, 80, 79, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Collar
    p.drawRoundedRect(QRectF(33, 56, 34, 6), 2, 2);

    // Body pedestal
    QPainterPath bodyPath;
    bodyPath.moveTo(30, 78);
    bodyPath.cubicTo(34, 68, 36, 64, 37, 62);
    bodyPath.lineTo(63, 62);
    bodyPath.cubicTo(64, 64, 66, 68, 70, 78);
    bodyPath.closeSubpath();
    p.drawPath(bodyPath);

    // Mitre head
    QPainterPath mitre;
    mitre.moveTo(35, 56);
    mitre.cubicTo(27, 46, 30, 29, 50, 21);
    mitre.cubicTo(70, 29, 73, 46, 65, 56);
    mitre.closeSubpath();
    p.drawPath(mitre);

    // Top finial
    p.drawEllipse(QPointF(50, 17), 4.5, 4.5);

    // Mitre slash cut
    p.setPen(pen);
    p.drawLine(QPointF(42, 33), QPointF(58, 43));

    // Internal cross detail
    p.setPen(detail);
    p.drawLine(QPointF(50, 48), QPointF(50, 53));
    p.drawLine(QPointF(47, 50), QPointF(53, 50));
    p.drawLine(QPointF(28, 82), QPointF(72, 82));
}

void PieceRenderer::drawQueen(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base
    QPainterPath basePath;
    basePath.moveTo(19, 86);
    basePath.quadTo(17, 80, 23, 78);
    basePath.lineTo(77, 78);
    basePath.quadTo(83, 80, 81, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Body
    QPainterPath bodyPath;
    bodyPath.moveTo(27, 78);
    bodyPath.cubicTo(32, 66, 36, 56, 36, 52);
    bodyPath.lineTo(64, 52);
    bodyPath.cubicTo(64, 56, 68, 66, 73, 78);
    bodyPath.closeSubpath();
    p.drawPath(bodyPath);

    // Circlet band
    p.drawRoundedRect(QRectF(31, 48, 38, 6), 2, 2);

    // Crown
    QPainterPath crown;
    crown.moveTo(33, 48);
    crown.lineTo(21, 28); // 1st tip
    crown.lineTo(32, 38);
    crown.lineTo(36, 23); // 2nd tip
    crown.lineTo(44, 36);
    crown.lineTo(50, 19); // 3rd (center) tip
    crown.lineTo(56, 36);
    crown.lineTo(64, 23); // 4th tip
    crown.lineTo(68, 38);
    crown.lineTo(79, 28); // 5th tip
    crown.lineTo(67, 48);
    crown.closeSubpath();
    p.drawPath(crown);

    // 5 Pearls on tips
    p.drawEllipse(QPointF(21, 27), 3.2, 3.2);
    p.drawEllipse(QPointF(36, 22), 3.2, 3.2);
    p.drawEllipse(QPointF(50, 18), 3.6, 3.6);
    p.drawEllipse(QPointF(64, 22), 3.2, 3.2);
    p.drawEllipse(QPointF(79, 27), 3.2, 3.2);

    // Detail lines
    p.setPen(detail);
    p.drawLine(QPointF(26, 82), QPointF(74, 82));
    p.drawLine(QPointF(36, 51), QPointF(64, 51));
}

void PieceRenderer::drawKing(QPainter &p, PieceColor color, const QRectF &r) {
    QBrush brush = fillBrush(color, r);
    QPen pen = outlinePen(color);
    QPen detail = detailPen(color);

    p.setBrush(brush);
    p.setPen(pen);

    // Base
    QPainterPath basePath;
    basePath.moveTo(19, 86);
    basePath.quadTo(17, 80, 23, 78);
    basePath.lineTo(77, 78);
    basePath.quadTo(83, 80, 81, 86);
    basePath.closeSubpath();
    p.drawPath(basePath);

    // Body
    QPainterPath bodyPath;
    bodyPath.moveTo(27, 78);
    bodyPath.cubicTo(32, 66, 35, 56, 34, 52);
    bodyPath.lineTo(66, 52);
    bodyPath.cubicTo(65, 56, 68, 66, 73, 78);
    bodyPath.closeSubpath();
    p.drawPath(bodyPath);

    // Circlet band
    p.drawRoundedRect(QRectF(30, 48, 40, 6), 2, 2);

    // Crown Dome / Arches
    QPainterPath crown;
    crown.moveTo(32, 48);
    crown.cubicTo(24, 38, 30, 27, 50, 26);
    crown.cubicTo(70, 27, 76, 38, 68, 48);
    crown.closeSubpath();
    p.drawPath(crown);

    // Royal Cross on King's Crown
    QPainterPath cross;
    cross.moveTo(47, 26);
    cross.lineTo(47, 19);
    cross.lineTo(42, 19);
    cross.lineTo(42, 15);
    cross.lineTo(47, 15);
    cross.lineTo(47, 10);
    cross.lineTo(53, 10);
    cross.lineTo(53, 15);
    cross.lineTo(58, 15);
    cross.lineTo(58, 19);
    cross.lineTo(53, 19);
    cross.lineTo(53, 26);
    cross.closeSubpath();
    p.drawPath(cross);

    // Detail lines
    p.setPen(detail);
    p.drawLine(QPointF(26, 82), QPointF(74, 82));
    p.drawLine(QPointF(50, 28), QPointF(50, 46));
    p.drawArc(QRectF(36, 32, 28, 18), 30 * 16, 120 * 16);
}
