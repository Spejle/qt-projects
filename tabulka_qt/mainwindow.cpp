#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>
#include <QStandardItem>

namespace {
constexpr int kRowsCountX = 9;
constexpr int kColumnsCountX = 9;
constexpr int kRowsSum = 10;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mModelCountX = new QStandardItemModel(kRowsCountX, kColumnsCountX, this);
    mModelSum = new QStandardItemModel(kRowsSum, 1, this);
    mModelSum->setHorizontalHeaderLabels({"Číslo"});

    ui->tableViewCountX->setModel(mModelCountX);
    ui->tableViewCountX->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewSum->setModel(mModelSum);
    ui->tableViewSum->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // itemChanged se vyšle při každé úpravě buňky (po potvrzení editace)
    connect(mModelCountX, &QStandardItemModel::itemChanged, this, &MainWindow::onModelCountXChanged);
    connect(mModelSum, &QStandardItemModel::itemChanged, this, &MainWindow::onModelSumChanged);

    onModelCountXChanged();
    onModelSumChanged();
}

void MainWindow::onModelCountXChanged()
{
    int count = 0;
    for (int r = 0; r < mModelCountX->rowCount(); ++r) {
        for (int c = 0; c < mModelCountX->columnCount(); ++c) {
            QStandardItem *item = mModelCountX->item(r, c);   // může být nullptr
            if (item && item->text().trimmed().compare("x", Qt::CaseInsensitive) == 0)
                ++count;
        }
    }
    ui->labelCountX->setText(QString("V tabulce jsou %1 X").arg(count));
}

void MainWindow::onModelSumChanged()
{
    double sum = 0;
    for (int r = 0; r < mModelSum->rowCount(); ++r) {
        QStandardItem *item = mModelSum->item(r, 0);
        if (!item)
            continue;
        bool ok = false;
        double value = item->text().replace(',', '.').toDouble(&ok);
        if (ok)
            sum += value;
    }
    ui->labelSum->setText(QString("Součet: %1").arg(sum));
}

MainWindow::~MainWindow()
{
    delete ui;
}
