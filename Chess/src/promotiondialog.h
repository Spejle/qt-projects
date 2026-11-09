#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "types.h"

class PromotionDialog : public QDialog {
    Q_OBJECT

public:
    explicit PromotionDialog(PieceColor color, QWidget *parent = nullptr);

    PieceType selectedPiece() const { return m_selected; }

private slots:
    void onPieceClicked(PieceType type);

private:
    PieceType m_selected = PieceType::Queen;
};

#endif // PROMOTIONDIALOG_H
