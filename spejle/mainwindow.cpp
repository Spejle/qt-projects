#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonCalc, &QPushButton::clicked, this, &MainWindow::calc);
    connect(ui->spinBoxSecondInput, &QSpinBox::valueChanged, this, &MainWindow::calc);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::calc()
{
    int result = ui->spinBoxFirstInput->text().toInt() + ui->spinBoxSecondInput->text().toInt();;
    ui->labelResult->setText(QString::number(result));
}
