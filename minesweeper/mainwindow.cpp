#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QPoint>

const int SIZE_ARRAY = 10;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)


{
        const int SIZE_ARRAY = 10;
    ui->setupUi(this);

    mModelStandardItem = new QStandardItemModel(this);
    for (int i = 0; i < SIZE_ARRAY; ++i) {
        QList<QStandardItem*> row;
        for (int j = 0; j < SIZE_ARRAY; ++j) {
            QStandardItem *item = new QStandardItem("");
            item->setTextAlignment(Qt::AlignCenter);
            row.append(item);
        }
        mModelStandardItem->appendRow(row);
    }

      ui->tableView->setModel(mModelStandardItem);
    createMines();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMines()
{
    int x,y;
    for(int i = 0; i < SIZE_ARRAY; ++i){
        x = QRandomGenerator::global()->bounded(SIZE_ARRAY);
        y = QRandomGenerator::global()->bounded(SIZE_ARRAY);

        QStandardItem *item = mModelStandardItem->item(x,y);
        item->setData("M", Qt::UserRole);
        item->setForeground(QBrush(QColor("red")));x;

    }

}

void MainWindow::createNumbers()
{
    for(int i = 0; i < SIZE_ARRAY; ++i){
        for(int j = 0; j < SIZE_ARRAY; ++j){
            if(mModelStandardItem->item(i,j)->data(Qt::UserRole).toString() != "M");

        }
    }
}

int MainWindow::countMineNumbers(int x, int y)
{
    QList<QPoint> listPoints;
    int xN, yN;

    // 1. Top-left
    xN = x - 1; yN = y - 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 2. Top
    xN = x - 1; yN = y;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 3. Top-right
    xN = x - 1; yN = y + 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 4. Left
    xN = x; yN = y - 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 5. Right
    xN = x; yN = y + 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 6. Bottom-left
    xN = x + 1; yN = y - 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 7. Bottom
    xN = x + 1; yN = y;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // 8. Bottom-right
    xN = x + 1; yN = y + 1;
    if (checkValidity(xN, yN)) listPoints.append(QPoint(xN, yN));

    // Now count how many of the valid neighbors are mines
    int count = 0;
    for (const QPoint &p : listPoints) {
        if (mModelStandardItem->item(p.x(), p.y())->data(Qt::UserRole).toString() == "M") {
            count++;
        }
    }

    return count;
}




bool MainWindow::checkValidity(int x, int y)
{

    if( x < 0 or x>= SIZE_ARRAY or y <0 or y >= SIZE_ARRAY)
        return false;
    else
        return true;
}

