#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->doubleSpinBoxHeight, &QDoubleSpinBox::valueChanged, this, &MainWindow::bmiCalc);
    connect(ui->doubleSpinBoxMass, &QDoubleSpinBox::valueChanged, this, &MainWindow::bmiCalc);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bmiCalc()
{
    double bmi = ui->doubleSpinBoxMass->value() / (ui->doubleSpinBoxHeight->value()*ui->doubleSpinBoxHeight->value());
    ui->labelBMI->setText("BMI: " + QString::number(bmi));
}

