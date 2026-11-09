#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRandomGenerator>
#include <QClipboard>
#include <QMessageBox>
#include <QVector>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Generátor hesel");

    ui->spinBox->setRange(1, 100);
    ui->spinBox->setValue(5);

    connect(ui->radioButtonStrong, &QRadioButton::toggled, this, &MainWindow::onStrongPassword);
    connect(ui->radioButtonBasic, &QRadioButton::toggled, this, &MainWindow::onBasicPassword);
    connect(ui->pushButtonGenerate, &QPushButton::clicked, this, &MainWindow::generatePassword);
    connect(ui->pushButtonCopy, &QPushButton::clicked, this, &MainWindow::copyToClipboard);

    connect(ui->spinBox, &QSpinBox::valueChanged, this, &MainWindow::generatePassword);
    connect(ui->checkBoxLowerCase, &QCheckBox::toggled, this, &MainWindow::generatePassword);
    connect(ui->checkBoxUppercase, &QCheckBox::toggled, this, &MainWindow::generatePassword);
    connect(ui->checkBoxNumbers, &QCheckBox::toggled, this, &MainWindow::generatePassword);
    connect(ui->checkBoxSymbols, &QCheckBox::toggled, this, &MainWindow::generatePassword);


    ui->radioButtonBasic->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBasicPassword()
{
    if (!ui->radioButtonBasic->isChecked())
        return;

    ui->spinBox->setMinimum(5);
    if (ui->spinBox->value() < 5)
        ui->spinBox->setValue(5);
}

void MainWindow::onStrongPassword()
{
    if (!ui->radioButtonStrong->isChecked())
        return;

    ui->spinBox->setMinimum(8);
    if (ui->spinBox->value() < 8)
        ui->spinBox->setValue(8);

    ui->checkBoxLowerCase->setChecked(true);
    ui->checkBoxUppercase->setChecked(true);
    ui->checkBoxNumbers->setChecked(true);
    ui->checkBoxSymbols->setChecked(true);
}

void MainWindow::generatePassword()
{
    const QString lowerChars = "abcdefghijklmnopqrstuvwxyz";
    const QString upperChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const QString numberChars = "0123456789";
    const QString symbolChars = "!@#$%^&*()_+-=[]{}|;:,.<>?";

    bool hasLower = ui->checkBoxLowerCase->isChecked();
    bool hasUpper = ui->checkBoxUppercase->isChecked();
    bool hasNumbers = ui->checkBoxNumbers->isChecked();
    bool hasSymbols = ui->checkBoxSymbols->isChecked();
    bool onlySelected = ui->checkBoxOnly->isChecked();
    int length = ui->spinBox->value();

    if (length <= 0) {
        return;
    }

    QString allowedPool;

    if (onlySelected) {
        // Pouzije pouze znaky ze zaskrtlych skupin
        if (hasLower) allowedPool += lowerChars;
        if (hasUpper) allowedPool += upperChars;
        if (hasNumbers) allowedPool += numberChars;
        if (hasSymbols) allowedPool += symbolChars;

        if (allowedPool.isEmpty()) {
            QMessageBox::warning(this, "Chyba", "Musíte vybrat alespoň jednu skupinu znaků!");
            return;
        }
    } else {
        // Pokud neni zaskrtly Only, muze pouzivat vsechny skupiny
        allowedPool = lowerChars + upperChars + numberChars + symbolChars;
    }

    QString password;

    // Silne heslo ma podminku: musi obsahovat alespon jeden znak z povinnych skupin
    if (ui->radioButtonStrong->isChecked()) {
        QVector<QString> requiredGroups;
        if (allowedPool.contains(lowerChars[0])) requiredGroups.append(lowerChars);
        if (allowedPool.contains(upperChars[0])) requiredGroups.append(upperChars);
        if (allowedPool.contains(numberChars[0])) requiredGroups.append(numberChars);
        if (allowedPool.contains(symbolChars[0])) requiredGroups.append(symbolChars);

        for (int i = 0; i < requiredGroups.size() && password.length() < length; ++i) {
            const QString &group = requiredGroups[i];
            int idx = QRandomGenerator::global()->bounded(group.length());
            password.append(group[idx]);
        }
    }

    // U bezneho hesla nejsou zadne dalsi podminky - generuje se ciste z allowedPool
    while (password.length() < length) {
        int idx = QRandomGenerator::global()->bounded(allowedPool.length());
        password.append(allowedPool[idx]);
    }

    // Promichat znaky
    for (int i = password.length() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        std::swap(password[i], password[j]);
    }

    ui->lineEdit->setText(password);
    ui->statusbar->showMessage("Heslo bylo vygenerováno.", 3000);
}

void MainWindow::copyToClipboard()
{
    QString password = ui->lineEdit->text();
    if (!password.isEmpty()) {
        QApplication::clipboard()->setText(password);
        ui->statusbar->showMessage("Heslo bylo zkopírováno do schránky.", 3000);
    } else {
        ui->statusbar->showMessage("Nejprve vygenerujte heslo.", 3000);
    }
}
