#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList data;
    data.append("chleb");
    data.append("rohlik");
    data.append("maslo");
    data.append("sunka");
    data.append("syr");
    data.append("okurka");
    // mModelStringList = new QStringListModel(this);
    // mModelStringList->setStringList(data);
    // ui->listView->setModel(mModelStringList);
    // connect(ui->listView, &QListView::clicked, this, &MainWindow::onListViewItemClicked);
    mModelStandardItem = new QStandardItemModel(this);
    for(int i = 0; i < data.size(); ++i){
        QStandardItem * item = new QStandardItem(data[i]);
        item->setCheckable(true);
        mModelStandardItem->appendRow(item);
    }
    ui->listView->setModel(mModelStandardItem);
    connect(ui->listView, &QListView::clicked, this, &MainWindow::onListViewItemClicked);
}

void MainWindow::onListViewItemClicked(const QModelIndex &index){
    QString result;
    for(int i = 0; i < mModelStandardItem->rowCount(); i++)
    {
        QStandardItem *item = mModelStandardItem->item(i);
        if(item->checkState() == Qt::Checked)
            result += item->data(Qt::DisplayRole).toString() + " ";
    }
    ui->labelInfo->setText(result);
}


MainWindow::~MainWindow()
{
    delete ui;
}
