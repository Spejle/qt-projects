#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::Prumer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Prumer()
{
    QString input = ui->lineEditZnamky->text();
    QStringList list = input.split(",");
    double sum = 0;
    for(int i = 0; i < list.size();i++){
        sum+= list[i].toInt();
    }
        double result = sum /list.size();
    ui->labelPrumer->setText(QString::number(result));
}
