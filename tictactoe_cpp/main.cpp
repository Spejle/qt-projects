#include "tablemodel.h"

#include <QApplication>
#include <QHeaderView>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Tabulka 5x6 - křížky");

    TicTacModel model;
    QTableView view;
    view.setModel(&model);
    view.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view.verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view.setStyleSheet("QTableView { font-size: 28px; }");
    QObject::connect(&view, &QTableView::clicked, &model, &TicTacModel::toggle);

    QLabel label;
    label.setStyleSheet("font-size: 16px;");

    auto update = [&] {
        QStringList parts;
        for (int r = 0; r < TicTacModel::Rows; ++r)
            parts << QString("Řádek %1: %2").arg(r + 1).arg(model.rowCountX(r));
        label.setText(parts.join(" | ") + QString("   —   Celkem: %1").arg(model.totalX()));
    };
    QObject::connect(&model, &TicTacModel::changed, update);
    update();

    auto *layout = new QVBoxLayout(&window);
    layout->addWidget(&view);
    layout->addWidget(&label);

    window.resize(640, 480);
    window.show();
    return app.exec();
}
