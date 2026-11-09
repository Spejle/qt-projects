#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>
#include <QStandardItem>

namespace {
constexpr int kRows = 5;
constexpr int kColumns = 6;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mModelStandardItem = new QStandardItemModel(this);

    for (int r = 0; r < kRows; ++r) {
        QList<QStandardItem*> row;
        for (int c = 0; c < kColumns; ++c) {
            QStandardItem *item = new QStandardItem;
            item->setTextAlignment(Qt::AlignCenter);
            row.append(item);
        }
        mModelStandardItem->appendRow(row);
    }

    ui->tableView->setModel(mModelStandardItem);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setStyleSheet("QTableView { font-size: 28px; }");

    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableViewClicked);

    updateLabel();
}

// klik: prázdné -> X -> O -> prázdné
void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    if (!item)
        return;

    if (item->text().isEmpty())
        item->setText("X");
    else if (item->text() == "X")
        item->setText("O");
    else
        item->setText("");

    updateLabel();
}

// kolečka (O) a prázdná pole se nepočítají, jen křížky
int MainWindow::countX(int row) const
{
    int count = 0;
    for (int c = 0; c < mModelStandardItem->columnCount(); ++c) {
        QStandardItem *item = mModelStandardItem->item(row, c);
        if (item && item->text() == "X")
            ++count;
    }
    return count;
}

void MainWindow::updateLabel()
{
    QStringList parts;
    int total = 0;
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r) {
        int count = countX(r);
        total += count;
        parts << QString("Řádek %1: %2").arg(r + 1).arg(count);
    }
    ui->labelInfo->setText(parts.join(" | ") + QString("   —   Celkem: %1").arg(total));
}

MainWindow::~MainWindow()
{
    delete ui;
}
