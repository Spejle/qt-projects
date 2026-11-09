#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>

namespace {
constexpr int kRows = 9;
constexpr int kColumns = 9;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mModelStandardItem = new QStandardItemModel(kRows, kColumns, this);

    ui->tableView->setModel(mModelStandardItem);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}
