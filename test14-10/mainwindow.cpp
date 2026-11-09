#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

    {
        ui->setupUi(this);
        connect(ui->radioButton, &QRadioButton::clicked, this, &MainWindow::labelAdd);
        connect(ui->radioButton_2, &QRadioButton::clicked, this, &MainWindow::labelAdd);


        connect(ui->checkBox, &QCheckBox::toggled, this, &MainWindow::labelAdd);
        connect(ui->checkBox_2, &QCheckBox::toggled, this, &MainWindow::labelAdd);

        connect(ui->spinBox, &QSpinBox::valueChanged, this, &MainWindow::labelAdd);
        connect(ui->spinBox_2, &QSpinBox::valueChanged, this, &MainWindow::labelAdd);

        connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::labelAdd);

    }

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::labelAdd()
{
    QString text = "";

    if (ui->radioButton->isChecked())
        text += ui->radioButton->objectName() + " ";
    if (ui->radioButton_2->isChecked())
        text += ui->radioButton_2->objectName() + " ";

    if (ui->checkBox->isChecked())
        text += ui->checkBox->objectName() + " ";
    if (ui->checkBox_2->isChecked())
        text += ui->checkBox_2->objectName() + " ";

    int sum = ui->spinBox->value() + ui->spinBox_2->value();

    text += QString::number(sum) + " ";

    text += ui->lineEdit->text();

    ui->label->setText(text);

}

