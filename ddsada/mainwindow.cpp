#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItem>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->comboBox->currentText()//metoda na ziskani aktualne textu v comboboxe

    //chceme tabulku 3x4 data od 1-12
    mModelStandardItem = new QStandardItemModel(this);
    for (int i = 0; i < 3; ++i) {
        QList<QStandardItem*> row;
        for (int j = i*4; j < (i+1)*4; ++j) {
            QStandardItem * item = new QStandardItem(QString::number(j+1));
            item->setToolTip("klikni na zvyseni o jedna");
            item->setData(j+1, Qt::UserRole);
            row.append(item);
        }
        // 4. Přidáme celý řádek najednou do modelu
        mModelStandardItem->appendRow(row);
    }

    //mModelStandardItem->sort(0);
    //nastavizipme jmena sloupecku
    QStringList headersNames;
    headersNames.append("prvy");
    headersNames.append("druhy");
    headersNames.append("treti");
    headersNames.append("ctvrty");
    mModelStandardItem->setHorizontalHeaderLabels(headersNames);

    //tabulka sa roztahne
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //skrijeme cislovani radku v tabulke
    ui->tableView->verticalHeader()->setVisible(false);

    ui->tableView->setModel(mModelStandardItem);

    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableViewItemClicked);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::onTableViewDoubleClicked);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onHeaderClicked);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onCurrencyChange);
    ///  connect(ui->comboBox, &QComboBox::currentTextChanged....)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTableViewItemClicked(const QModelIndex &index)
{
    //po kliknuti zvysujeme polozku o jedna
    //na indexe je polozka na ktoru sme klikli
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    //ziskame zobrazovane data z itemu
    int value = item->data(Qt::UserRole).toInt();
    ++value;
    item->setData(value, Qt::UserRole);
    item->setData(QString::number(value) + " " + ui->comboBox->currentText(), Qt::DisplayRole);
    //nastavime novy tool tip
    item->setData("po kliknuti", Qt::ToolTipRole);
}

void MainWindow::onTableViewDoubleClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    //ziskame zobrazovane data z itemu
    int value = (item->data(Qt::UserRole).toInt() - 1)*2;
    item->setData(value, Qt::UserRole);
    item->setData(QString::number(value) + " " + ui->comboBox->currentText(), Qt::DisplayRole);
    //nastavime novy tool tip
    item->setData("po kliknuti", Qt::ToolTipRole);
}

void MainWindow::onHeaderClicked(int logicalIndex)
{
    for(int i = 0; i < mModelStandardItem->rowCount(); ++i) {
        QStandardItem *item = mModelStandardItem->item(i, logicalIndex);
        //vyzaduje aby data byla ulozena jako Qt::UserRole
        int value = item->data(Qt::UserRole).toInt() * 2;
        item->setData(value, Qt::UserRole);
        item->setData(QString::number(value) + " " + ui->comboBox->currentText(), Qt::DisplayRole);
    }
}

void MainWindow::onCurrencyChange()
{
    for(int i = 0; i < mModelStandardItem->rowCount(); ++i)
    {
        for(int j = 0; j < mModelStandardItem->columnCount(); ++j) {
            QStandardItem *item = mModelStandardItem->item(i, j);
            //vyzaduje aby data byla ulozena jako Qt::UserRole
            QString value = item->data(Qt::UserRole).toString();
            item->setData(value + " " + ui->comboBox->currentText(), Qt::DisplayRole);

            //min hezka varianta, pracujeme vsak jenom s Qt::Displarole datami
            // QString data = item->data(Qt::DisplayRole).toString();
            //rozdelime data na value a menu string
            // QStringList dataList = data.split(" ");
            // item->setData(dataList[0] + " " + ui->comboBox->currentText(), Qt::DisplayRole);
        }
    }
}
