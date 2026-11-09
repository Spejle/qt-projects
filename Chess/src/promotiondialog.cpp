#include "promotiondialog.h"
#include "piecerenderer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>

PromotionDialog::PromotionDialog(PieceColor color, QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::CustomizeWindowHint)
{
    setWindowTitle(tr("Pawn Promotion"));
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QLabel *label = new QLabel(tr("Choose piece to promote pawn to:"), this);
    label->setAlignment(Qt::AlignCenter);
    QFont f = label->font();
    f.setBold(true);
    f.setPointSize(11);
    label->setFont(f);
    mainLayout->addWidget(label);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    PieceType pieces[] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
    const char* names[] = {"Queen", "Rook", "Bishop", "Knight"};

    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton(this);
        btn->setFixedSize(70, 70);
        btn->setIcon(QIcon(PieceRenderer::piecePixmap(pieces[i], color, 64)));
        btn->setIconSize(QSize(56, 56));
        btn->setToolTip(tr(names[i]));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background: #F8F9FA; border: 2px solid #D0D7DE; border-radius: 8px; }"
            "QPushButton:hover { background: #E7F3FF; border-color: #0969DA; }"
            "QPushButton:pressed { background: #CFE4FA; }"
        );

        PieceType pt = pieces[i];
        connect(btn, &QPushButton::clicked, this, [this, pt]() {
            onPieceClicked(pt);
        });

        btnLayout->addWidget(btn);
    }

    mainLayout->addLayout(btnLayout);
    adjustSize();
}

void PromotionDialog::onPieceClicked(PieceType type) {
    m_selected = type;
    accept();
}
