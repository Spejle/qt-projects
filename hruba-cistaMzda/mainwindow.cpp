#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->doubleSpinBox_Hruby, &QDoubleSpinBox::valueChanged, this, &MainWindow::mzdaVypocet);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mzdaVypocet()
{
    double resultSoc = ui->doubleSpinBox_Hruby->value() * 0.071;
    ui->labelSocialniCastka->setText(QString::number(resultSoc)+ " CZK");

    double resultZdr = ui->doubleSpinBox_Hruby->value() * 0.045;
    ui->labelZdravodtniCastka->setText(QString::number(resultZdr)+ " CZK");

    double result = ui->doubleSpinBox_Hruby->value() * 0.15;
    ui->labelPrimuCastka->setText(QString::number(result)+ " CZK");

    ui->label_CistaMzda->setText(QString::number(ui->doubleSpinBox_Hruby->value()-resultSoc-resultZdr-result));
}
