#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->checkBoxBacon, &QCheckBox::toggled, this, &MainWindow::order);
    connect(ui->checkBoxCheese, &QCheckBox::toggled, this, &MainWindow::order);
    connect(ui->checkBoxHam, &QCheckBox::toggled, this, &MainWindow::order);
    connect(ui->checkBoxSampiony, &QCheckBox::toggled, this, &MainWindow::order);
    connect(ui->checkBoxSalami, &QCheckBox::toggled, this, &MainWindow::order);

    connect(ui->radioButtonCheese, &QRadioButton::toggled, this, &MainWindow::order);
    connect(ui->radioButtonKebab, &QRadioButton::toggled, this, &MainWindow::order);
    connect(ui->radioButtonHam, &QRadioButton::toggled, this, &MainWindow::order);
    connect(ui->radioButtonMushrooms, &QRadioButton::toggled, this, &MainWindow::order);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::order()
{ QString pizza = "Pizza je: ";
    int price = 0;

    if(ui->radioButtonCheese->isChecked()){
        pizza = pizza + "Syrova + ";
        price = 200;
    };
    if(ui->radioButtonKebab->isChecked()){
        pizza = pizza + "Kebab + ";
        price = 200;
    };
    if(ui->radioButtonMushrooms->isChecked()){
        pizza = pizza + "Houbova + ";
        price = 200;
    };
    if(ui->radioButtonHam->isChecked()){
        pizza = pizza + "Sunkova + ";
        price = 200;
    };

    QString addons = "Priloha je: ";

    if(ui->checkBoxBacon->isChecked()){
        addons = addons + "slanina, ";
        price += 15;
    }
    if(ui->checkBoxHam->isChecked()){
        addons = addons + "sunka, ";
        price += 15;
    }
    if(ui->checkBoxCheese->isChecked()){
        addons = addons + "syr, ";
        price += 15;
    }
    if(ui->checkBoxSampiony->isChecked()){
        addons = addons + "sampiony, ";
        price += 15;
    }
    if(ui->checkBoxSalami->isChecked()){
        addons = addons + "salam, ";
        price += 15;
    }

    ui->labelSum->setText("Vybrali jste " + pizza + addons);
    ui->labelCost->setText("Cena : " + QString::number(price));

}
