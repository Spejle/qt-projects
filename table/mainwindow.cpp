#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItem>
#include <QHeaderView>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mModelStandardItem = new QStandardItemModel(this);

    int number = 1;
    for (int r = 0; r < 3; ++r) {
        QList<QStandardItem*> row;
        for (int c = 0; c < 4; ++c) {
            QStandardItem *item = new QStandardItem;
            item->setData(number++, Qt::UserRole);
            updateItemText(item);
            row.append(item);
        }

        mModelStandardItem->appendRow(row);
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setModel(mModelStandardItem);

    ui->comboBoxCurrency->addItems({"CZK", "EUR", "USD", "YEN", "zł", "HRV"});

    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableViewClicked);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::onTableViewDoubleClicked);
    connect(ui->pushButtonAddSuffix, &QPushButton::clicked, this, &MainWindow::onPushButtonAddSuffixClicked);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onHeaderClicked);
}

void MainWindow::updateItemText(QStandardItem *item)
{
    QString text = QString::number(item->data(Qt::UserRole).toInt());
    if (!mSuffix.isEmpty())
        text += " " + mSuffix;
    item->setText(text);
}

void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    item->setData(item->data(Qt::UserRole).toInt() + 1, Qt::UserRole);
    updateItemText(item);
    ui->labelInfo->setText("Value: " + item->text());
}

void MainWindow::onPushButtonAddSuffixClicked()
{
    mSuffix = ui->comboBoxCurrency->currentText();
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r)
        for (int c = 0; c < mModelStandardItem->columnCount(); ++c)
            updateItemText(mModelStandardItem->item(r, c));
}

void MainWindow::onTableViewDoubleClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    item->setData((item->data(Qt::UserRole).toInt() - 1 ) * 2, Qt::UserRole);
    updateItemText(item);
    ui->labelInfo->setText("Value: " + item->text());
}

void MainWindow::onHeaderClicked(int logicalIndex)
{
    QStandardItem *item = mModelStandardItem->horizontalHeaderItem(logicalIndex);
    if (!item) {
        item = new QStandardItem;
        item->setData(0, Qt::UserRole);
        mModelStandardItem->setHorizontalHeaderItem(logicalIndex, item);
    }
    item->setData(item->data(Qt::UserRole).toInt() + 1, Qt::UserRole);
    updateItemText(item);
    ui->labelInfo->setText("Header Value: " + item->text());
}

MainWindow::~MainWindow()
{
    delete ui;
}
