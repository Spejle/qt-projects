#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->radioButton, &QRadioButton::clicked, this, &MainWindow::labelAdd);
    connect(ui->radioButton_2, &QRadioButton::clicked, this, &MainWindow::labelAdd);


    connect(ui->checkBox, &QCheckBox::toggled, this, &MainWindow::lineEditAdd);
    connect(ui->checkBox_2, &QCheckBox::toggled, this, &MainWindow::lineEditAdd);

    connect(ui->spinBox, &QSpinBox::valueChanged, this, &MainWindow::labelAdd);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::lineEditAdd()
{
    if(ui->checkBox->isChecked()){
        ui->lineEdit->setText("CheckBox");
    }
    if(ui->checkBox_2->isChecked()){
        ui->lineEdit->setText("CheckBox2");
    }
    if(ui->checkBox->isChecked() && ui->checkBox_2->isChecked())
    {
        ui->lineEdit->setText("CheckBox ChechBox2");
    }
}

void MainWindow::labelAdd()
{
    if(ui->radioButton->isChecked()){
        ui->label->setText("RadioButton "+ QString::number(ui->spinBox->value()));
    }
    else{
        ui->label->setText("RadioButton2 " + QString::number(ui->spinBox->value()));
    }
}
