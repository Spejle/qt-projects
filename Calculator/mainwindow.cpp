#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonZero, &QPushButton::clicked, this, &MainWindow::on0);
    connect(ui->pushButtonOne, &QPushButton::clicked, this, &MainWindow::on1);
    connect(ui->pushButtonTwo, &QPushButton::clicked, this, &MainWindow::on2);
    connect(ui->pushButtonThree, &QPushButton::clicked, this, &MainWindow::on3);
    connect(ui->pushButtonFour, &QPushButton::clicked, this, &MainWindow::on4);
    connect(ui->pushButtonFive, &QPushButton::clicked, this, &MainWindow::on5);
    connect(ui->pushButtonSix, &QPushButton::clicked, this, &MainWindow::on6);
    connect(ui->pushButtonSeven, &QPushButton::clicked, this, &MainWindow::on7);
    connect(ui->pushButtonEight, &QPushButton::clicked, this, &MainWindow::on8);
    connect(ui->pushButtonNine, &QPushButton::clicked, this, &MainWindow::on9);

    connect(ui->pushButtonPlus, &QPushButton::clicked, this, &MainWindow::onPlus);
    connect(ui->pushButtonMinus, &QPushButton::clicked, this, &MainWindow::onMinus);
    connect(ui->pushButtonDivide, &QPushButton::clicked, this, &MainWindow::onDivide);
    connect(ui->pushButtonMultiply, &QPushButton::clicked, this, &MainWindow::onMultiply);
    connect(ui->pushButtonPower, &QPushButton::clicked, this, &MainWindow::onPower);
    connect(ui->pushButtonEqual, &QPushButton::clicked, this, &MainWindow::onEqual);
    connect(ui->pushButtonComma, &QPushButton::clicked, this, &MainWindow::onComma);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(ui->pushButtonClear, &QPushButton::clicked, this, &MainWindow::onClear);

}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::calculate(double number1, char op, double number2){
    switch (op)
    {
    case '+':
        return number1 + number2;
    case '-':
        return number1 - number2;
    case '*':
        return number1 * number2;
    case '/':
        return number1 / number2;
    case '^':
        return std::pow(number1, number2);
    default:
        return -1;
    }

}
void MainWindow::insertToDisplay(QString string)
{
    ui->lineEdit->setText(ui->lineEdit->text() + string);
}

void MainWindow::on0()
{
    insertToDisplay("0");
}

void MainWindow::on1()
{
    insertToDisplay("1");
}
void MainWindow::on2()
{
    insertToDisplay("2");
}
void MainWindow::on3()
{
    insertToDisplay("3");
}
void MainWindow::on4()
{
    insertToDisplay("4");
}
void MainWindow::on5()
{
    insertToDisplay("5");
}
void MainWindow::on6()
{
    insertToDisplay("6");
}
void MainWindow::on7()
{
    insertToDisplay("7");
}
void MainWindow::on8()
{
    insertToDisplay("8");
}
void MainWindow::on9()
{
    insertToDisplay("9");
}

void MainWindow::onPlus()
{

    insertToDisplay(" + ");
}

void MainWindow::onMinus()
{
    insertToDisplay(" - ");
}

void MainWindow::onDivide()
{
    insertToDisplay(" / ");
}

void MainWindow::onMultiply()
{
    insertToDisplay(" * ");
}

void MainWindow::onPower()
{
    insertToDisplay(" ^ ");
}

static double parseNumber(const QString &token)
{
    return QString(token).replace(',', '.').toDouble();
}

void MainWindow::onEqual()
{
    QStringList tokens = ui->lineEdit->text().split(' ', Qt::SkipEmptyParts);
    if (tokens.isEmpty())
        return;

    // First pass: resolve ^ left to right.
    QStringList pass0;
    pass0.append(tokens.at(0));
    for (int i = 1; i + 1 < tokens.size(); i += 2) {
        QString op = tokens.at(i);
        double next = parseNumber(tokens.at(i + 1));
        if (op == "^") {
            double prev = parseNumber(pass0.last());
            pass0.removeLast();
            pass0.append(QString::number(calculate(prev, '^', next)));
        } else {
            pass0.append(op);
            pass0.append(QString::number(next));
        }
    }

    // Second pass: resolve * and / left to right.
    QStringList pass1;
    pass1.append(pass0.at(0));
    for (int i = 1; i + 1 < pass0.size(); i += 2) {
        QString op = pass0.at(i);
        double next = parseNumber(pass0.at(i + 1));
        if (op == "*" || op == "/") {
            double prev = parseNumber(pass1.last());
            pass1.removeLast();
            pass1.append(QString::number(calculate(prev, op.at(0).toLatin1(), next)));
        } else {
            pass1.append(op);
            pass1.append(QString::number(next));
        }
    }

    // Third pass: resolve + and - left to right.
    double result = parseNumber(pass1.at(0));
    for (int i = 1; i + 1 < pass1.size(); i += 2) {
        char op = pass1.at(i).at(0).toLatin1();
        double next = parseNumber(pass1.at(i + 1));
        result = calculate(result, op, next);
    }

    ui->lineEdit->setText(QString::number(result).replace('.', ','));
}

void MainWindow::onComma()
{
    QStringList tokens = ui->lineEdit->text().split(' ', Qt::SkipEmptyParts);
    if (!tokens.isEmpty() && tokens.last().contains(','))
        return;

    insertToDisplay(",");
}

void MainWindow::onDelete()
{
    ui->lineEdit->setText(ui->lineEdit->text().chopped(1));
}

void MainWindow::onClear()
{
    ui->lineEdit->clear();
}
